#include "config.h"

#include <atomic>

#include <Arduino.h>
#include <Wire.h>
#include <CoreMutex.h>

extern "C" {
#include <pio_usb.h>
}

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>

#include "bindings.h"
#include "buzzer.h"
#include "settings.h"
#include "state.h"
#include "sunk.h"
#include "splash.xbm"
#include "logo.xbm"

const char *const MODIFIER_NAMES[] = {
  "CtrlL", "ShiftL", "AltL", "GuiL",
  "CtrlR", "ShiftR", "AltR", "GuiR",
};

const char *const BUTTON_NAMES[] = {
  "LeftClick", "RightClick", "MiddleClick", "MouseBack", "MouseForward",
};

// USB Host object
Adafruit_USBH_Host USBHost;
// holding device descriptor
tusb_desc_device_t desc_device;

static Adafruit_SSD1306 display(128, 32, &Wire, /* OLED_RESET */ -1);

std::atomic<bool> wait = true;

State state;
Buzzer buzzer;
Settings settings;
__attribute__((section(".mutex_array"))) mutex_t buzzerMutex;
__attribute__((section(".mutex_array"))) mutex_t settingsMutex;
struct {
  bool ok;
} fake;

void setup() {
  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  analogWriteRange(100);
  pinMode(POWER_KEY, OUTPUT);

  Wire.setSCL(DISPLAY_SCL);
  Wire.setSDA(DISPLAY_SDA);
  display.begin(SSD1306_SWITCHCAPVCC, /* SCREEN_ADDRESS */ 0x3C);
  display.setRotation(DISPLAY_ROTATION);
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.clearDisplay();
  display.drawXBitmap(0, 0, splash_bits, 128, 32, SSD1306_WHITE);
  display.display();

#if defined(PICOPROBE_ENABLE)
  Serial1.end(); // needed under CFG_TUSB_DEBUG
  Serial1.setPinout(PICOPROBE_TX, PICOPROBE_RX);
  Serial1.setFIFOSize(4096);
  Serial1.begin(115200, SERIAL_8N1);
#elif defined(SUNK_ENABLE)
  // gpio invert must be set *after* setPinout/begin
  Serial1.setPinout(SUN_KTX, SUN_KRX);
  Serial1.begin(1200, SERIAL_8N1);
  gpio_set_outover(SUN_KTX, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(SUN_KRX, GPIO_OVERRIDE_INVERT);
#endif
#if defined(FAKE_SUN_ENABLE)
  // gpio invert must be set *after* setPinout/begin
  Serial2.setPinout(FAKE_SUN_KRX, FAKE_SUN_KTX);
  Serial2.begin(1200, SERIAL_8N1);
  gpio_set_outover(FAKE_SUN_KRX, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(FAKE_SUN_KTX, GPIO_OVERRIDE_INVERT);
#elif defined(SUNM_ENABLE)
  // gpio invert must be set *after* setPinout/begin
  Serial2.setPinout(SUN_MTX, SUN_MRX);
  Serial2.begin(SUNM_BAUD, SERIAL_8N1);
  gpio_set_outover(SUN_MTX, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(SUN_MRX, GPIO_OVERRIDE_INVERT);
#endif

  for (int i = 0; i < splash_height; i += 2) {
    for (int j = 0; j < splash_width; j += 1) {
      if (splash_bits[(splash_width + 7) / 8 * i + j / 8] >> j % 8 & 1)
        Sprintf("\033[7m ");
      else
        Sprintf("\033[0m ");
    }
    Sprintf("\033[0m\n");
  }

  Settings::begin();
  settings.readAll();

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

template<typename... Args>
void drawMenuItem(int16_t x, int16_t y, bool on, const char *fmt, Args... args) {
  if (on) {
    display.fillRect(x + 4, y, 120, 8, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(x + 8, y);
    display.printf(fmt, args...);
  } else {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(x + 8, y);
    display.printf(fmt, args...);
  }
}

template<typename... Args>
void drawMenuItem(unsigned i, const char *fmt, Args... args) {
  if (i >= state.topMenuItem && i <= state.topMenuItem + 2)
    drawMenuItem(0, 8 * (1 + i - state.topMenuItem), state.selectedMenuItem == i, fmt, args...);
}

void loop() {
  const auto t = micros();
  display.clearDisplay();
  display.drawXBitmap(0, 0, logo_bits, 64, 16, SSD1306_WHITE);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0, 0);
  // static int i = 0;
  // display.printf("#%d @%lu", i++, t / 1'000);
  // display.printf("usb3sun%c", t / 500'000 % 2 == 1 ? '.' : ' ');
  if (state.inMenu) {
    display.setCursor(0, 8);
    unsigned int i = 0;
    drawMenuItem(i++, "Go back");
    drawMenuItem(i++, "Force click: %s",
      settings.forceClick() == ForceClick::_::NO ? "no"
      : settings.forceClick() == ForceClick::_::OFF ? "off"
      : settings.forceClick() == ForceClick::_::ON ? "on"
      : "?");
    drawMenuItem(i++, "Click duration: %u ms", settings.clickDuration());
    drawMenuItem(i++, "Reprogram idprom");
    drawMenuItem(i++, "Wipe idprom (AAh)");
  } else {
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
  display.display();
  delay(10);

#ifdef FAKE_SUN_ENABLE
  if (!fake.ok) {
    Serial2.write(SUNK_RESET);
  }
#endif
}

#ifdef SUNK_ENABLE
void serialEvent1() {
  while (Serial1.available() > 0) {
    uint8_t command = Serial1.read();
    Sprintf("sun keyboard: rx command %02Xh\n", command);
    switch (command) {
      case SUNK_RESET:
        // self test fail:
        // Serial.write(0x7E);
        // Serial.write(0x01);
        Serial1.write(SUNK_RESET_RESPONSE);
        Serial1.write(0x04);
        Serial1.write(0x7F); // TODO optional make code
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
        while (Serial1.peek() == -1) delay(1);
        uint8_t status = Serial1.read();
        Sprintf("sun keyboard: led status %02Xh\n", status);
        state.num = status & 1 << 0;
        state.compose = status & 1 << 1;
        state.scroll = status & 1 << 2;
        state.caps = status & 1 << 3;
      } break;
      case SUNK_LAYOUT: {
        Serial1.write(SUNK_LAYOUT_RESPONSE);
        // UNITED STATES (TODO alternate layouts)
        uint8_t layout = 0b00000000;
        Serial1.write(&layout, 1);
      } break;
    }
  }
}
#endif

#ifdef FAKE_SUN_ENABLE
void serialEvent2() {
  while (Serial2.available() > 0) {
    uint8_t command = Serial2.read();
    Sprintf("fake: rx command %02Xh\n", command);
    switch (command) {
      case SUNK_IDLE:
        break;
      case SUNK_LAYOUT_RESPONSE:
        break;
      case SUNK_RESET_RESPONSE:
        while (Serial2.read() == -1) delay(1);
        while (Serial2.read() == -1) delay(1);
        fake.ok = true;
        break;
      case SUNK_NUMLOCK:
        state.num = !state.num;
        break;
      case SUNK_CAPSLOCK:
        state.caps = !state.caps;
        break;
      case SUNK_SCROLLLOCK:
        state.scroll = !state.scroll;
        break;
    }
  }
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
  USBHost.task();
  buzzer.update();
}

void sunkSend(bool make, uint8_t code) {
  static int activeCount = 0;
  if (make) {
    activeCount += 1;
    buzzer.click();
  } else {
    activeCount -= 1;
    code |= SUNK_BREAK_BIT;
  }

#ifdef SUNK_ENABLE
  Sprintf("sun keyboard: tx command %02Xh\n", code);
  Serial1.write(code);
#else
  Sprintf("sun keyboard: tx command %02Xh (disabled)\n", code);
#endif

  if (activeCount == 0) {
#ifdef SUNK_ENABLE
  Sprintf("sun keyboard: idle\n");
  Serial1.write(SUNK_IDLE);
#else
  Sprintf("sun keyboard: idle (disabled)\n");
#endif
  }

  switch (code) {
    case SUNK_POWER:
      Sprintf("sun power: high\n");
      digitalWrite(POWER_KEY, HIGH);
      break;
    case SUNK_POWER | SUNK_BREAK_BIT:
      Sprintf("sun power: low\n");
      digitalWrite(POWER_KEY, LOW);
      break;
  }
}

template <typename... Args>
void sunkSend(const char *fmt, Args... args) {
  char result[1024];
  size_t len = snprintf(result, sizeof(result) / sizeof(*result), fmt, args...);
  if (len >= sizeof(result) / sizeof(*result)) {
    Sprintf("error: sun keyboard: snprintf buffer overflow\n");
    return;
  }
  for (auto i = 0; i < len; i++) {
    if (result[i] >= sizeof(SUNK_ASCII) / sizeof(*SUNK_ASCII) || SUNK_ASCII[result[i]] == 0) {
      Sprintf("error: sun keyboard: octet %02Xh not in SUNK_ASCII\n", result[i]);
      return;
    }
  }
  for (auto i = 0; i < len; i++) {
    if (SUNK_ASCII[result[i]] & SUNK_SEND_SHIFT) {
      delay(50);
      sunkSend(true, SUNK_SHIFT_L);
      sunkSend(true, SUNK_SHIFT_L);
      sunkSend(true, SUNK_SHIFT_L);
      delay(50);
    }
    sunkSend(true, SUNK_ASCII[result[i]] & 0xFF);
    sunkSend(false, SUNK_ASCII[result[i]] & 0xFF);
    if (SUNK_ASCII[result[i]] & SUNK_SEND_SHIFT) {
      delay(50);
      sunkSend(false, SUNK_SHIFT_L);
      sunkSend(false, SUNK_SHIFT_L);
      sunkSend(false, SUNK_SHIFT_L);
      delay(50);
    }
  }
}

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  (void)desc_report;
  (void)desc_len;
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  Sprintf("usb [%u:%u]: hid mount vid:pid=%04x:%04x\n", dev_addr, instance, vid, pid);

  tuh_hid_report_info_t infos[16];
  size_t infos_len = tuh_hid_parse_report_descriptor(infos, sizeof(infos) / sizeof(*infos), desc_report, desc_len);
  for (size_t i = 0; i < infos_len; i++)
    Sprintf("      report[%zu] report_id=%u usage=%02Xh usage_page=%04Xh\n", infos[i].report_id, infos[i].usage, infos[i].usage_page);

  if (!tuh_hid_receive_report(dev_addr, instance))
    Sprintf("error: usb [%u:%u]: failed to request to receive report\n", dev_addr, instance);

  buzzer.plug();
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  Sprintf("usb [%u:%u]: hid unmount\n", dev_addr, instance);
}

void tuh_mount_cb(uint8_t dev_addr) {
  Sprintf("usb [%u]: mount\n", dev_addr);
}

void tuh_umount_cb(uint8_t dev_addr) {
  Sprintf("usb [%u]: unmount\n", dev_addr);
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

      for (int i = 0; i < 6; i++) {
        if (kreport->keycode[i] != USBK_RESERVED && kreport->keycode[i] < USBK_FIRST_KEYCODE) {
#ifdef UHID_VERBOSE
          Sprintf(" !%u", kreport->keycode[i]);
#endif
          goto out;
        }
      }

      struct {
        uint8_t usbkModifier;
        bool make;
      } modifierChanges[8 * 2];
      struct {
        uint8_t usbkSelector;
        bool make;
      } selectorChanges[6 * 2];
      size_t modifierChangesLen = 0;
      size_t selectorChangesLen = 0;

      for (int i = 0; i < 8; i++) {
        if ((state.lastModifiers & 1 << i) != (kreport->modifier & 1 << i)) {
#ifdef UHID_VERBOSE
          Sprintf(" %c%s", kreport->modifier & 1 << i ? '+' : '-', MODIFIER_NAMES[i]);
#endif
          modifierChanges[modifierChangesLen++] = {(uint8_t) (1u << i), kreport->modifier & 1 << i ? true : false};
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
          selectorChanges[selectorChangesLen++] = {state.lastKeys[i], false};
        }
        if (!newInOlds && kreport->keycode[i] >= USBK_FIRST_KEYCODE) {
#ifdef UHID_VERBOSE
          Sprintf(" +%u", kreport->keycode[i]);
#endif
          selectorChanges[selectorChangesLen++] = {kreport->keycode[i], true};
        }
      }

#ifdef UHID_VERBOSE
      Sprintln();
#endif

#ifdef SUNK_ENABLE
      for (int i = 0; i < modifierChangesLen; i++) {
        // for DV bindings, make when key makes and break when key breaks
        for (int j = 0; j < sizeof(DV_BINDINGS) / sizeof(*DV_BINDINGS); j++)
          if (DV_BINDINGS[j].usbkModifier == modifierChanges[i].usbkModifier)
            sunkSend(modifierChanges[i].make, DV_BINDINGS[j].sunkMake);
      }
#endif

      // treat simultaneous DV and Sel changes as DV before Sel, for DV+Sel bindings
      state.lastModifiers = kreport->modifier;

      for (int i = 0; i < selectorChangesLen; i++) {
        if (state.inMenu) {
          if (selectorChanges[i].make) {
            switch (selectorChanges[i].usbkSelector) {
              case USBK_RIGHT:
                switch (state.selectedMenuItem) {
                  case 1:
                    ++settings.forceClick();
                    settings.write(settings.forceClick_field);
                    break;
                  case 2:
                    if (settings.clickDuration() < 96u) {
                      settings.clickDuration() += 5u;
                      settings.write(settings.clickDuration_field);
                      buzzer.click();
                    }
                    break;
                }
                break;
              case USBK_LEFT:
                switch (state.selectedMenuItem) {
                  case 1:
                    --settings.forceClick();
                    settings.write(settings.forceClick_field);
                    break;
                  case 2:
                    if (settings.clickDuration() > 4u) {
                      settings.clickDuration() -= 5u;
                      settings.write(settings.clickDuration_field);
                      buzzer.click();
                    }
                    break;
                }
                break;
              case USBK_DOWN:
                if (state.selectedMenuItem < state.menuItemCount - 1u)
                  state.selectedMenuItem += 1u;
                if (state.selectedMenuItem - state.topMenuItem > 2u)
                  state.topMenuItem += 1u;
                break;
              case USBK_UP:
                if (state.selectedMenuItem > 0u)
                  state.selectedMenuItem -= 1u;
                if (state.selectedMenuItem < state.topMenuItem)
                  state.topMenuItem -= 1u;
                break;
              case USBK_RETURN:
              case USBK_ENTER:
                switch (state.selectedMenuItem) {
                  // case N:
                  //   state.foo = !state.foo;
                  //   break;
                  case 3: {
                    // uint32_t serial = 0x538F1242u; // australis
                    // uint32_t serial = 0x507FA05Cu; // borealis
                    // uint32_t hostid24 = (((serial >> 28 & 0xF) * 53 + (serial >> 20 & 0xFF)) & 0xFF) << 16
                    //   | (serial & 0xFFFF);

                    uint32_t hostid24 = 0x76FBC1u; // australis
                    // uint32_t hostid24 = 0x717878u; // borealis
                    unsigned i = 0;

                    // http://www.alyon.asso.fr/InfosTechniques/informatique/SunHardwareReference/sun-nvram-hostid.faq

                    // version 1
                    sunkSend("1 %x mkp\n", i++);

                    // hostid byte 1/4 (system type)
                    sunkSend("real-machine-type %x mkp\n", i++);

                    // ethernet address oui (always 08:00:20)
                    sunkSend("8 %x mkp\n", i++);
                    sunkSend("0 %x mkp\n", i++);
                    sunkSend("20 %x mkp\n", i++);

                    // ethernet address lower half
                    sunkSend("%x %x mkp\n", hostid24 >> 16 & 0xFF, i++);
                    sunkSend("%x %x mkp\n", hostid24 >> 8 & 0xFF, i++);
                    sunkSend("%x %x mkp\n", hostid24 >> 0 & 0xFF, i++);

                    // set date of manufacture such that it and the system type byte
                    // cancel each other out in the checksum
                    sunkSend("real-machine-type %x mkp\n", i++);
                    sunkSend("0 %x mkp\n", i++);
                    sunkSend("0 %x mkp\n", i++);
                    sunkSend("0 %x mkp\n", i++);

                    // hostid bytes 2/3/4
                    sunkSend("%x %x mkp\n", hostid24 >> 16 & 0xFF, i++);
                    sunkSend("%x %x mkp\n", hostid24 >> 8 & 0xFF, i++);
                    sunkSend("%x %x mkp\n", hostid24 >> 0 & 0xFF, i++);

                    // shift key buggy, so use simplified approach below
                    // sunkSend("0 %x 0 do i idprom@ xor loop f mkp\n", i++);

                    // if your ethernet address 08:00:20:xx:yy:zz = 
                    sunkSend("29 %x mkp\n", i++);

                    // only needed for SS1000, but harmless otherwise
                    sunkSend("update-system-idprom\n");

                    sunkSend(".idprom\n");
                    sunkSend("banner\n");
                  } break;
                  case 4: {
                    for (unsigned i = 0; i <= 0xF; i++)
                      sunkSend("aa %x mkp\n", i);
                  } break;
                }
                state.inMenu = false;
                break;
            }
          }
          continue;
        }

        // CtrlR+Space acts like a DV+Sel binding, but opens the settings menu
        if (selectorChanges[i].usbkSelector == USBK_SPACE && state.lastModifiers == USBK_CTRL_R) {
          state.inMenu = !state.inMenu;
          state.selectedMenuItem = 0u;
          state.topMenuItem = 0u;
          continue;
        }

        bool consumedByDvSel = false;

        // for DV+Sel bindings:
        // • make when the Sel key makes and the old DV keys are equal
        // • does not make when the Sel key makes and the old DV keys are proper superset
        // • does not make when the DV key makes after the Sel key makes
        // • break when the Sel key breaks and the old modifiers are equal
        // • FIXME does not break when the DV key breaks before the Sel key breaks!
        for (int j = 0; j < sizeof(DV_SEL_BINDINGS) / sizeof(*DV_SEL_BINDINGS); j++) {
          if (DV_SEL_BINDINGS[j].usbkSelector == selectorChanges[i].usbkSelector && DV_SEL_BINDINGS[j].usbkModifier == state.lastModifiers) {
            sunkSend(selectorChanges[i].make, DV_SEL_BINDINGS[j].sunkMake);
            consumedByDvSel = true;
          }
        }

        // for Sel bindings
        // • make when key makes and break when key breaks
        // • does not make or break when key was consumed by a DV+Sel binding
        if (!consumedByDvSel)
          for (int j = 0; j < sizeof(SEL_BINDINGS) / sizeof(*SEL_BINDINGS); j++)
            if (SEL_BINDINGS[j].usbkSelector == selectorChanges[i].usbkSelector)
              sunkSend(selectorChanges[i].make, SEL_BINDINGS[j].sunkMake);
      }

      // finally commit the Sel changes
      for (int i = 0; i < 6; i++)
        state.lastKeys[i] = kreport->keycode[i];
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
      size_t len = Serial2.write(result, sizeof(result) / sizeof(*result));
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
