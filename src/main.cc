#include "config.h"

#include <atomic>

#include <Arduino.h>
#include <Wire.h>
#include <CoreMutex.h>
#include <SerialPIO.h>

extern "C" {
#include <pio_usb.h>
}

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>

#include "bindings.h"
#include "buzzer.h"
#include "display.h"
#include "menu.h"
#include "pinout.h"
#include "settings.h"
#include "state.h"
#include "sunk.h"
#include "view.h"
#include "splash.xbm"
#include "logo.xbm"

const char *const MODIFIER_NAMES[] = {
  "CtrlL", "ShiftL", "AltL", "GuiL",
  "CtrlR", "ShiftR", "AltR", "GuiR",
};

const char *const BUTTON_NAMES[] = {
  "LeftClick", "RightClick", "MiddleClick", "MouseBack", "MouseForward",
};

enum class Message : uint32_t {
  UHID_LED_FROM_STATE,
  UHID_LED_ALL_OFF,
  UHID_LED_ALL_ON,
};

// core 1 only
Adafruit_USBH_Host USBHost;
struct {
  bool present = false;
  uint8_t dev_addr;
  uint8_t instance;
  uint8_t if_protocol;
} hid[16];

std::atomic<bool> wait = true;

Pinout pinout;
State state;
Buzzer buzzer;
Settings settings;
__attribute__((section(".mutex_array"))) mutex_t buzzerMutex;
__attribute__((section(".mutex_array"))) mutex_t settingsMutex;

void drawStatus(int16_t x, int16_t y, const char *label, bool on);
void sunkSend(bool make, uint8_t code);

struct DefaultView : View {
  void handlePaint() override {
    drawStatus(78, 0, "CLK", state.clickEnabled);
    drawStatus(104, 0, "BEL", state.bell);
    drawStatus(0, 18, "CAP", state.caps);
    drawStatus(26, 18, "CMP", state.compose);
    drawStatus(52, 18, "SCR", state.scroll);
    drawStatus(78, 18, "NUM", state.num);
    if (buzzer.current != Buzzer::_::NONE) {
      const auto x = 106;
      const auto y = 16;
      display.fillRect(x + 6, y + 1, 2, 11, SSD1306_WHITE);
      display.fillRect(x + 5, y + 2, 4, 10, SSD1306_WHITE);
      display.fillRect(x + 4, y + 4, 6, 8, SSD1306_WHITE);
      display.fillRect(x + 2, y + 9, 10, 3, SSD1306_WHITE);
      display.fillRect(x + 1, y + 10, 12, 2, SSD1306_WHITE);
      display.fillRect(x + 5, y + 13, 4, 1, SSD1306_WHITE);
    } else {
      display.fillRect(106, 16, 14, 14, SSD1306_BLACK);
    }
  }

  void handleKey(const UsbkChanges &changes) override {
    unsigned long t = micros();

#ifdef SUNK_ENABLE
    for (int i = 0; i < changes.dvLen; i++) {
      // for DV bindings, make when key makes and break when key breaks
      if (uint8_t sunkMake = USBK_TO_SUNK.dv[changes.dv[i].usbkModifier])
        sunkSend(changes.dv[i].make, sunkMake);
    }
#endif

    // treat simultaneous DV and Sel changes as DV before Sel, for special bindings
    const uint8_t lastModifiers = changes.kreport->modifier;

    for (int i = 0; i < changes.selLen; i++) {
      const uint8_t usbkSelector = changes.sel[i].usbkSelector;
      const uint8_t make = changes.sel[i].make;

      // CtrlR+Space acts like a special binding, but opens the settings menu
      // note: no other modifiers are allowed, to avoid getting them stuck down
      if (changes.sel[i].usbkSelector == USBK_SPACE && lastModifiers == USBK_CTRL_R) {
        MENU_VIEW.open();
        continue;
      }

      static bool specialBindingIsPressed[256]{};
      bool consumedBySpecialBinding[256]{};

      // for special bindings (CtrlR+Sel):
      // • make when the Sel key makes and the DV keys include CtrlR
      // • break when the Sel key breaks, even if the DV keys no longer include CtrlR
      // • do not make when CtrlR makes after the Sel key makes
      if (uint8_t sunkMake = USBK_TO_SUNK.special[usbkSelector]) {
        if (make && !!(state.lastModifiers & USBK_CTRL_R)) {
          sunkSend(true, sunkMake);
          specialBindingIsPressed[usbkSelector] = true;
          consumedBySpecialBinding[usbkSelector] = true;
        } else if (!make && specialBindingIsPressed[usbkSelector]) {
          sunkSend(false, sunkMake);
          specialBindingIsPressed[usbkSelector] = false;
          consumedBySpecialBinding[usbkSelector] = true;
        }
      }

      // for Sel bindings
      // • make when key makes and break when key breaks
      // • do not make or break when key was consumed by the corresponding special binding
      if (uint8_t sunkMake = USBK_TO_SUNK.sel[usbkSelector])
        if (!consumedBySpecialBinding[usbkSelector])
          sunkSend(make, sunkMake);
    }

#ifdef DEBUG_TIMINGS
    Sprintf("sent in %lu\n", micros() - t);
#endif
  }
};

