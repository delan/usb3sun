#include "config.h"

#include <atomic>

#ifdef USB3SUN_HAL_ARDUINO_PICO
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#endif

#include "bindings.h"
#include "buzzer.h"
#include "cli.h"
#include "hal.h"
#include "menu.h"
#include "pinout.h"
#include "settings.h"
#include "state.h"
#include "sunm.h"
#include "sunk.h"
#include "usb.h"
#include "view.h"

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
struct {
  bool present = false;
  uint8_t dev_addr;
  uint8_t instance;
  uint8_t if_protocol;
  struct {
    bool present = false;
    uint8_t report_id;
    uint8_t report;
  } led;
} hid[16];

std::atomic<bool> waiting = true;

Pinout pinout;
State state;
Buzzer buzzer;
Settings settings;
USB3SUN_MUTEX usb3sun_mutex buzzerMutex;
USB3SUN_MUTEX usb3sun_mutex settingsMutex;

void drawStatus(int16_t x, int16_t y, const char *label, bool on);

struct DefaultView : View {
  const char *name() const override {
    return "DefaultView";
  }

  void handlePaint() override {
    usb3sun_display_text(0, 0, false, USB3SUN_VERSION);
    drawStatus(78, 0, "CLK", state.clickEnabled);
    drawStatus(104, 0, "BEL", state.bell);
    drawStatus(0, 18, "CAP", state.caps);
    drawStatus(26, 18, "CMP", state.compose);
    drawStatus(52, 18, "SCR", state.scroll);
    drawStatus(78, 18, "NUM", state.num);
    if (buzzer.current != Buzzer::_::NONE) {
      const auto x = 106;
      const auto y = 16;
      usb3sun_display_rect(x + 6, y + 1, 2, 11, 0, false, true);
      usb3sun_display_rect(x + 5, y + 2, 4, 10, 0, false, true);
      usb3sun_display_rect(x + 4, y + 4, 6, 8, 0, false, true);
      usb3sun_display_rect(x + 2, y + 9, 10, 3, 0, false, true);
      usb3sun_display_rect(x + 1, y + 10, 12, 2, 0, false, true);
      usb3sun_display_rect(x + 5, y + 13, 4, 1, 0, false, true);
    } else {
      usb3sun_display_rect(106, 16, 14, 14, 0, true, true);
    }
  }

  void handleKey(const UsbkChanges &changes) override {
#ifdef DEBUG_TIMINGS
    unsigned long t = usb3sun_micros();
#endif

#ifdef SUNK_ENABLE
    for (size_t i = 0; i < changes.dvLen; i++) {
      // for DV bindings, make when key makes and break when key breaks
      if (uint8_t sunkMake = USBK_TO_SUNK.dv[changes.dv[i].usbkModifier])
        sunkSend(changes.dv[i].make, sunkMake);
    }
#endif

    // treat simultaneous DV and Sel changes as DV before Sel, for special bindings
    const uint8_t lastModifiers = changes.kreport.modifier;

    for (size_t i = 0; i < changes.selLen; i++) {
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
    Sprintf("sent in %ju\n", usb3sun_micros() - t);
#endif
  }
};

static DefaultView DEFAULT_VIEW{};

void setup() {
  // pico led on, then configure pin modes
  pinout.begin();
  Sprintln("usb3sun " USB3SUN_VERSION);
  Sprintf("pinout: v%zu\n", usb3sun_pinout_version());

  usb3sun_display_init();
  Settings::begin();
  settings.readAll();
  pinout.beginSun();

  View::push(&DEFAULT_VIEW);

#ifdef WAIT_PIN
  while (usb3sun_gpio_read(WAIT_PIN));
#endif
#ifdef WAIT_SERIAL
  while (usb3sun_debug_cdc_read() == -1);
#endif
  waiting = false;

  usb3sun_gpio_write(LED_PIN, false);
}

void drawStatus(int16_t x, int16_t y, const char *label, bool on) {
  usb3sun_display_rect(x, y, 24, 14, 4, false, on);
  usb3sun_display_text(x + 3, y + 4, on, label);
}

void loop() {
  usb3sun_display_clear();
  View::paint();
  usb3sun_display_flush();

#ifdef UHID_LED_TEST
  static int z = 0;
  Message message = ++z % 2 == 0
    ? Message::UHID_LED_ALL_OFF
    : Message::UHID_LED_ALL_ON;
  usb3sun_fifo_push((uint32_t)message);
#endif

  int input;
  while ((input = usb3sun_debug_uart_read()) != -1) {
    handleCliInput(input);
  }
#ifdef SUNK_SNIFFER_ENABLE
  while ((input = usb3sun_sunk_sniff_tx()) != -1) {
    uint8_t command = input;
    Sprintf("sunk: sniff tx %02Xh (%d)\n", command, command);
  }
#endif


  usb3sun_sleep_micros(10'000);
}

#if defined(SUNK_ENABLE) || defined(SUNK_SNIFFER_ENABLE)
void sunkEvent() {
  int result;
  while ((result = usb3sun_sunk_read()) != -1) {
    uint8_t command = result;
    Sprintf("sunk: rx %02Xh\n", command);
    switch (command) {
      case SUNK_RESET: {
#ifndef SUNK_SNIFFER_ENABLE
        // self test fail:
        // usb3sun_sunk_write(0x7E);
        // usb3sun_sunk_write(0x01);
        uint8_t response[]{SUNK_RESET_RESPONSE, 0x04, 0x7F}; // TODO optional make code
        usb3sun_sunk_write(response, sizeof response);
#endif
      } break;
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
        while ((result = usb3sun_sunk_read()) == -1) usb3sun_sleep_micros(1'000);
        uint8_t status = result;
        Sprintf("sunk: led status %02Xh\n", status);
        state.num = status & 1 << 0;
        state.compose = status & 1 << 1;
        state.scroll = status & 1 << 2;
        state.caps = status & 1 << 3;

        // ensure state update finished, then notify
        usb3sun_dmb();
        usb3sun_fifo_push((uint32_t)Message::UHID_LED_FROM_STATE);
      } break;
      case SUNK_LAYOUT: {
#ifndef SUNK_SNIFFER_ENABLE
        // UNITED STATES (TODO alternate layouts)
        uint8_t response[]{SUNK_LAYOUT_RESPONSE, 0b00000000};
        usb3sun_sunk_write(response, sizeof response);
#endif
      } break;
    }
  }
}
#endif

void serialEvent1() {
#if defined(SUNK_ENABLE) || defined(SUNK_SNIFFER_ENABLE)
  sunkEvent();
#endif
}

void serialEvent2() {
#if defined(SUNK_ENABLE) || defined(SUNK_SNIFFER_ENABLE)
  sunkEvent();
#endif
}

void setup1() {
  while (waiting);

  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = usb3sun_clock_speed();
  if (cpu_hz != 120000000uL && cpu_hz != 240000000uL) {
    usb3sun_panic("error: cpu frequency %u, set [env:pico] board_build.f_cpu = 120000000L\n", cpu_hz);
  }

  usb3sun_usb_init();
}

void loop1() {
  uint32_t message;
  if (usb3sun_fifo_pop(&message)) {
    for (size_t i = 0; i < sizeof(hid) / sizeof(*hid); i++) {
      if (!hid[i].present || !hid[i].led.present)
        continue;
      uint8_t dev_addr = hid[i].dev_addr;
      uint8_t instance = hid[i].instance;
      uint8_t report_id = hid[i].led.report_id;
      switch (message) {
        case (uint32_t)Message::UHID_LED_FROM_STATE:
          hid[i].led.report =
            state.num << 0
            | state.caps << 1
            | state.scroll << 2
            | state.compose << 3;
          break;
        case (uint32_t)Message::UHID_LED_ALL_OFF:
          hid[i].led.report = 0x00;
          break;
        case (uint32_t)Message::UHID_LED_ALL_ON:
          hid[i].led.report = 0xFF;
          break;
      }
#if defined(UHID_LED_ENABLE)
#ifndef UHID_VERBOSE
      Sprint("*");
#endif
#ifdef UHID_VERBOSE
      Sprintf("hid [%zu]: usb [%u:%u]: set led report %02Xh\n", i, dev_addr, instance, hid[i].led.report);
#endif
      usb3sun_uhid_set_led_report(dev_addr, instance, report_id, hid[i].led.report);
#else
      (void) dev_addr;
      (void) instance;
      (void) report_id;
#endif
    }
  }
  usb3sun_usb_task();
  buzzer.update();
}

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  uint16_t vid, pid;
  usb3sun_usb_vid_pid(dev_addr, &vid, &pid);
  Sprintf("usb [%u:%u]: hid mount vid:pid=%04x:%04x\n", dev_addr, instance, vid, pid);

  usb3sun_hid_report_info reports[16];
  size_t reports_len = usb3sun_uhid_parse_report_descriptor(reports, sizeof(reports) / sizeof(*reports), desc_report, desc_len);
  for (size_t i = 0; i < reports_len; i++)
    Sprintf("    reports[%zu] report_id=%u usage=%02Xh usage_page=%04Xh\n", i, reports[i].report_id, reports[i].usage, reports[i].usage_page);

  // hid_subclass_enum_t if_subclass = ...;
  uint8_t if_protocol = usb3sun_uhid_interface_protocol(dev_addr, instance);
  Sprintf("    bInterfaceProtocol=%u", if_protocol);
  switch (if_protocol) {
    case USB3SUN_UHID_KEYBOARD:
      Sprintln(" (boot keyboard)");
      break;
    case USB3SUN_UHID_MOUSE:
      Sprintln(" (boot mouse)");
      break;
    default:
      Sprintln();
  }

  // TODO non-boot input devices
  switch (if_protocol) {
    case USB3SUN_UHID_KEYBOARD:
    case USB3SUN_UHID_MOUSE: {
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
          hid[i].led.present = false;
          if (if_protocol == USB3SUN_UHID_KEYBOARD) {
            for (size_t j = 0; j < reports_len; j++) {
              if (reports[j].usage_page == 1 && reports[j].usage == 6) {
                hid[i].led.present = true;
                hid[i].led.report_id = reports[j].report_id;
                Sprintf("hid [%zu]: led report_id=%u\n", i, hid[i].led.report_id);
              }
            }
          }
          hid[i].present = true;
          ok = true;
          break;
        }
      }
      if (!ok)
        Sprintln("error: usb [%u:%u]: hid table full");
    }
  }

  if (!usb3sun_uhid_request_report(dev_addr, instance))
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
  uint8_t if_protocol = usb3sun_uhid_interface_protocol(dev_addr, instance);
