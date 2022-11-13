#include <Arduino.h>
#include <Wire.h>

#include <pio_usb.h>
#define HOST_PIN_DP   2   // Pin used as D+ for host, D- = D+ + 1

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>

#define LANGUAGE_ID 0x0409  // English
// USB Host object
Adafruit_USBH_Host USBHost;
// holding device descriptor
tusb_desc_device_t desc_device;

static Adafruit_SSD1306 display(128, 32, &Wire1, /* OLED_RESET */ -1);

void loop() {
  // without this println, no Core1 setup or any other USB messages
  Serial.println("this println is needed or everything fucking breaks");

  pinMode(18, OUTPUT_12MA);
  pinMode(19, OUTPUT_12MA);

  display.clearDisplay();
  display.setCursor(0, 0);
  static int i = 0;
  display.printf("%d", i++);
  display.setCursor(0, 12);
  display.print("CAP  CMP  SCR  NUM");
  display.drawRoundRect(0, 20, 18, 12, 4, SSD1306_WHITE);
  display.drawRoundRect(30, 20, 18, 12, 4, SSD1306_WHITE);
  display.drawRoundRect(60, 20, 18, 12, 4, SSD1306_WHITE);
  display.drawRoundRect(90, 20, 18, 12, 4, SSD1306_WHITE);
  display.display();

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(18, LOW);
  digitalWrite(19, HIGH);
  delay(420);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(18, HIGH);
  digitalWrite(19, LOW);
  delay(420);
  delay(1000);
}

void setup() {
  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, /* SCREEN_ADDRESS */ 0x3C);
  display.setRotation(2);
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.clearDisplay();

  Serial1.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("TinyUSB Dual Device Info Example");
}

void setup1() {
  delay(5000); ///////
  Serial.println("Core1 setup to run TinyUSB host with pio-usb");

  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
    while ( !Serial ) delay(10);   // wait for native usb
    Serial.printf("Error: CPU Clock = %u, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
    Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n", cpu_hz);
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
  // Serial.println("loop1");
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

  Serial.printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  Serial.printf("VID = %04x, PID = %04x\r\n", vid, pid);
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Serial.printf("Error: cannot request to receive report\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  Serial.printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  Serial.printf("HIDreport : ");
  for (uint16_t i = 0; i < len; i++) {
    Serial.printf("0x%02X ", report[i]);
  }
  Serial.println();
  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Serial.printf("Error: cannot request to receive report\r\n");
  }
}