static DefaultView DEFAULT_VIEW{};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // check for pinout v2 (active high)
  pinMode(PINOUT_V2_PIN, INPUT_PULLDOWN);
  if (digitalRead(PINOUT_V2_PIN) == HIGH) {
    pinout.v2();
  } else {
    pinout.v1();
  }

  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);

  analogWriteRange(100);
  pinMode(POWER_KEY, OUTPUT);

  Wire.setSCL(DISPLAY_SCL);
  Wire.setSDA(DISPLAY_SDA);

  // set DISPLAY_RES# high to turn on the display via Q7.
  // some display modules need delay to start reliably. for example, i have one module with a C9 on
  // its pcb that needs no delay, but i have another without C9 that stays black every other reset
  // unless given 15 ms of delay. tested with Q7 = 2N7000, R18 = 4K7, resetting the pico in three
  // different patterns (reset/run ms): 50/200, 250/750, 3000/1000. let’s double that just in case.
  pinMode(DISPLAY_RES, OUTPUT);
  digitalWrite(DISPLAY_RES, HIGH);
  delay(30);

  display.begin(SSD1306_SWITCHCAPVCC, /* SCREEN_ADDRESS */ 0x3C);
  display.setRotation(DISPLAY_ROTATION);
  display.cp437(true);
  display.setTextWrap(false);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.clearDisplay();
  display.drawXBitmap(0, 0, splash_bits, 128, 32, SSD1306_WHITE);
  display.display();

  Settings::begin();
  settings.readAll();
  pinout.beginSun();

  for (int i = 0; i < splash_height; i += 2) {
    for (int j = 0; j < splash_width; j += 1) {
      if (splash_bits[(splash_width + 7) / 8 * i + j / 8] >> j % 8 & 1)
        Sprintf("\033[7m ");
      else
        Sprintf("\033[0m ");
    }
    Sprintf("\033[0m\n");
  }

  View::push(&DEFAULT_VIEW);

#ifdef WAIT_PIN
  pinMode(WAIT_PIN, INPUT_PULLUP);
  while (digitalRead(WAIT_PIN));
#endif
#ifdef WAIT_SERIAL
  while (Serial.read() == -1);
#endif
  wait = false;

  digitalWrite(LED_BUILTIN, LOW);
}