#ifdef UHID_VERBOSE
  Sprintf("usb [%u:%u]: hid report if_protocol=%u", dev_addr, instance, if_protocol);
  for (uint16_t i = 0; i < len; i++)
    Sprintf(" %02Xh", report[i]);
#else
  Sprint(".");
  (void) len;
#endif

  switch (if_protocol) {
    case USB3SUN_UHID_KEYBOARD: {
      const UsbkReport *kreport = reinterpret_cast<const UsbkReport *>(report);

#ifdef DEBUG_TIMINGS
      unsigned long t = usb3sun_micros();
#endif

      for (int i = 0; i < 6; i++) {
        if (kreport->keycode[i] != USBK_RESERVED && kreport->keycode[i] < USBK_FIRST_KEYCODE) {
#ifdef UHID_VERBOSE
          Sprintf(" !%u\n", kreport->keycode[i]);
#endif
          goto out;
        }
      }

      UsbkChanges changes{};
      changes.kreport = *kreport;

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
      Sprintf("diffed in %ju\n", usb3sun_micros() - t);
#endif

      View::sendKeys(changes);

      // commit the DV and Sel changes
      state.lastModifiers = changes.kreport.modifier;
      for (int i = 0; i < 6; i++)
        state.lastKeys[i] = changes.kreport.keycode[i];
    } break;
    case USB3SUN_UHID_MOUSE: {
      const UsbmReport *mreport = reinterpret_cast<const UsbmReport *>(report);
#ifdef UHID_VERBOSE
      Sprintf(" buttons=%u x=%d y=%d", mreport->buttons, mreport->x, mreport->y);
#endif

#ifdef UHID_VERBOSE
      for (int i = 0; i < 3; i++)
        if ((state.lastButtons & 1 << i) != (mreport->buttons & 1 << i))
          Sprintf(" %c%s", mreport->buttons & 1 << i ? '+' : '-', BUTTON_NAMES[i]);
      Sprintln();
#endif

      sunmSend(
        mreport->x, mreport->y,
        !!(mreport->buttons & USBM_LEFT),
        !!(mreport->buttons & USBM_MIDDLE),
        !!(mreport->buttons & USBM_RIGHT));

      state.lastButtons = mreport->buttons;
    } break;
    default: {
#ifdef UHID_VERBOSE
      Sprintln();
#endif
    } break;
  }
out:
  // continue to request to receive report
  if (!usb3sun_uhid_request_report(dev_addr, instance))
    Sprintf("error: usb [%u:%u]: failed to request to receive report\n", dev_addr, instance);
}

