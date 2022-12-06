#define USBK_RESERVED 0
#define USBK_ERROR_ROLLOVER 1
#define USBK_FIRST_KEYCODE 4
#define SUN_MTX SUN_PIN4
#define SUN_KRX SUN_PIN5
#define SUN_KTX SUN_PIN6
#define SUN_PIN1 SUN_PURPLE   // 0 V
#define SUN_PIN2 SUN_BROWN    // 0 V
#define SUN_PIN3 SUN_BLUE     // +5 Vdc
#define SUN_PIN4 SUN_GRAY     // mouse tx
#define SUN_PIN5 SUN_RED      // keyboard rx
#define SUN_PIN6 SUN_GREEN    // keyboard tx
#define SUN_PIN7 SUN_YELLOW   // 0 V
#define SUN_PIN8 SUN_ORANGE   // +5 Vdc
#define SUN_GRAY 14
#define SUN_GREEN 15
#define SUN_RED 18
#define DISPLAY_SCL 17
#define DISPLAY_SDA 16
#define DISPLAY_ROTATION 0

#define Sprint(...) (Serial.print(__VA_ARGS__), Serial.flush())
#define Sprintln(...) (Serial.println(__VA_ARGS__), Serial.flush())
#define Sprintf(...) (Serial.printf(__VA_ARGS__), Serial.flush())

#include <Arduino.h>
#include <Wire.h>

#include <pio_usb.h>
#define HOST_PIN_DP   2   // Pin used as D+ for host, D- = D+ + 1

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>

const char *const MODIFIER_NAMES[] = {
  "CtrlL", "ShiftL", "AltL", "SuperL",
  "CtrlR", "ShiftR", "AltR", "SuperR",
};

// USB Host object
Adafruit_USBH_Host USBHost;
// holding device descriptor
tusb_desc_device_t desc_device;

static Adafruit_SSD1306 display(128, 32, &Wire, /* OLED_RESET */ -1);

struct {
  bool caps;
  bool compose;
  bool scroll;
  bool num;
  uint8_t lastModifiers;
  uint8_t lastKeys[6];
} state;

void setup() {
  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(SUN_MTX, OUTPUT);
  pinMode(SUN_KRX, INPUT);
  pinMode(SUN_KTX, OUTPUT);
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

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);
  static int i = 0;
  display.printf("%d", i++);
  drawStatus(0, 18, "CAP", state.caps);
  drawStatus(26, 18, "CMP", state.compose);
  drawStatus(52, 18, "SCR", state.scroll);
  drawStatus(78, 18, "NUM", state.num);
  display.display();
  delay(1000);
}

void setup1() {
  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
    while ( !Serial ) delay(10);   // wait for native usb
    Sprintf("Error: CPU Clock = %u, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
    Sprintf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n", cpu_hz);
    while(1) delay(1);
  }

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = HOST_PIN_DP;
  USBHost.configure_pio_usb(1, &pio_cfg);

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
}

void loop1() {
  USBHost.task();
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

  tuh_hid_report_info_t infos[16];
  Sprintf("%u\n", tuh_hid_parse_report_descriptor(infos, sizeof(infos) / sizeof(*infos), desc_report, desc_len));
  Sprintf("report_id=%u, usage=%u, usage_page=%u\n", infos[0].report_id, infos[0].usage, infos[0].usage_page);

  Sprintf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  Sprintf("VID = %04x, PID = %04x\r\n", vid, pid);
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Sprintf("Error: cannot request to receive report\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  Sprintf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  Sprintf("report");
  for (uint16_t i = 0; i < len; i++)
    Sprintf(" %02Xh", report[i]);

  switch (tuh_hid_interface_protocol(dev_addr, instance)) {
    case HID_ITF_PROTOCOL_KEYBOARD: {
      hid_keyboard_report_t *kreport = (hid_keyboard_report_t *) report;

      for (int i = 0; i < 6; i++) {
        if (kreport->keycode[i] != USBK_RESERVED && kreport->keycode[i] < USBK_FIRST_KEYCODE) {
          Sprintf(" !%u", kreport->keycode[i]);
          goto out;
        }
      }

      for (int i = 0; i < 8; i++)
        if ((state.lastModifiers & 1 << i) != (kreport->modifier & 1 << i))
          Sprintf(" %c%s", kreport->modifier & 1 << i ? '+' : '-', MODIFIER_NAMES[i]);

      for (int i = 0; i < 6; i++) {
        bool oldInNews = false;
        bool newInOlds = false;
        for (int j = 0; j < 6; j++) {
          if (state.lastKeys[i] == kreport->keycode[j])
            oldInNews = true;
          if (kreport->keycode[i] == state.lastKeys[j])
            newInOlds = true;
        }
        if (!oldInNews && state.lastKeys[i] >= USBK_FIRST_KEYCODE)
          Sprintf(" -%u", state.lastKeys[i]);
        if (!newInOlds && kreport->keycode[i] >= USBK_FIRST_KEYCODE)
          Sprintf(" +%u", kreport->keycode[i]);
      }

      state.lastModifiers = kreport->modifier;
      for (int i = 0; i < 6; i++)
        state.lastKeys[i] = kreport->keycode[i];
    } break;
    case HID_ITF_PROTOCOL_MOUSE:
      break;
  }
out:

  Sprintln();
  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Sprintf("Error: cannot request to receive report\r\n");
  }
}