void drawStatus(int16_t x, int16_t y, const char *label, bool on) {
  if (on) {
    display.fillRoundRect(x, y, 24, 14, 4, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(x + 3, y + 4);
    display.print(label);
  } else {
    display.drawRoundRect(x, y, 24, 14, 4, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(x + 3, y + 4);
    display.print(label);
  }
}

void loop() {
  const auto t = micros();
  display.clearDisplay();
  // display.drawXBitmap(0, 0, logo_bits, 64, 16, SSD1306_WHITE);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0, 0);
  display.print(USB3SUN_VERSION);
  // static int i = 0;
  // display.printf("#%d @%lu", i++, t / 1'000);
  // display.printf("usb3sun%c", t / 500'000 % 2 == 1 ? '.' : ' ');
  View::paint();
  display.display();

#ifdef UHID_LED_TEST
  static int z = 0;
  if (++z % 10 == 0) {
    Message message = z / 10 % 2 == 0
      ? Message::UHID_LED_ALL_OFF
      : Message::UHID_LED_ALL_ON;
    rp2040.fifo.push_nb((uint32_t)message);
  }
#endif

  delay(10);
}

#ifdef SUNK_ENABLE
void sunkEvent() {
  while (pinout.sunk->available() > 0) {
    uint8_t command = pinout.sunk->read();
    Sprintf("sun keyboard: rx command %02Xh\n", command);
    switch (command) {
      case SUNK_RESET:
        // self test fail:
        // pinout.sunk->write(0x7E);
        // pinout.sunk->write(0x01);
        pinout.sunk->write(SUNK_RESET_RESPONSE);
        pinout.sunk->write(0x04);
        pinout.sunk->write(0x7F); // TODO optional make code
        break;
      case SUNK_BELL_ON:
        state.bell = true;
        buzzer.update();
        break;
      case SUNK_BELL_OFF:
        state.bell = false;
        buzzer.update();
        break;
      case SUNK_CLICK_ON:
        state.clickEnabled = true;
        break;
      case SUNK_CLICK_OFF:
        state.clickEnabled = false;
        break;
      case SUNK_LED: {
        while (pinout.sunk->peek() == -1) delay(1);
        uint8_t status = pinout.sunk->read();
        Sprintf("sun keyboard: led status %02Xh\n", status);
        state.num = status & 1 << 0;
        state.compose = status & 1 << 1;
        state.scroll = status & 1 << 2;
        state.caps = status & 1 << 3;

        // ensure state update finished, then notify
        __dmb();
        rp2040.fifo.push_nb((uint32_t)Message::UHID_LED_FROM_STATE);
      } break;
      case SUNK_LAYOUT: {
        pinout.sunk->write(SUNK_LAYOUT_RESPONSE);
        // UNITED STATES (TODO alternate layouts)
        uint8_t layout = 0b00000000;
        pinout.sunk->write(&layout, 1);
      } break;
    }
  }
}

void serialEvent1() {
#if defined(SUNK_ENABLE)
  sunkEvent();
#endif
}

void serialEvent2() {
#if defined(SUNK_ENABLE)
  sunkEvent();
#endif
}
#endif

void setup1() {
  while (wait);

  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if (cpu_hz != 120000000uL && cpu_hz != 240000000uL) {
    Sprintf("error: cpu frequency %u, set [env:pico] board_build.f_cpu = 120000000L\n", cpu_hz);
    while (true) delay(1);
  }

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = USB0_DP;
  pio_cfg.sm_tx = 1;

  // tuh_configure -> pico pio hcd_configure -> memcpy to static global
  USBHost.configure_pio_usb(1, &pio_cfg);

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  // tuh_init -> pico pio hcd_init -> pio_usb_host_init -> pio_usb_bus_init -> set root[0]->initialized
  USBHost.begin(1);

  // set root[i]->initialized for the first unused i less than PIO_USB_ROOT_PORT_CNT
  pio_usb_host_add_port(USB1_DP);
}

void loop1() {
  uint32_t message;
  if (rp2040.fifo.pop_nb(&message)) {
    for (size_t i = 0; i < sizeof(hid) / sizeof(*hid); i++) {
      if (!hid[i].present || hid[i].if_protocol != HID_ITF_PROTOCOL_KEYBOARD)
        continue;
      uint8_t dev_addr = hid[i].dev_addr;
      uint8_t instance = hid[i].instance;
      uint8_t report;
      switch (message) {
        case (uint32_t)Message::UHID_LED_FROM_STATE:
          report =
            state.num << 0
            | state.caps << 1
            | state.scroll << 2
            | state.compose << 3;
          break;
        case (uint32_t)Message::UHID_LED_ALL_OFF:
          report = 0x00;
          break;
        case (uint32_t)Message::UHID_LED_ALL_ON:
          report = 0xFF;
          break;
      }
#ifndef UHID_VERBOSE
      Sprint("*");
#endif
#ifdef UHID_VERBOSE
      Sprintf("hid [%zu]: usb [%u:%u]: set led report %02Xh\n", i, dev_addr, instance, report);
#endif
      // TODO what report id? all values seem to work?
      tuh_hid_set_report(dev_addr, instance, 6, HID_REPORT_TYPE_OUTPUT, &report, sizeof(report));
    }
  }
  USBHost.task();
  buzzer.update();
}

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  Sprintf("usb [%u:%u]: hid mount vid:pid=%04x:%04x\n", dev_addr, instance, vid, pid);

  tuh_hid_report_info_t reports[16];
  size_t reports_len = tuh_hid_parse_report_descriptor(reports, sizeof(reports) / sizeof(*reports), desc_report, desc_len);
  for (size_t i = 0; i < reports_len; i++)
    Sprintf("    reports[%zu] report_id=%u usage=%02Xh usage_page=%04Xh\n", reports[i].report_id, reports[i].usage, reports[i].usage_page);

  // hid_subclass_enum_t if_subclass = ...;
  uint8_t if_protocol = tuh_hid_interface_protocol(dev_addr, instance);
  Sprintf("    bInterfaceProtocol=%u", if_protocol);
  switch (if_protocol) {
    case HID_ITF_PROTOCOL_KEYBOARD:
      Sprintln(" (boot keyboard)");
      break;
    case HID_ITF_PROTOCOL_MOUSE:
      Sprintln(" (boot mouse)");
      break;
    default:
      Sprintln();
  }

  // TODO non-boot input devices
  switch (if_protocol) {
    case HID_ITF_PROTOCOL_KEYBOARD:
    case HID_ITF_PROTOCOL_MOUSE: {
      bool ok = false;
      for (size_t i = 0; i < sizeof(hid) / sizeof(*hid); i++) {
        if (!hid[i].present) {
          Sprintf(
            "hid [%zu]: usb [%u:%u], bInterfaceProtocol=%u\n",
            i, dev_addr, instance, if_protocol
          );
          hid[i].dev_addr = dev_addr;
          hid[i].instance = instance;
          hid[i].if_protocol = if_protocol;
          hid[i].present = true;
          ok = true;
          break;
        }
      }
      if (!ok)
        Sprintln("error: usb [%u:%u]: hid table full");
    }
  }

  if (!tuh_hid_receive_report(dev_addr, instance))
    Sprintf("error: usb [%u:%u]: failed to request to receive report\n", dev_addr, instance);

  buzzer.plug();
}

// FIXME this never seems to get called?
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  Sprintf("usb [%u:%u]: hid unmount\n", dev_addr, instance);
}