#ifdef USB3SUN_HAL_LINUX_NATIVE

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TEST_REQUIRES(expr) do { fprintf(stderr, ">>> skipping test (requires %s)\n", #expr); return true; } while (0)
#define TEST_ASSERT_EQ(actual, expected) do { if (actual != expected) { std::cerr << "\n" __FILE__ ":" << __LINE__ << ": assertion failed: " #actual "\n    actual: " << actual << "\n    expected: " << expected << "\n"; return false; } } while (0)
static bool assert_then_clear_test_history(const char *file, size_t line, const std::vector<Op> &expected) {
  const std::vector<Entry> &actual = usb3sun_test_get_history();
  std::optional<size_t> first_difference{};
  for (size_t i = 0; i < actual.size() || i < expected.size(); i++) {
    if ((i < actual.size()) != (i < expected.size()) || actual[i].op != expected[i]) {
      first_difference = i;
      break;
    }
  }
  if (first_difference.has_value()) {
    std::cerr << "\n" << file << ":" << line << ": assertion failed: bad test history!\n";
    for (size_t i = 0; i < *first_difference; i++) {
      std::cerr << "    " << actual[i] << "\n";
    }
    for (size_t i = *first_difference; i < actual.size() || i < expected.size(); i++) {
      std::cerr << "!!!";
      if (i < actual.size()) {
        std::cerr << " " << actual[i];
      }
      if (i < expected.size()) {
        std::cerr << " (expected " << expected[i] << ")";
      }
      std::cerr << "\n";
    }
  }
  usb3sun_test_clear_history();
  return !first_difference.has_value();
}
#define assert_then_clear_test_history(...) assert_then_clear_test_history(__FILE__, __LINE__, __VA_ARGS__)

static std::vector<const char *> test_names = {
  "setup_pinout_v1",
  "setup_pinout_v2",
  "sunk_reset",
  "uhid_mount",
  "buzzer_bell",
  "buzzer_click",
  "settings_read_ok",
  "settings_read_not_found",
  "settings_read_v1_ok",
  "settings_read_v1_wrong_version",
  "settings_read_v1_too_short",
  "view_stack",
  "menu_settings",
  "menu_hostid",
};

static void help() {
  std::cerr << "usage: path/to/program <demo|all|test_name>\n";
  std::cerr << "...where test_name can be one of:\n";
  for (const char *&name : test_names) {
    std::cerr << "    " << name << "\n";
  }
}

static std::vector<uint8_t> bytes(size_t len, const uint8_t *data) {
  return {data, data + len};
}

static std::vector<uint8_t> bytes(size_t len, const char *data) {
  return bytes(len, reinterpret_cast<const uint8_t *>(data));
}

static bool run_test(const char *test_name) {
  if (!strcmp(test_name, "setup_pinout_v1")) {
    usb3sun_test_init(PinoutV2Op::id | SunkInitOp::id | SunmInitOp::id | GpioWriteOp::id | GpioReadOp::id | SunkSnifferInitOp::id);
    usb3sun_mock_gpio_read(PINOUT_V2_PIN, false);
    setup();
    return assert_then_clear_test_history(std::vector<Op> {
      GpioWriteOp {LED_PIN, true},
      GpioReadOp {PINOUT_V2_PIN, false},
#ifdef SUNK_ENABLE
      SunkInitOp {},
#endif
#ifdef SUNK_SNIFFER_ENABLE
      SunkSnifferInitOp {},
#endif
#ifdef SUNM_ENABLE
      SunmInitOp {9600},
#endif
      GpioWriteOp {LED_PIN, false},
    });
  }

  if (!strcmp(test_name, "setup_pinout_v2")) {
    usb3sun_test_init(PinoutV2Op::id | SunkInitOp::id | SunmInitOp::id | GpioWriteOp::id | GpioReadOp::id | SunkSnifferInitOp::id);
    usb3sun_mock_gpio_read(PINOUT_V2_PIN, true);
    setup();
    return assert_then_clear_test_history(std::vector<Op> {
      GpioWriteOp {LED_PIN, true},
      GpioReadOp {PINOUT_V2_PIN, true},
      PinoutV2Op {},
      GpioWriteOp {DISPLAY_ENABLE, true},
#ifdef SUNK_ENABLE
      SunkInitOp {},
#endif
#ifdef SUNK_SNIFFER_ENABLE
      SunkSnifferInitOp {},
#endif
#if defined(SUNK_ENABLE) || defined(SUNK_SNIFFER_ENABLE)
      GpioWriteOp {KTX_ENABLE, false},
#endif
#ifdef SUNM_ENABLE
      SunmInitOp {9600},
#endif
      GpioWriteOp {LED_PIN, false},
    });
  }

  if (!strcmp(test_name, "sunk_reset")) {
#if !defined(SUNK_ENABLE) && !defined(SUNK_SNIFFER_ENABLE)
    TEST_REQUIRES(SUNK_ENABLE or SUNK_SNIFFER_ENABLE);
#endif
    usb3sun_test_init(SunkReadOp::id | SunkWriteOp::id);
    usb3sun_mock_sunk_read("\x01", 1); // SUNK_RESET
    setup();
    while (usb3sun_mock_sunk_read_has_input()) {
      serialEvent1();
      serialEvent2();
    }
    return assert_then_clear_test_history(std::vector<Op> {
#if defined(SUNK_ENABLE) || defined(SUNK_SNIFFER_ENABLE)
      SunkReadOp {},
#ifndef SUNK_SNIFFER_ENABLE
      SunkWriteOp {{0xFF, 0x04, 0x7F}},
#endif
      SunkReadOp {},
      SunkReadOp {},
#endif
    });
  }

  if (!strcmp(test_name, "uhid_mount")) {
    usb3sun_test_init(UhidRequestReportOp::id);
    setup();

    uint8_t empty[]{};
    usb3sun_mock_usb_vid_pid(true, 0x045E, 0x0750); // Microsoft Wired Keyboard 600
    usb3sun_mock_uhid_parse_report_descriptor(std::vector<usb3sun_hid_report_info> {
        usb3sun_hid_report_info {0, 0x06, 0x0001},
    });
    usb3sun_mock_uhid_interface_protocol(USB3SUN_UHID_KEYBOARD);
    usb3sun_mock_uhid_request_report_result(true);
    tuh_hid_mount_cb(1, 0, empty, 0);

    usb3sun_mock_uhid_parse_report_descriptor(std::vector<usb3sun_hid_report_info> {
        usb3sun_hid_report_info {1, 0x01, 0x000C},
        usb3sun_hid_report_info {3, 0x80, 0x0001},
    });
    usb3sun_mock_uhid_interface_protocol(0);
    tuh_hid_mount_cb(1, 1, empty, 0);

    // TODO assert something else that’s true for [1:0] but not true for [1:1]
    return assert_then_clear_test_history(std::vector<Op> {
      UhidRequestReportOp {1, 0},
      UhidRequestReportOp {1, 1},
    });
  }

  if (!strcmp(test_name, "buzzer_bell")) {
#if !defined(SUNK_ENABLE) && !defined(SUNK_SNIFFER_ENABLE)
    TEST_REQUIRES(SUNK_ENABLE or SUNK_SNIFFER_ENABLE);
#endif
    usb3sun_test_init(BuzzerStartOp::id | GpioWriteOp::id);
    usb3sun_mock_sunk_read("\x01\x02\x03", 3); // SUNK_RESET, SUNK_BELL_ON, SUNK_BELL_OFF
    setup();
    while (usb3sun_mock_sunk_read_has_input()) {
      serialEvent1();
      serialEvent2();
    }
    return assert_then_clear_test_history(std::vector<Op> {
      GpioWriteOp {LED_PIN, true},
      GpioWriteOp {DISPLAY_ENABLE, true},
      GpioWriteOp {KTX_ENABLE, false},
      GpioWriteOp {LED_PIN, false},
      BuzzerStartOp {2083},
      GpioWriteOp {BUZZER_PIN, false},
    });
  }

  if (!strcmp(test_name, "buzzer_click")) {
#if !defined(SUNK_ENABLE) && !defined(SUNK_SNIFFER_ENABLE)
    TEST_REQUIRES(SUNK_ENABLE or SUNK_SNIFFER_ENABLE);
#endif
    const auto pumpSunkInput = []() {
      while (usb3sun_mock_sunk_read_has_input()) {
        serialEvent1();
        serialEvent2();
      }
    };
    const auto pressKey = []() {
      sunkSend(true, SUNK_RETURN);
      sunkSend(false, SUNK_RETURN);
    };
    const auto pumpBuzzerUpdates = []() {
      uint64_t t_start = usb3sun_micros();
      while (usb3sun_micros() - t_start < 5'000) {
        loop1();
      }
      loop1();
    };
    usb3sun_test_init(BuzzerStartOp::id | GpioWriteOp::id);
    usb3sun_mock_sunk_read("\x01\x02", 2); // SUNK_RESET, SUNK_BELL_ON

    // get the setup out of the way.
    setup();
    if (!assert_then_clear_test_history(std::vector<Op> {
      GpioWriteOp {LED_PIN, true},
      GpioWriteOp {DISPLAY_ENABLE, true},
      GpioWriteOp {KTX_ENABLE, false},
      GpioWriteOp {LED_PIN, false},
    })) return false;

    // no click by default.
    pumpSunkInput();
    pressKey();
    pumpBuzzerUpdates();
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {2083},
    })) return false;

    // click when workstation enables click mode.
    usb3sun_mock_sunk_read("\x0A", 1); // SUNK_CLICK_ON
    pumpSunkInput();
    pressKey();
    pumpBuzzerUpdates();
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {1000},
      BuzzerStartOp {2083},
    })) return false;

    // no click when workstation disables click mode.
    usb3sun_mock_sunk_read("\x0B", 1); // SUNK_CLICK_OFF
    pumpSunkInput();
    pressKey();
    pumpBuzzerUpdates();
    if (!assert_then_clear_test_history(std::vector<Op> {
    })) return false;

    // click when forceClick is on, even when click mode is disabled.
    settings.forceClick.current = ForceClick::_::ON;
    pumpSunkInput();
    pressKey();
    pumpBuzzerUpdates();
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {1000},
      BuzzerStartOp {2083},
    })) return false;

    // no click when forceClick is off, even when click mode is enabled.
    settings.forceClick.current = ForceClick::_::OFF;
    usb3sun_mock_sunk_read("\x0A", 1); // SUNK_CLICK_ON
    pumpSunkInput();
    pressKey();
    pumpBuzzerUpdates();
    if (!assert_then_clear_test_history(std::vector<Op> {
    })) return false;

    return true;
  }

  if (!strcmp(test_name, "settings_read_ok")) {
    usb3sun_test_init(FsReadOp::id | FsWriteOp::id);
    usb3sun_mock_fs_read([](const char *path, char *data, size_t data_len, size_t &actual_len) {
      if (!strcmp(path, "/clickDuration.v2")) {
        memcpy(data, "\x55\x55\x55\x55\x55\x55\x55\x55", actual_len = std::min(data_len, (size_t)8));
        return true;
      }
      if (!strcmp(path, "/forceClick.v2")) {
        memcpy(data, "\x02\x00\x00\x00", actual_len = std::min(data_len, (size_t)4));
        return true;
      }
      if (!strcmp(path, "/mouseBaud.v2")) {
        memcpy(data, "\x02\x00\x00\x00", actual_len = std::min(data_len, (size_t)4));
        return true;
      }
      if (!strcmp(path, "/hostid.v2")) {
        memcpy(data, "\x31\x32\x33\x34\x35\x36", actual_len = std::min(data_len, (size_t)6));
        return true;
      }
      return false;
    });
    setup();
    TEST_ASSERT_EQ(settings.clickDuration, 0x5555555555555555);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::ON);
    TEST_ASSERT_EQ(settings.mouseBaud.current, MouseBaud::_::S4800);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'1', '2', '3', '4', '5', '6'}}));
    return assert_then_clear_test_history(std::vector<Op> {
      FsReadOp {"/clickDuration.v2", 8, bytes(8, "\x55\x55\x55\x55\x55\x55\x55\x55")},
      FsReadOp {"/forceClick.v2", 4, bytes(4, "\x02\x00\x00\x00")},
      FsReadOp {"/mouseBaud.v2", 4, bytes(4, "\x02\x00\x00\x00")},
      FsReadOp {"/hostid.v2", 6, bytes(6, "\x31\x32\x33\x34\x35\x36")},
    });
  }

  if (!strcmp(test_name, "settings_read_not_found")) {
    usb3sun_test_init(FsReadOp::id | FsWriteOp::id);
    usb3sun_mock_fs_read([](const char *path, char *data, size_t data_len, size_t &actual_len) {
      (void) path;
      (void) data;
      (void) data_len;
      (void) actual_len;
      return false;
    });
    setup();
    TEST_ASSERT_EQ(settings.clickDuration, 5);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::NO);
    TEST_ASSERT_EQ(settings.mouseBaud.current, MouseBaud::_::S9600);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'0', '0', '0', '0', '0', '0'}}));
    return assert_then_clear_test_history(std::vector<Op> {
      FsReadOp {"/clickDuration.v2", 8, {}},
      FsReadOp {"/clickDuration", 16, {}},
      FsReadOp {"/forceClick.v2", 4, {}},
      FsReadOp {"/forceClick", 8, {}},
      FsReadOp {"/mouseBaud.v2", 4, {}},
      FsReadOp {"/mouseBaud", 8, {}},
      FsReadOp {"/hostid.v2", 6, {}},
      FsReadOp {"/hostid", 12, {}},
    });
  }

  if (!strcmp(test_name, "settings_read_v1_ok")) {
    usb3sun_test_init(FsReadOp::id | FsWriteOp::id);
    usb3sun_mock_fs_read([](const char *path, char *data, size_t data_len, size_t &actual_len) {
      if (!strcmp(path, "/clickDuration")) {
        //            [      version ][      padding ][                unsigned long ]
        memcpy(data, "\x01\x00\x00\x00\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x55\x55\x55\x55", actual_len = std::min(data_len, (size_t)16));
        return true;
      }
      if (!strcmp(path, "/forceClick")) {
        //            [      version ][ enum:int32_t ]
        memcpy(data, "\x01\x00\x00\x00\x02\x00\x00\x00", actual_len = std::min(data_len, (size_t)8));
        return true;
      }
      if (!strcmp(path, "/mouseBaud")) {
        //            [      version ][ enum:int32_t ]
        memcpy(data, "\x01\x00\x00\x00\x02\x00\x00\x00", actual_len = std::min(data_len, (size_t)8));
        return true;
      }
      if (!strcmp(path, "/hostid")) {
        //            [      version ][           uint8_t[6] ][  pad ]
        memcpy(data, "\x01\x00\x00\x00\x31\x32\x33\x34\x35\x36\xAA\xAA", actual_len = std::min(data_len, (size_t)12));
        return true;
      }
      return false;
    });
    setup();
    TEST_ASSERT_EQ(settings.clickDuration, 0x5555555555555555);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::ON);
    TEST_ASSERT_EQ(settings.mouseBaud.current, MouseBaud::_::S4800);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'1', '2', '3', '4', '5', '6'}}));
    return assert_then_clear_test_history(std::vector<Op> {
      FsReadOp {"/clickDuration.v2", 8, {}},
      FsReadOp {"/clickDuration", 16, bytes(16, "\x01\x00\x00\x00\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x55\x55\x55\x55")},
      FsWriteOp {"/clickDuration.v2", bytes(8, "\x55\x55\x55\x55\x55\x55\x55\x55")},
      FsReadOp {"/forceClick.v2", 4, {}},
      FsReadOp {"/forceClick", 8, bytes(8, "\x01\x00\x00\x00\x02\x00\x00\x00")},
      FsWriteOp {"/forceClick.v2", bytes(4, "\x02\x00\x00\x00")},
      FsReadOp {"/mouseBaud.v2", 4, {}},
      FsReadOp {"/mouseBaud", 8, bytes(8, "\x01\x00\x00\x00\x02\x00\x00\x00")},
      FsWriteOp {"/mouseBaud.v2", bytes(4, "\x02\x00\x00\x00")},
      FsReadOp {"/hostid.v2", 6, {}},
      FsReadOp {"/hostid", 12, bytes(12, "\x01\x00\x00\x00\x31\x32\x33\x34\x35\x36\xAA\xAA")},
      FsWriteOp {"/hostid.v2", bytes(6, "\x31\x32\x33\x34\x35\x36")},
    });
  }

  if (!strcmp(test_name, "settings_read_v1_wrong_version")) {
    usb3sun_test_init(FsReadOp::id | FsWriteOp::id);
    usb3sun_mock_fs_read([](const char *path, char *data, size_t data_len, size_t &actual_len) {
      if (!strcmp(path, "/clickDuration")) {
        //            [      version ][      padding ][                     uint64_t ]
        memcpy(data, "\x00\x00\x00\x00\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x55\x55\x55\x55", actual_len = std::min(data_len, (size_t)16));
        return true;
      }
      if (!strcmp(path, "/forceClick")) {
        //            [      version ][ enum:int32_t ]
        memcpy(data, "\x00\x00\x00\x00\x02\x00\x00\x00", actual_len = std::min(data_len, (size_t)8));
        return true;
      }
      if (!strcmp(path, "/mouseBaud")) {
        //            [      version ][ enum:int32_t ]
        memcpy(data, "\x00\x00\x00\x00\x02\x00\x00\x00", actual_len = std::min(data_len, (size_t)8));
        return true;
      }
      if (!strcmp(path, "/hostid")) {
        //            [      version ][           uint8_t[6] ][  pad ]
        memcpy(data, "\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\xAA\xAA", actual_len = std::min(data_len, (size_t)12));
        return true;
      }
      return false;
    });
    setup();
    TEST_ASSERT_EQ(settings.clickDuration, 5);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::NO);
    TEST_ASSERT_EQ(settings.mouseBaud.current, MouseBaud::_::S9600);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'0', '0', '0', '0', '0', '0'}}));
    return assert_then_clear_test_history(std::vector<Op> {
      FsReadOp {"/clickDuration.v2", 8, {}},
      FsReadOp {"/clickDuration", 16, bytes(16, "\x00\x00\x00\x00\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x55\x55\x55\x55")},
      FsReadOp {"/forceClick.v2", 4, {}},
      FsReadOp {"/forceClick", 8, bytes(8, "\x00\x00\x00\x00\x02\x00\x00\x00")},
      FsReadOp {"/mouseBaud.v2", 4, {}},
      FsReadOp {"/mouseBaud", 8, bytes(8, "\x00\x00\x00\x00\x02\x00\x00\x00")},
      FsReadOp {"/hostid.v2", 6, {}},
      FsReadOp {"/hostid", 12, bytes(12, "\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\xAA\xAA")},
    });
  }

  if (!strcmp(test_name, "settings_read_v1_too_short")) {
    usb3sun_test_init(FsReadOp::id | FsWriteOp::id);
    usb3sun_mock_fs_read([](const char *path, char *data, size_t data_len, size_t &actual_len) {
      if (!strcmp(path, "/clickDuration")) {
        //            [      version ][      padding ][ ???????????????? 7 bytes ]
        memcpy(data, "\x01\x00\x00\x00\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x55\x55\x55", actual_len = std::min(data_len, (size_t)15));
        return true;
      }
      if (!strcmp(path, "/forceClick")) {
        //            [      version ][ ???? 3 bytes ]
        memcpy(data, "\x01\x00\x00\x00\x02\x00\x00", actual_len = std::min(data_len, (size_t)7));
        return true;
      }
      if (!strcmp(path, "/mouseBaud")) {
        //            [      version ][ ???? 3 bytes ]
        memcpy(data, "\x01\x00\x00\x00\x02\x00\x00", actual_len = std::min(data_len, (size_t)7));
        return true;
      }
      if (!strcmp(path, "/hostid")) {
        //            [      version ][           uint8_t[6] ][??]
        memcpy(data, "\x01\x00\x00\x00\x31\x32\x33\x34\x35\x36\xAA", actual_len = std::min(data_len, (size_t)11));
        return true;
      }
      return false;
    });
    setup();
    TEST_ASSERT_EQ(settings.clickDuration, 5);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::NO);
    TEST_ASSERT_EQ(settings.mouseBaud.current, MouseBaud::_::S9600);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'0', '0', '0', '0', '0', '0'}}));
    return assert_then_clear_test_history(std::vector<Op> {
      FsReadOp {"/clickDuration.v2", 8, {}},
      FsReadOp {"/clickDuration", 16, bytes(15, "\x01\x00\x00\x00\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x55\x55\x55")},
      FsReadOp {"/forceClick.v2", 4, {}},
      FsReadOp {"/forceClick", 8, bytes(7, "\x01\x00\x00\x00\x02\x00\x00")},
      FsReadOp {"/mouseBaud.v2", 4, {}},
      FsReadOp {"/mouseBaud", 8, bytes(7, "\x01\x00\x00\x00\x02\x00\x00")},
      FsReadOp {"/hostid.v2", 6, {}},
      FsReadOp {"/hostid", 12, bytes(11, "\x01\x00\x00\x00\x31\x32\x33\x34\x35\x36\xAA")},
    });
  }

  if (!strcmp(test_name, "view_stack")) {
    usb3sun_test_init(0);
    setup();
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    TEST_ASSERT_EQ(View::peek(), &MENU_VIEW);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    return true;
  }

  const auto findMenuItem = [](uint8_t usbkSelector, MenuItem targetItem) {
    auto oldItem = MENU_VIEW.selectedItem;
    while (MENU_VIEW.selectedItem != (size_t)targetItem) {
      View::sendMakeBreak({}, usbkSelector);
      if (MENU_VIEW.selectedItem == oldItem)
        usb3sun_panic("BUG: menu item not found!\n");
      oldItem = MENU_VIEW.selectedItem;
    }
  };

  if (!strcmp(test_name, "menu_settings")) {
    usb3sun_test_init(FsWriteOp::id | SunmInitOp::id | RebootOp::id | BuzzerStartOp::id | AlarmOp::id);
    setup();
    if (!assert_then_clear_test_history(std::vector<Op> {
#ifdef SUNM_ENABLE
      SunmInitOp {9600},
#endif
    })) return false;

    // no confirm-save when menu was not touched.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);

    // confirm-save when force click setting is changed,
    // but no settings change or write if we say no.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::ForceClick);
    View::sendMakeBreak({}, USBK_RIGHT); // → Force click: off
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ESCAPE); // cancel
    TEST_ASSERT_EQ(View::peek(), &MENU_VIEW);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_N); // don't save
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::NO);
    if (!assert_then_clear_test_history(std::vector<Op> {
    })) return false;

    // confirm-save when click duration setting is changed,
    // but no settings change or write if we say no.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::ClickDuration);
    View::sendMakeBreak({}, USBK_RIGHT); // → Click duration: 10 ms
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ESCAPE); // cancel
    TEST_ASSERT_EQ(View::peek(), &MENU_VIEW);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_N); // don't save
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.clickDuration, 5);
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {1000},
    })) return false;

    // confirm-save when mouse baud setting is changed,
    // but no settings change or write if we say no.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::MouseBaud);
    View::sendMakeBreak({}, USBK_LEFT); // → Mouse baud: 4800
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ESCAPE); // cancel
    TEST_ASSERT_EQ(View::peek(), &MENU_VIEW);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_N); // don't save
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.mouseBaudReal(), 9600);
    if (!assert_then_clear_test_history(std::vector<Op> {
    })) return false;

    // no confirm-save when settings are changed and changed back.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::ForceClick);
    View::sendMakeBreak({}, USBK_RIGHT); // → Force click: off
    View::sendMakeBreak({}, USBK_LEFT); // → Force click: no
    findMenuItem(USBK_DOWN, MenuItem::ClickDuration);
    View::sendMakeBreak({}, USBK_RIGHT); // → Click duration: 10 ms
    View::sendMakeBreak({}, USBK_LEFT); // → Click duration: 5 ms
    findMenuItem(USBK_DOWN, MenuItem::MouseBaud);
    View::sendMakeBreak({}, USBK_LEFT); // → Mouse baud: 4800
    View::sendMakeBreak({}, USBK_RIGHT); // → Mouse baud: 9600
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {1000},
      BuzzerStartOp {1000},
    })) return false;

    // when the force click setting is changed, the setting should change in memory,
    // and we should issue a fs write for only the changed setting.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::ForceClick);
    View::sendMakeBreak({}, USBK_RIGHT); // → Force click: off
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ENTER); // save settings
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::OFF);
    if (!assert_then_clear_test_history(std::vector<Op> {
      FsWriteOp {"/forceClick.v2", bytes(4, "\x01\x00\x00\x00")},
    })) return false;

    // when the click duration setting is changed, the setting should change in memory,
    // and we should issue a fs write for only the changed setting.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::ClickDuration);
    View::sendMakeBreak({}, USBK_RIGHT); // → Click duration: 10 ms
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ENTER); // save settings
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.clickDuration, 10);
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {1000},
      FsWriteOp {"/clickDuration.v2", bytes(8, "\x0A\x00\x00\x00\x00\x00\x00\x00")},
    })) return false;

    // when the mouse baud setting is changed, the setting should change in memory,
    // we should issue a fs write for only the changed setting,
    // and we should reinit the sun mouse interface.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::MouseBaud);
    View::sendMakeBreak({}, USBK_LEFT); // → Mouse baud: 4800
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ENTER); // save settings
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.mouseBaudReal(), 4800);
    if (!assert_then_clear_test_history(std::vector<Op> {
      FsWriteOp {"/mouseBaud.v2", bytes(4, "\x02\x00\x00\x00")},
#ifdef SUNM_ENABLE
      SunmInitOp {4800},
#endif
    })) return false;

    // when saving settings, we should reboot if requested.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::ForceClick);
    View::sendMakeBreak({}, USBK_RIGHT); // → Force click: on
    findMenuItem(USBK_DOWN, MenuItem::ClickDuration);
    View::sendMakeBreak({}, USBK_RIGHT); // → Click duration: 15 ms
    findMenuItem(USBK_DOWN, MenuItem::MouseBaud);
    View::sendMakeBreak({}, USBK_LEFT); // → Mouse baud: 2400
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak(USBK_SHIFT_L, USBK_ENTER); // save settings
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.forceClick.current, ForceClick::_::ON);
    TEST_ASSERT_EQ(settings.clickDuration, 15);
    TEST_ASSERT_EQ(settings.mouseBaudReal(), 2400);
    if (!assert_then_clear_test_history(std::vector<Op> {
      BuzzerStartOp {1000},
      FsWriteOp {"/clickDuration.v2", bytes(8, "\x0F\x00\x00\x00\x00\x00\x00\x00")},
      FsWriteOp {"/forceClick.v2", bytes(4, "\x02\x00\x00\x00")},
      FsWriteOp {"/mouseBaud.v2", bytes(4, "\x01\x00\x00\x00")},
      RebootOp {},
    })) return false;

    return true;
  }

  if (!strcmp(test_name, "menu_hostid")) {
    usb3sun_test_init(FsWriteOp::id | SunkWriteOp::id);
    setup();

    // no confirm-save when hostid is changed but we say cancel.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::Hostid);
    View::sendMakeBreak({}, USBK_RETURN); // Hostid: 000000
    View::sendMakeBreak({}, USBK_1); // → 100000
    View::sendMakeBreak({}, USBK_ESCAPE); // cancel
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'0', '0', '0', '0', '0', '0'}}));
    if (!assert_then_clear_test_history(std::vector<Op> {
#ifdef SUNK_ENABLE
      SunkWriteOp {bytes(1, "\xD9")}, // SUNK_RETURN
      SunkWriteOp {bytes(1, "\x7F")}, // SUNK_IDLE
#endif
    })) return false;

    // confirm-save when hostid is changed and we say ok,
    // and use that hostid when asked to reprogram idprom,
    // but no settings change or write if we say no.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::Hostid);
    View::sendMakeBreak({}, USBK_RETURN); // Hostid: 000000
    View::sendMakeBreak({}, USBK_1); // → 100000
    View::sendMakeBreak({}, USBK_RETURN); // ok
    findMenuItem(USBK_DOWN, MenuItem::ReprogramIdprom);
    View::sendMakeBreak({}, USBK_RETURN); // Reprogram idprom
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_ESCAPE); // cancel
    TEST_ASSERT_EQ(View::peek(), &MENU_VIEW);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_N); // don't save
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'0', '0', '0', '0', '0', '0'}}));
    std::vector<Op> expected{};
