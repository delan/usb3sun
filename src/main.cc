#define SUN_MTX SUN_PIN4
#define SUN_KRX SUN_PIN5
#define SUN_KTX SUN_PIN6

// begin pinouts/configs
  // wait for some GPIO to be grounded on boot
  #define WAIT_PIN 22
  // wait for serial input on boot
  // #define WAIT_SERIAL

  #define SUN_PIN1 SUN_PURPLE   // 0 V
  #define SUN_PIN2 SUN_BROWN    // 0 V
  #define SUN_PIN3 SUN_BLUE     // +5 Vdc
  #define SUN_PIN4 SUN_GRAY     // mouse tx
  #define SUN_PIN5 SUN_RED      // keyboard rx
  #define SUN_PIN6 SUN_GREEN    // keyboard tx
  #define SUN_PIN7 SUN_YELLOW   // 0 V
  #define SUN_PIN8 SUN_ORANGE   // +5 Vdc

  #define SUN_GRAY 8            // must be a GP# valid for UART1 TX
  #define SUN_MRX_UNUSED 9      // must be a GP# valid for UART1 RX
  #define SUN_GREEN 12          // must be a GP# valid for UART0 TX
  #define SUN_RED 13            // must be a GP# valid for UART0 RX

  // fake Sun host for loopback testing (disables mouse support)
  #define FAKE_SUN_ENABLE
  #define FAKE_SUN_KRX 4        // must be a GP# valid for UART1 TX
  #define FAKE_SUN_KTX 5        // must be a GP# valid for UART1 RX

  #define BUZZER_PIN 28         // must be a GP#
  #define DISPLAY_SCL 17
  #define DISPLAY_SDA 16
  #define DISPLAY_ROTATION 2
// end pinouts/configs

#define Sprint(...) (Serial.print(__VA_ARGS__), Serial.flush())
#define Sprintln(...) (Serial.println(__VA_ARGS__), Serial.flush())
#define Sprintf(...) (Serial.printf(__VA_ARGS__), Serial.flush())

#include <atomic>

#include <Arduino.h>
#include <Wire.h>

#include <pio_usb.h>
#define HOST_PIN_DP   2   // Pin used as D+ for host, D- = D+ + 1

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>

#include "bindings.h"

const char *const MODIFIER_NAMES[] = {
  "CtrlL", "ShiftL", "AltL", "SuperL",
  "CtrlR", "ShiftR", "AltR", "SuperR",
};

// USB Host object
Adafruit_USBH_Host USBHost;
// holding device descriptor
tusb_desc_device_t desc_device;

static Adafruit_SSD1306 display(128, 32, &Wire, /* OLED_RESET */ -1);

#if defined(WAIT_PIN) || defined(WAIT_SERIAL)
std::atomic<bool> wait = true;
#endif

struct {
  bool bell;
  bool clickEnabled = true;
  bool caps;
  bool compose;
  bool scroll;
  bool num;
  uint8_t lastModifiers;
  uint8_t lastKeys[6];
  std::atomic<unsigned long> clickingSince;
  bool inMenu = false;
  unsigned selectedMenuItem = 0u;
  unsigned topMenuItem = 0u;
  unsigned clickDuration = 5u; // [0,100]
} state;
struct {
  bool ok;
} fake;