void tuh_mount_cb(uint8_t dev_addr) {
  Sprintf("usb [%u]: mount\n", dev_addr);
}

void tuh_umount_cb(uint8_t dev_addr) {
  Sprintf("usb [%u]: unmount\n", dev_addr);
  for (size_t i = 0; i < sizeof(hid) / sizeof(*hid); i++) {
    if (hid[i].present && hid[i].dev_addr == dev_addr) {
      Sprintf("hid [%zu]: removing\n", i);
      hid[i].present = false;
    }
  }
  buzzer.unplug();
}

void tuh_hid_set_protocol_complete_cb(uint8_t dev_addr, uint8_t instance, uint8_t protocol) {
  // haven’t seen this actually get printed so far, but only tried a few devices
  Sprintf("usb [%u:%u]: hid set protocol returned %u\n", dev_addr, instance, protocol);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  uint8_t if_protocol = tuh_hid_interface_protocol(dev_addr, instance);
#ifndef UHID_VERBOSE
  Sprint(".");
#endif
#ifdef UHID_VERBOSE
  Sprintf("usb [%u:%u]: hid report if_protocol=%u", dev_addr, instance, if_protocol);
#endif
#ifdef UHID_VERBOSE
  for (uint16_t i = 0; i < len; i++)
    Sprintf(" %02Xh", report[i]);
#endif

  switch (if_protocol) {
    case HID_ITF_PROTOCOL_KEYBOARD: {
      hid_keyboard_report_t *kreport = (hid_keyboard_report_t *) report;

      unsigned long t = micros();

      for (int i = 0; i < 6; i++) {
        if (kreport->keycode[i] != USBK_RESERVED && kreport->keycode[i] < USBK_FIRST_KEYCODE) {
#ifdef UHID_VERBOSE
          Sprintf(" !%u", kreport->keycode[i]);
#endif
          goto out;
        }
      }

      UsbkChanges changes{};
      changes.kreport = kreport;

      for (int i = 0; i < 8; i++) {
        if ((state.lastModifiers & 1 << i) != (kreport->modifier & 1 << i)) {
#ifdef UHID_VERBOSE
          Sprintf(" %c%s", kreport->modifier & 1 << i ? '+' : '-', MODIFIER_NAMES[i]);
#endif
          changes.dv[changes.dvLen++] = {(uint8_t) (1u << i), kreport->modifier & 1 << i ? true : false};
        }
      }

      for (int i = 0; i < 6; i++) {
        bool oldInNews = false;
        bool newInOlds = false;
        for (int j = 0; j < 6; j++) {
          if (state.lastKeys[i] == kreport->keycode[j])
            oldInNews = true;
          if (kreport->keycode[i] == state.lastKeys[j])
            newInOlds = true;
        }
        if (!oldInNews && state.lastKeys[i] >= USBK_FIRST_KEYCODE) {
#ifdef UHID_VERBOSE
          Sprintf(" -%u", state.lastKeys[i]);
#endif
          changes.sel[changes.selLen++] = {state.lastKeys[i], false};
        }
        if (!newInOlds && kreport->keycode[i] >= USBK_FIRST_KEYCODE) {
#ifdef UHID_VERBOSE
          Sprintf(" +%u", kreport->keycode[i]);
#endif
          changes.sel[changes.selLen++] = {kreport->keycode[i], true};
        }
      }

#ifdef UHID_VERBOSE
      Sprintln();
#endif
#ifdef DEBUG_TIMINGS
      Sprintf("diffed in %lu\n", micros() - t);
#endif

      View::key(changes);

      // commit the DV and Sel changes
      state.lastModifiers = changes.kreport->modifier;
      for (int i = 0; i < 6; i++)
        state.lastKeys[i] = changes.kreport->keycode[i];
    } break;
    case HID_ITF_PROTOCOL_MOUSE: {
      hid_mouse_report_t *mreport = (hid_mouse_report_t *) report;
#ifdef UHID_VERBOSE
      Sprintf(" buttons=%u x=%d y=%d", mreport->buttons, mreport->x, mreport->y);
#endif

#ifdef UHID_VERBOSE
      for (int i = 0; i < 3; i++)
        if ((state.lastButtons & 1 << i) != (mreport->buttons & 1 << i))
          Sprintf(" %c%s", mreport->buttons & 1 << i ? '+' : '-', BUTTON_NAMES[i]);
      Sprintln();
#endif

      // correct: https://web.archive.org/web/20220226000612/http://www.bitsavers.org/pdf/mouseSystems/300771-001_Mouse_Systems_Optical_Mouse_Technical_Reference_Models_M2_and_M3_1985.pdf
      // wrong: https://web.archive.org/web/20100213183456/http://privatewww.essex.ac.uk/~nbb/mice-pc.html
      // note in particular that:
      // • positive dx is right, but positive dy is up
      // • buttons are 0 when pressed and 1 when released
      uint8_t result[] = {
        0x80
          | (mreport->buttons & USBM_LEFT ? 0 : SUNM_LEFT)
          | (mreport->buttons & USBM_MIDDLE ? 0 : SUNM_CENTER)
          | (mreport->buttons & USBM_RIGHT ? 0 : SUNM_RIGHT),
        (uint8_t) mreport->x, (uint8_t) -mreport->y, 0, 0,
      };
#ifdef SUNM_ENABLE
      size_t len = pinout.sunm->write(result, sizeof(result) / sizeof(*result));
#ifdef SUNM_VERBOSE
      Sprintf("sun mouse: tx %02Xh %02Xh %02Xh %02Xh %02Xh = %zu\n",
        result[0], result[1], result[2], result[3], result[4], len);
#endif
#else
      Sprintf("sun mouse: tx %02Xh %02Xh %02Xh %02Xh %02Xh (disabled)\n",
        result[0], result[1], result[2], result[3], result[4]);
#endif

      state.lastButtons = mreport->buttons;
    } break;
  }
out:
  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance))
    Sprintf("error: usb [%u:%u]: failed to request to receive report\n", dev_addr, instance);
}