#ifdef SUNK_ENABLE
    for (const auto &octet : bytes(
      1310,
      "\x1E\x9E\x7F\x79\xF9\x7F\x27\xA7\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x25\xA5\x7F\x27\xA7\x7F\x79\xF9\x7F\x1E\x9E\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x39\xB9\x7F\x38\xB8\x7F\x4D\xCD\x7F\x55\xD5\x7F\x28"
      "\xA8\x7F\x6A\xEA\x7F\x4D\xCD\x7F\x66\xE6\x7F\x52\xD2\x7F\x3D\xBD\x7F\x69\xE9\x7F\x38\xB8\x7F\x28\xA8\x7F\x3A\xBA\x7F\x3B\xBB\x7F\x3F\xBF\x7F\x38\xB8\x7F\x79\xF9\x7F\x1E\x9E\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x25\xA5\x7F\x79\xF9"
      "\x7F\x1F\x9F\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x20\xA0\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1F\x9F\x7F\x27\xA7\x7F\x79\xF9\x7F\x21\xA1\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F"
      "\x3F\xBF\x7F\x59\xD9\x7F\x1E\x9E\x7F\x27\xA7\x7F\x79\xF9\x7F\x22\xA2\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x23\xA3\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x24"
      "\xA4\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x25\xA5\x7F\x27\xA7\x7F\x79\xF9\x7F\x25\xA5\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x39\xB9\x7F\x38\xB8\x7F\x4D\xCD\x7F\x55\xD5\x7F\x28\xA8\x7F\x6A\xEA\x7F\x4D\xCD"
      "\x7F\x66\xE6\x7F\x52\xD2\x7F\x3D\xBD\x7F\x69\xE9\x7F\x38\xB8\x7F\x28\xA8\x7F\x3A\xBA\x7F\x3B\xBB\x7F\x3F\xBF\x7F\x38\xB8\x7F\x79\xF9\x7F\x25\xA5\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x26\xA6\x7F\x79\xF9\x7F"
      "\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x4D\xCD\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x68\xE8\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1E\x9E\x7F\x27"
      "\xA7\x7F\x79\xF9\x7F\x66\xE6\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x4F\xCF\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x38\xB8\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4"
      "\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1F\x9F\x7F\x26\xA6\x7F\x79\xF9\x7F\x50\xD0\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x3C\xBC\x7F\x3F\xBF\x7F\x4F\xCF\x7F\x4D\xCD\x7F\x3A\xBA\x7F\x38\xB8\x7F\x28\xA8\x7F\x4E\xCE\x7F\x3B\xBB\x7F\x4E\xCE\x7F"
      "\x3A\xBA\x7F\x38\xB8\x7F\x6A\xEA\x7F\x28\xA8\x7F\x3D\xBD\x7F\x4F\xCF\x7F\x3F\xBF\x7F\x39\xB9\x7F\x3E\xBE\x7F\x6A\xEA\x7F\x59\xD9\x7F\x6C\xEC\x7F\x3D\xBD\x7F\x4F\xCF\x7F\x3F\xBF\x7F\x39\xB9\x7F\x3E\xBE\x7F\x6A\xEA\x7F\x59\xD9\x7F\x68\xE8\x7F\x4D\xCD\x7F\x69"
      "\xE9\x7F\x69\xE9\x7F\x38\xB8\x7F\x39\xB9\x7F\x59\xD9\x7F\x1E\x9E\x7F\x79\xF9\x7F\x27\xA7\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x25\xA5\x7F\x27\xA7\x7F\x79\xF9\x7F\x1E\x9E\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9"
      "\x7F\x39\xB9\x7F\x38\xB8\x7F\x4D\xCD\x7F\x55\xD5\x7F\x28\xA8\x7F\x6A\xEA\x7F\x4D\xCD\x7F\x66\xE6\x7F\x52\xD2\x7F\x3D\xBD\x7F\x69\xE9\x7F\x38\xB8\x7F\x28\xA8\x7F\x3A\xBA\x7F\x3B\xBB\x7F\x3F\xBF\x7F\x38\xB8\x7F\x79\xF9\x7F\x1E\x9E\x7F\x79\xF9\x7F\x6A\xEA\x7F"
      "\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x25\xA5\x7F\x79\xF9\x7F\x1F\x9F\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x20\xA0\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1F\x9F\x7F\x27\xA7\x7F\x79"
      "\xF9\x7F\x21\xA1\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1E\x9E\x7F\x27\xA7\x7F\x79\xF9\x7F\x22\xA2\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x23\xA3\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4"
      "\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x24\xA4\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x25\xA5\x7F\x27\xA7\x7F\x79\xF9\x7F\x25\xA5\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x39\xB9\x7F\x38\xB8\x7F"
      "\x4D\xCD\x7F\x55\xD5\x7F\x28\xA8\x7F\x6A\xEA\x7F\x4D\xCD\x7F\x66\xE6\x7F\x52\xD2\x7F\x3D\xBD\x7F\x69\xE9\x7F\x38\xB8\x7F\x28\xA8\x7F\x3A\xBA\x7F\x3B\xBB\x7F\x3F\xBF\x7F\x38\xB8\x7F\x79\xF9\x7F\x25\xA5\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59"
      "\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x26\xA6\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x4D\xCD\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x68\xE8\x7F\x79\xF9\x7F\x6A\xEA"
      "\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1E\x9E\x7F\x27\xA7\x7F\x79\xF9\x7F\x66\xE6\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F\x79\xF9\x7F\x4F\xCF\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x27\xA7\x7F"
      "\x79\xF9\x7F\x38\xB8\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x1F\x9F\x7F\x26\xA6\x7F\x79\xF9\x7F\x50\xD0\x7F\x79\xF9\x7F\x6A\xEA\x7F\x54\xD4\x7F\x3F\xBF\x7F\x59\xD9\x7F\x3C\xBC\x7F\x3F\xBF\x7F\x4F\xCF\x7F\x4D\xCD\x7F\x3A\xBA\x7F\x38"
      "\xB8\x7F\x28\xA8\x7F\x4E\xCE\x7F\x3B\xBB\x7F\x4E\xCE\x7F\x3A\xBA\x7F\x38\xB8\x7F\x6A\xEA\x7F\x28\xA8\x7F\x3D\xBD\x7F\x4F\xCF\x7F\x3F\xBF\x7F\x39\xB9\x7F\x3E\xBE\x7F\x6A\xEA\x7F\x59\xD9\x7F\x6C\xEC\x7F\x3D\xBD\x7F\x4F\xCF\x7F\x3F\xBF\x7F\x39\xB9\x7F\x3E\xBE"
      "\x7F\x6A\xEA\x7F\x59\xD9\x7F\x68\xE8\x7F\x4D\xCD\x7F\x69\xE9\x7F\x69\xE9\x7F\x38\xB8\x7F\x39\xB9\x7F\x59\xD9\x7F\xE9\x7F"
    )) {
      expected.push_back(SunkWriteOp {bytes(1, &octet)});
    }