void setup() {
  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  analogWriteRange(100);

#ifdef WAIT_PIN
  pinMode(WAIT_PIN, INPUT_PULLUP);
  while (digitalRead(WAIT_PIN));
#endif
#ifdef WAIT_SERIAL
  while (Serial.read() == -1);
#endif
#if defined(WAIT_PIN) || defined(WAIT_SERIAL)
  wait = false;
#endif

  gpio_set_outover(SUN_KTX, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(SUN_KRX, GPIO_OVERRIDE_INVERT);
  Serial1.setPinout(SUN_KTX, SUN_KRX);
  Serial1.begin(1200, SERIAL_8N1);
#ifdef FAKE_SUN_ENABLE
  gpio_set_outover(FAKE_SUN_KRX, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(FAKE_SUN_KTX, GPIO_OVERRIDE_INVERT);
  Serial2.setPinout(FAKE_SUN_KRX, FAKE_SUN_KTX);
  Serial2.begin(1200, SERIAL_8N1);
#endif

  Wire.setSCL(DISPLAY_SCL);
  Wire.setSDA(DISPLAY_SDA);
  display.begin(SSD1306_SWITCHCAPVCC, /* SCREEN_ADDRESS */ 0x3C);
  display.setRotation(DISPLAY_ROTATION);
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.clearDisplay();

  Sprintln("usb3sun");

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

void buzzerClick() {
  if (!state.clickEnabled)
    return;

  // violation of sparc keyboard spec :) but distinguishable from bell!
  tone(BUZZER_PIN, 1'000u, state.clickDuration);
  state.clickingSince = micros();
}

void buzzerUpdate() {
  const auto t = micros();
  const unsigned long clickingSince = state.clickingSince;
  if (state.clickingSince >= state.clickDuration * 1'000uL && t - state.clickingSince < state.clickDuration * 1'000uL)
    return;
  if (state.bell)
    tone(BUZZER_PIN, 1'000'000u / 480u);
  else
    noTone(BUZZER_PIN);
}

void loop() {
  const auto t = micros();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0, 0);
  // static int i = 0;
  // display.printf("#%d @%lu", i++, t / 1'000);
  display.printf("usb3sun%c", t / 500'000 % 2 == 1 ? '.' : ' ');
  if (state.inMenu) {
    display.setCursor(0, 8);
    unsigned int i = 0;
    if (i >= state.topMenuItem && i <= state.topMenuItem + 2) drawMenuItem(0, 8 * (1 + i - state.topMenuItem), state.selectedMenuItem == i, "Go back"); i++;
    if (i >= state.topMenuItem && i <= state.topMenuItem + 2) drawMenuItem(0, 8 * (1 + i - state.topMenuItem), state.selectedMenuItem == i, state.clickEnabled ? "Disable click" : "Enable click"); i++;
    if (i >= state.topMenuItem && i <= state.topMenuItem + 2) drawMenuItem(0, 8 * (1 + i - state.topMenuItem), state.selectedMenuItem == i, "Click duration: %u ms", state.clickDuration); i++;
  } else {
    drawStatus(78, 0, "CLK", state.clickEnabled);
    drawStatus(104, 0, "BEL", state.bell);
    drawStatus(0, 18, "CAP", state.caps);
    drawStatus(26, 18, "CMP", state.compose);
    drawStatus(52, 18, "SCR", state.scroll);
    drawStatus(78, 18, "NUM", state.num);
    if (state.bell || t - state.clickingSince < 100'000uL) {
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
  } else {
    ;
  }
#endif
}

void serialEvent1() {
  while (Serial1.available() > 0) {
    uint8_t command = Serial1.read();
    Sprintf("command %u\n", command);
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
        buzzerUpdate();
        break;
      case SUNK_BELL_OFF:
        state.bell = false;
        buzzerUpdate();
        break;
      case SUNK_CLICK_ON:
        state.clickEnabled = true;
        break;
      case SUNK_CLICK_OFF:
        state.clickEnabled = false;
        break;
      case SUNK_LED: {
        uint8_t status = Serial.read();
        state.num = status << 0 & 1;
        state.compose = status << 1 & 1;
        state.scroll = status << 2 & 1;
        state.caps = status << 3 & 1;
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

#ifdef FAKE_SUN_ENABLE
void serialEvent2() {
  while (Serial2.available() > 0) {
    uint8_t command = Serial2.read();
    Sprintf("fake: command %02Xh\n", command);
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
#if defined(WAIT_PIN) || defined(WAIT_SERIAL)
  while (wait);
#endif

  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if (cpu_hz != 120000000uL && cpu_hz != 240000000uL) {
    Sprintf("error: cpu frequency %u, set [env:pico] board_build.f_cpu = 120000000L\n", cpu_hz);
    while (true) delay(1);
  }

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;

  // claim state machines so that tone() doesn’t clobber them
  // pio0 and pio1 based on PIO_USB_DEFAULT_CONFIG and logic in pio_usb_bus_init
  // https://github.com/sekigon-gonnoc/Pico-PIO-USB/blob/52805e6d92556e67d3738bd8fb10227a45b13a08/src/pio_usb.c#L277
  pio_cfg.sm_tx = pio_claim_unused_sm(pio0, true);
  pio_cfg.sm_rx = pio_claim_unused_sm(pio1, true);

  pio_cfg.pin_dp = HOST_PIN_DP;
  USBHost.configure_pio_usb(1, &pio_cfg);

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
}

void loop1() {
  USBHost.task();
  buzzerUpdate();
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
  Sprintf("mount dev_addr=%u instance=%u vid:pid=%04x:%04x\n", dev_addr, instance, vid, pid);

  tuh_hid_report_info_t infos[16];
  size_t infos_len = tuh_hid_parse_report_descriptor(infos, sizeof(infos) / sizeof(*infos), desc_report, desc_len);
  for (size_t i = 0; i < infos_len; i++)
    Sprintf("      report[%zu] report_id=%u usage=%u usage_page=%u\n", infos[i].report_id, infos[i].usage, infos[i].usage_page);

  if (!tuh_hid_receive_report(dev_addr, instance))
    Sprintf("error: failed to request to receive report\n");
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  Sprintf("unmount dev_addr=%u instance=%u\n", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  Sprintf("report");
  for (uint16_t i = 0; i < len; i++)
    Sprintf(" %02Xh", report[i]);

  switch (tuh_hid_interface_protocol(dev_addr, instance)) {
    case HID_ITF_PROTOCOL_KEYBOARD: {
      hid_keyboard_report_t *kreport = (hid_keyboard_report_t *) report;
      buzzerClick();

      for (int i = 0; i < 6; i++) {
        if (kreport->keycode[i] != USBK_RESERVED && kreport->keycode[i] < USBK_FIRST_KEYCODE) {
          Sprintf(" !%u", kreport->keycode[i]);
          goto out;
        }
      }

      for (int i = 0; i < 8; i++)
        if ((state.lastModifiers & 1 << i) != (kreport->modifier & 1 << i))
          Sprintf(" %c%s", kreport->modifier & 1 << i ? '+' : '-', MODIFIER_NAMES[i]);

      if (kreport->modifier == 0b00001111) {
        state.inMenu = !state.inMenu;
        state.selectedMenuItem = 0u;
        state.topMenuItem = 0u;
      }

      struct {
        uint8_t keycode;
        bool make;
      } changes[12];
      size_t changesLen = 0;

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
          Sprintf(" -%u", state.lastKeys[i]);
          changes[changesLen++] = {state.lastKeys[i], false};
        }
        if (!newInOlds && kreport->keycode[i] >= USBK_FIRST_KEYCODE) {
          Sprintf(" +%u", kreport->keycode[i]);
          changes[changesLen++] = {kreport->keycode[i], true};
        }
      }

      for (int i = 0; i < changesLen; i++) {
        if (state.inMenu) {
          if (changes[i].make) {
            switch (changes[i].keycode) {
              case USBK_RIGHT:
                switch (state.selectedMenuItem) {
                  case 2:
                    if (state.clickDuration < 96u) {
                      state.clickDuration += 5u;
                      buzzerClick();
                    }
                    break;
                }
                break;
              case USBK_LEFT:
                switch (state.selectedMenuItem) {
                  case 2:
                    if (state.clickDuration > 4u) {
                      state.clickDuration -= 5u;
                      buzzerClick();
                    }
                    break;
                }
                break;
              case USBK_DOWN:
                if (state.selectedMenuItem < 3u - 1u)
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
                  case 1:
                    state.clickEnabled = !state.clickEnabled;
                    break;
                }
                state.inMenu = false;
                break;
            }
          }
          continue;
        }

        for (int j = 0; j < sizeof(SIMPLE_BINDINGS) / sizeof(*SIMPLE_BINDINGS); j++)
          if (SIMPLE_BINDINGS[j].usbk == changes[i].keycode)
            Serial1.write(changes[i].make ? SIMPLE_BINDINGS[j].sunkMake : SIMPLE_BINDINGS[j].sunkBreak);
      }

      state.lastModifiers = kreport->modifier;
      for (int i = 0; i < 6; i++)
        state.lastKeys[i] = kreport->keycode[i];
    } break;
    case HID_ITF_PROTOCOL_MOUSE: {
      hid_mouse_report_t *mreport = (hid_mouse_report_t *) report;
      Sprintf(" buttons=%u x=%d y=%d", mreport->buttons, mreport->x, mreport->y);
    } break;
  }
out:

  Sprintln();
  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance))
    Sprintf("error: failed to request to receive report\n");
}