#endif
    if (!assert_then_clear_test_history(expected)) return false;

    // no confirm-save when hostid setting is changed and changed back.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::Hostid);
    View::sendMakeBreak({}, USBK_RETURN); // Hostid: 000000
    View::sendMakeBreak({}, USBK_1); // → 100000
    View::sendMakeBreak({}, USBK_RETURN); // ok
    View::sendMakeBreak({}, USBK_RETURN); // Hostid: 100000
    View::sendMakeBreak({}, USBK_0); // → 000000
    View::sendMakeBreak({}, USBK_RETURN); // ok
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'0', '0', '0', '0', '0', '0'}}));
    if (!assert_then_clear_test_history(std::vector<Op> {
#ifdef SUNK_ENABLE
      SunkWriteOp {bytes(1, "\xD9")}, // SUNK_RETURN
      SunkWriteOp {bytes(1, "\x7F")}, // SUNK_IDLE
#endif
    })) return false;

    // when the hostid setting is changed, the setting should change in memory,
    // and we should issue a fs write for only the changed setting.
    View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
    findMenuItem(USBK_DOWN, MenuItem::Hostid);
    View::sendMakeBreak({}, USBK_RETURN); // Hostid: 000000
    View::sendMakeBreak({}, USBK_1); // → 100000
    View::sendMakeBreak({}, USBK_RETURN); // ok
    findMenuItem(USBK_UP, MenuItem::GoBack);
    View::sendMakeBreak({}, USBK_RETURN); // Go back
    TEST_ASSERT_EQ(View::peek(), &SAVE_SETTINGS_VIEW);
    View::sendMakeBreak({}, USBK_RETURN); // save settings
    TEST_ASSERT_EQ(View::peek(), &DEFAULT_VIEW);
    TEST_ASSERT_EQ(settings.hostid, (HostidV2::Value {{'1', '0', '0', '0', '0', '0'}}));
    if (!assert_then_clear_test_history(std::vector<Op> {
      FsWriteOp {"/hostid.v2", bytes(6, "\x31\x30\x30\x30\x30\x30")},
#ifdef SUNK_ENABLE
      SunkWriteOp {bytes(1, "\xD9")}, // SUNK_RETURN
      SunkWriteOp {bytes(1, "\x7F")}, // SUNK_IDLE
#endif
    })) return false;

    return true;
  }

  help();
  return false;
}

static void handleDemoInput(std::optional<uint8_t> cur) {
  static enum {
    NORMAL,
    ESC,
    CSI,
  } inputState = NORMAL;
  switch (inputState) {
    case NORMAL: {
      if (cur.has_value()) {
        switch (*cur) {
          case '\x1B': // escape/alt in terminal (^[)
            inputState = ESC;
            break;
          case 'q':
            usb3sun_test_terminal_demo_mode(false);
            exit(0);
            break;
          default:
            if (auto usbk = ASCII_TO_USBK[*cur]) {
              View::sendMakeBreak({}, usbk);
            } else {
              Sprintf("\033[33m%02X\033[0m", *cur);
            }
            break;
        }
      }
    } break;
    case ESC: {
      if (cur.has_value()) {
        switch (*cur) {
          case '[': // control sequence introducer (^[[)
            inputState = CSI;
            goto end;
          case ' ':
            View::sendMakeBreak(USBK_CTRL_R, USBK_SPACE);
            break;
          default:
            if (*cur >= ' ' && *cur <= '~') {
              Sprintf("\033[33m^[\033[0m%c", *cur);
            } else {
              Sprintf("\033[33m^[%02X\033[0m", *cur);
            }
            break;
        }
        inputState = NORMAL;
      } else {
        // escape key
        View::sendMakeBreak({}, USBK_ESCAPE);
        inputState = NORMAL;
      }
    } break;
    case CSI: {
      if (cur.has_value()) {
        switch (*cur) {
          case 'A': // CUU
            View::sendMakeBreak({}, USBK_UP);
            break;
          case 'B': // CUD
            View::sendMakeBreak({}, USBK_DOWN);
            break;
          case 'C': // CUF
            View::sendMakeBreak({}, USBK_RIGHT);
            break;
          case 'D': // CUB
            View::sendMakeBreak({}, USBK_LEFT);
            break;
        }
        if (*cur >= 0x40 && *cur <= 0x7E) {
          inputState = NORMAL;
        }
      }
    } break;
    default: abort();
  }
end:
  ;
}

static void initDisplay(const char *outputPath) {
  fprintf(stderr, ">>> to see the display, cat %s\n", outputPath);
  if (mkfifo(outputPath, 0666) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo");
      return;
    }
  }
  if (auto fd = open(outputPath, O_WRONLY); fd != -1) {
    usb3sun_mock_display_output(fd);
  } else {
    perror("open");
  }
}

int main(int argc, char **argv) {
  if (argc >= 2) {
    const char *test_name = argv[1];
    if (!strcmp(test_name, "demo")) {
      if (argc >= 3) {
        initDisplay(argv[2]);
      } else {
        char displayPath[] = "/tmp/usb3sun.XXXXXX\0display";
        if (mkdtemp(displayPath)) {
          displayPath[19] = '/';
          initDisplay(displayPath);
        } else {
          perror("mkdtemp");
        }
      }
      usb3sun_test_init(0);
      usb3sun_test_exit_on_reboot();
      usb3sun_test_terminal_demo_mode(true);
      setup();
      setup1();
      while (true) {
        constexpr size_t escAltTimeout = 100'000ul;
        static char prev = '\0';
        static uint64_t tLastEsc = usb3sun_micros();
        bool hadInput = false;
        int input;
        while ((input = usb3sun_debug_uart_read()) != -1) {
          handleDemoInput(static_cast<uint8_t>(input));
          hadInput = true;
          prev = input;
          if (input == /* ESC */ '\033') {
            tLastEsc = usb3sun_micros();
          }
        }
        if (!hadInput && prev == /* ESC */ '\033') {
          if (usb3sun_micros() - tLastEsc > escAltTimeout) {
            handleDemoInput({});
          }
        }
        loop();
        loop1();
      }
    } else if (!strcmp(test_name, "all")) {
      for (const char *&name : test_names) {
        std::cerr << ">>> starting test: " << name << "\n";
        test_name = name;
        pid_t pid = fork();
        if (pid == 0) {
          if (run_test(test_name)) {
            return 0;
          } else {
            std::cerr << ">>> test failed: " << test_name << "\n";
            return 1;
          }
        } else if (pid == -1) {
          perror("fatal: fork");
          return 1;
        } else {
          int result;
          wait(&result);
          if (!WIFEXITED(result) || WEXITSTATUS(result) != 0) {
            std::cerr << "fatal: wait(2) returned " << result << "\n";
            return 1;
          }
        }
      }
      return 0;
    } else {
      return run_test(test_name) ? 0 : 1;
    }
  }
  help();
  return 0;
}

#endif
