#include "config.h"
#include "hal.h"

#include <cstdint>

// Adafruit GFX Library 1.11.5, glcdfont.c
static const uint8_t adafruit_gfx_classic[]  = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x5B, 0x4F, 0x5B, 0x3E, 0x3E, 0x6B,
  0x4F, 0x6B, 0x3E, 0x1C, 0x3E, 0x7C, 0x3E, 0x1C, 0x18, 0x3C, 0x7E, 0x3C,
  0x18, 0x1C, 0x57, 0x7D, 0x57, 0x1C, 0x1C, 0x5E, 0x7F, 0x5E, 0x1C, 0x00,
  0x18, 0x3C, 0x18, 0x00, 0xFF, 0xE7, 0xC3, 0xE7, 0xFF, 0x00, 0x18, 0x24,
  0x18, 0x00, 0xFF, 0xE7, 0xDB, 0xE7, 0xFF, 0x30, 0x48, 0x3A, 0x06, 0x0E,
  0x26, 0x29, 0x79, 0x29, 0x26, 0x40, 0x7F, 0x05, 0x05, 0x07, 0x40, 0x7F,
  0x05, 0x25, 0x3F, 0x5A, 0x3C, 0xE7, 0x3C, 0x5A, 0x7F, 0x3E, 0x1C, 0x1C,
  0x08, 0x08, 0x1C, 0x1C, 0x3E, 0x7F, 0x14, 0x22, 0x7F, 0x22, 0x14, 0x5F,
  0x5F, 0x00, 0x5F, 0x5F, 0x06, 0x09, 0x7F, 0x01, 0x7F, 0x00, 0x66, 0x89,
  0x95, 0x6A, 0x60, 0x60, 0x60, 0x60, 0x60, 0x94, 0xA2, 0xFF, 0xA2, 0x94,
  0x08, 0x04, 0x7E, 0x04, 0x08, 0x10, 0x20, 0x7E, 0x20, 0x10, 0x08, 0x08,
  0x2A, 0x1C, 0x08, 0x08, 0x1C, 0x2A, 0x08, 0x08, 0x1E, 0x10, 0x10, 0x10,
  0x10, 0x0C, 0x1E, 0x0C, 0x1E, 0x0C, 0x30, 0x38, 0x3E, 0x38, 0x30, 0x06,
  0x0E, 0x3E, 0x0E, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F,
  0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14,
  0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62, 0x36, 0x49,
  0x56, 0x20, 0x50, 0x00, 0x08, 0x07, 0x03, 0x00, 0x00, 0x1C, 0x22, 0x41,
  0x00, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x08,
  0x08, 0x3E, 0x08, 0x08, 0x00, 0x80, 0x70, 0x30, 0x00, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x00, 0x00, 0x60, 0x60, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02,
  0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x72, 0x49,
  0x49, 0x49, 0x46, 0x21, 0x41, 0x49, 0x4D, 0x33, 0x18, 0x14, 0x12, 0x7F,
  0x10, 0x27, 0x45, 0x45, 0x45, 0x39, 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x41,
  0x21, 0x11, 0x09, 0x07, 0x36, 0x49, 0x49, 0x49, 0x36, 0x46, 0x49, 0x49,
  0x29, 0x1E, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x40, 0x34, 0x00, 0x00,
  0x00, 0x08, 0x14, 0x22, 0x41, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x41,
  0x22, 0x14, 0x08, 0x02, 0x01, 0x59, 0x09, 0x06, 0x3E, 0x41, 0x5D, 0x59,
  0x4E, 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E,
  0x41, 0x41, 0x41, 0x22, 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x7F, 0x49, 0x49,
  0x49, 0x41, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x3E, 0x41, 0x41, 0x51, 0x73,
  0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x20, 0x40,
  0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40,
  0x40, 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E,
  0x41, 0x41, 0x41, 0x3E, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x51,
  0x21, 0x5E, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x26, 0x49, 0x49, 0x49, 0x32,
  0x03, 0x01, 0x7F, 0x01, 0x03, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x1F, 0x20,
  0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x63, 0x14, 0x08, 0x14,
  0x63, 0x03, 0x04, 0x78, 0x04, 0x03, 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00,
  0x7F, 0x41, 0x41, 0x41, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41,
  0x41, 0x7F, 0x04, 0x02, 0x01, 0x02, 0x04, 0x40, 0x40, 0x40, 0x40, 0x40,
  0x00, 0x03, 0x07, 0x08, 0x00, 0x20, 0x54, 0x54, 0x78, 0x40, 0x7F, 0x28,
  0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x44, 0x28, 0x38, 0x44, 0x44, 0x28,
  0x7F, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x08, 0x7E, 0x09, 0x02, 0x18,
  0xA4, 0xA4, 0x9C, 0x78, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D,
  0x40, 0x00, 0x20, 0x40, 0x40, 0x3D, 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,
  0x00, 0x41, 0x7F, 0x40, 0x00, 0x7C, 0x04, 0x78, 0x04, 0x78, 0x7C, 0x08,
  0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44, 0x38, 0xFC, 0x18, 0x24, 0x24,
  0x18, 0x18, 0x24, 0x24, 0x18, 0xFC, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x48,
  0x54, 0x54, 0x54, 0x24, 0x04, 0x04, 0x3F, 0x44, 0x24, 0x3C, 0x40, 0x40,
  0x20, 0x7C, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x3C, 0x40, 0x30, 0x40, 0x3C,
  0x44, 0x28, 0x10, 0x28, 0x44, 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x44, 0x64,
  0x54, 0x4C, 0x44, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x77, 0x00,
  0x00, 0x00, 0x41, 0x36, 0x08, 0x00, 0x02, 0x01, 0x02, 0x04, 0x02, 0x3C,
  0x26, 0x23, 0x26, 0x3C, 0x1E, 0xA1, 0xA1, 0x61, 0x12, 0x3A, 0x40, 0x40,
  0x20, 0x7A, 0x38, 0x54, 0x54, 0x55, 0x59, 0x21, 0x55, 0x55, 0x79, 0x41,
  0x22, 0x54, 0x54, 0x78, 0x42, // a-umlaut
  0x21, 0x55, 0x54, 0x78, 0x40, 0x20, 0x54, 0x55, 0x79, 0x40, 0x0C, 0x1E,
  0x52, 0x72, 0x12, 0x39, 0x55, 0x55, 0x55, 0x59, 0x39, 0x54, 0x54, 0x54,
  0x59, 0x39, 0x55, 0x54, 0x54, 0x58, 0x00, 0x00, 0x45, 0x7C, 0x41, 0x00,
  0x02, 0x45, 0x7D, 0x42, 0x00, 0x01, 0x45, 0x7C, 0x40, 0x7D, 0x12, 0x11,
  0x12, 0x7D, // A-umlaut
  0xF0, 0x28, 0x25, 0x28, 0xF0, 0x7C, 0x54, 0x55, 0x45, 0x00, 0x20, 0x54,
  0x54, 0x7C, 0x54, 0x7C, 0x0A, 0x09, 0x7F, 0x49, 0x32, 0x49, 0x49, 0x49,
  0x32, 0x3A, 0x44, 0x44, 0x44, 0x3A, // o-umlaut
  0x32, 0x4A, 0x48, 0x48, 0x30, 0x3A, 0x41, 0x41, 0x21, 0x7A, 0x3A, 0x42,
  0x40, 0x20, 0x78, 0x00, 0x9D, 0xA0, 0xA0, 0x7D, 0x3D, 0x42, 0x42, 0x42,
  0x3D, // O-umlaut
  0x3D, 0x40, 0x40, 0x40, 0x3D, 0x3C, 0x24, 0xFF, 0x24, 0x24, 0x48, 0x7E,
  0x49, 0x43, 0x66, 0x2B, 0x2F, 0xFC, 0x2F, 0x2B, 0xFF, 0x09, 0x29, 0xF6,
  0x20, 0xC0, 0x88, 0x7E, 0x09, 0x03, 0x20, 0x54, 0x54, 0x79, 0x41, 0x00,
  0x00, 0x44, 0x7D, 0x41, 0x30, 0x48, 0x48, 0x4A, 0x32, 0x38, 0x40, 0x40,
  0x22, 0x7A, 0x00, 0x7A, 0x0A, 0x0A, 0x72, 0x7D, 0x0D, 0x19, 0x31, 0x7D,
  0x26, 0x29, 0x29, 0x2F, 0x28, 0x26, 0x29, 0x29, 0x29, 0x26, 0x30, 0x48,
  0x4D, 0x40, 0x20, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x38, 0x2F, 0x10, 0xC8, 0xAC, 0xBA, 0x2F, 0x10, 0x28, 0x34, 0xFA, 0x00,
  0x00, 0x7B, 0x00, 0x00, 0x08, 0x14, 0x2A, 0x14, 0x22, 0x22, 0x14, 0x2A,
  0x14, 0x08, 0x55, 0x00, 0x55, 0x00, 0x55, // #176 (25% block) missing in old
                                            // code
  0xAA, 0x55, 0xAA, 0x55, 0xAA,             // 50% block
  0xFF, 0x55, 0xFF, 0x55, 0xFF,             // 75% block
  0x00, 0x00, 0x00, 0xFF, 0x00, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x14, 0x14,
  0x14, 0xFF, 0x00, 0x10, 0x10, 0xFF, 0x00, 0xFF, 0x10, 0x10, 0xF0, 0x10,
  0xF0, 0x14, 0x14, 0x14, 0xFC, 0x00, 0x14, 0x14, 0xF7, 0x00, 0xFF, 0x00,
  0x00, 0xFF, 0x00, 0xFF, 0x14, 0x14, 0xF4, 0x04, 0xFC, 0x14, 0x14, 0x17,
  0x10, 0x1F, 0x10, 0x10, 0x1F, 0x10, 0x1F, 0x14, 0x14, 0x14, 0x1F, 0x00,
  0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10,
  0x10, 0x1F, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0x00, 0x00, 0x00, 0xFF,
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x10, 0x00,
  0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x1F,
  0x10, 0x17, 0x00, 0x00, 0xFC, 0x04, 0xF4, 0x14, 0x14, 0x17, 0x10, 0x17,
  0x14, 0x14, 0xF4, 0x04, 0xF4, 0x00, 0x00, 0xFF, 0x00, 0xF7, 0x14, 0x14,
  0x14, 0x14, 0x14, 0x14, 0x14, 0xF7, 0x00, 0xF7, 0x14, 0x14, 0x14, 0x17,
  0x14, 0x10, 0x10, 0x1F, 0x10, 0x1F, 0x14, 0x14, 0x14, 0xF4, 0x14, 0x10,
  0x10, 0xF0, 0x10, 0xF0, 0x00, 0x00, 0x1F, 0x10, 0x1F, 0x00, 0x00, 0x00,
  0x1F, 0x14, 0x00, 0x00, 0x00, 0xFC, 0x14, 0x00, 0x00, 0xF0, 0x10, 0xF0,
  0x10, 0x10, 0xFF, 0x10, 0xFF, 0x14, 0x14, 0x14, 0xFF, 0x14, 0x10, 0x10,
  0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x38, 0x44, 0x44,
  0x38, 0x44, 0xFC, 0x4A, 0x4A, 0x4A, 0x34, // sharp-s or beta
  0x7E, 0x02, 0x02, 0x06, 0x06, 0x02, 0x7E, 0x02, 0x7E, 0x02, 0x63, 0x55,
  0x49, 0x41, 0x63, 0x38, 0x44, 0x44, 0x3C, 0x04, 0x40, 0x7E, 0x20, 0x1E,
  0x20, 0x06, 0x02, 0x7E, 0x02, 0x02, 0x99, 0xA5, 0xE7, 0xA5, 0x99, 0x1C,
  0x2A, 0x49, 0x2A, 0x1C, 0x4C, 0x72, 0x01, 0x72, 0x4C, 0x30, 0x4A, 0x4D,
  0x4D, 0x30, 0x30, 0x48, 0x78, 0x48, 0x30, 0xBC, 0x62, 0x5A, 0x46, 0x3D,
  0x3E, 0x49, 0x49, 0x49, 0x00, 0x7E, 0x01, 0x01, 0x01, 0x7E, 0x2A, 0x2A,
  0x2A, 0x2A, 0x2A, 0x44, 0x44, 0x5F, 0x44, 0x44, 0x40, 0x51, 0x4A, 0x44,
  0x40, 0x40, 0x44, 0x4A, 0x51, 0x40, 0x00, 0x00, 0xFF, 0x01, 0x03, 0xE0,
  0x80, 0xFF, 0x00, 0x00, 0x08, 0x08, 0x6B, 0x6B, 0x08, 0x36, 0x12, 0x36,
  0x24, 0x36, 0x06, 0x0F, 0x09, 0x0F, 0x06, 0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x00, 0x10, 0x10, 0x00, 0x30, 0x40, 0xFF, 0x01, 0x01, 0x00, 0x1F,
  0x01, 0x01, 0x1E, 0x00, 0x19, 0x1D, 0x17, 0x12, 0x00, 0x3C, 0x3C, 0x3C,
  0x3C, 0x00, 0x00, 0x00, 0x00, 0x00 // #255 NBSP
};

#ifdef USB3SUN_HAL_ARDUINO_PICO

#define DEBUG_UART          Serial1     // UART0
#define SUNK_UART_V1        Serial1     // UART0
#define SUNK_UART_V2        Serial2     // UART1
#define SUNM_UART_V1        Serial2     // UART1

#include <pico/mutex.h>
#include <pico/platform.h>
#include <pico/time.h>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>

extern "C" {
#include <pio_usb.h>
}

static Adafruit_SSD1306 display{128, 32, &Wire, /* OLED_RESET */ -1};
static Adafruit_USBH_Host USBHost;
static SerialPIO sunkTxSnifferV1{SerialPIO::NOPIN, SUN_KTX_V1};
static SerialPIO sunkTxSnifferV2{SerialPIO::NOPIN, SUN_KTX_V2};
static SerialPIO ch9350{CH9350_TX, CH9350_RX};
static struct {
  size_t version = 1;
  HardwareSerial *sunk = &SUNK_UART_V1;
  HardwareSerial *sunkTxSniffer = &sunkTxSnifferV1;
  HardwareSerial *sunm = &SUNM_UART_V1;
  Adafruit_USBD_CDC *debugCdc = nullptr;
  SerialUART *debugUart = nullptr;
  usb3sun_pin sunkTx = SUN_KTX_V1;
  usb3sun_pin sunkRx = SUN_KRX_V1;
  usb3sun_pin sunmTx = SUN_MTX_V1;
  SerialUART *sunkUart = &SUNK_UART_V1;
  SerialUART &sunmV1 = SUNM_UART_V1;
  SerialPIO sunmV2{SUN_MTX_V2, SerialPIO::NOPIN};
} pinout;

static void draw_dot(int16_t x, int16_t y, bool inverted) {
  display.drawPixel(x, y, inverted ? SSD1306_BLACK : SSD1306_WHITE);
}

size_t usb3sun_pinout_version(void) {
  return pinout.version;
}

void usb3sun_pinout_v2(void) {
  pinout.version = 2;
  pinout.sunk = &SUNK_UART_V2;
  pinout.sunkTxSniffer = &sunkTxSnifferV2;
  pinout.sunm = &pinout.sunmV2;
  pinout.sunkTx = SUN_KTX_V2;
  pinout.sunkRx = SUN_KRX_V2;
  pinout.sunmTx = SUN_MTX_V2;
  pinout.sunkUart = &SUNK_UART_V2;
}

void usb3sun_sunk_init(void) {
  pinout.sunk->end();
  pinout.sunkUart->setPinout(pinout.sunkTx, pinout.sunkRx);
  pinout.sunk->begin(1200, SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  usb3sun_gpio_set_as_inverted(pinout.sunkTx);
  usb3sun_gpio_set_as_inverted(pinout.sunkRx);
}

int usb3sun_sunk_read(void) {
  return pinout.sunk->read();
}

size_t usb3sun_sunk_write(uint8_t *data, size_t len) {
  return pinout.sunk->write(data, len);
}

void usb3sun_sunk_sniffer_init(void) {
  pinout.sunk->end();
  // SerialUART api requires both tx and rx pins, but we can detach tx afterwards
  pinout.sunkUart->setPinout(pinout.sunkTx, pinout.sunkRx);
  pinout.sunk->begin(1200, SERIAL_8N1);
  // SerialPIO has no such requirement; this call detaches tx from hardware uart
  pinout.sunkTxSniffer->begin(1200, SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  usb3sun_gpio_set_as_inverted(pinout.sunkTx);
  usb3sun_gpio_set_as_inverted(pinout.sunkRx);
}

int usb3sun_sunk_sniff_tx(void) {
  return pinout.sunkTxSniffer->read();
}

void usb3sun_sunm_init(uint32_t baud) {
  pinout.sunm->end();
  switch (pinout.version) {
    case 1:
      pinout.sunmV1.setPinout(SUN_MTX_V1, SUN_MRX_V1);
      break;
    case 2:
      // do nothing
      break;
  }
  pinout.sunm->begin(baud, SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  usb3sun_gpio_set_as_inverted(pinout.sunmTx);
}

size_t usb3sun_sunm_write(uint8_t *data, size_t len) {
  return pinout.sunm->write(data, len);
}

void usb3sun_usb_init(void) {
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
  pio_usb_host_add_port(USB1_DP, PIO_USB_PINOUT_DPDM);
}

void usb3sun_usb_task(void) {
  USBHost.task();
}

bool usb3sun_usb_vid_pid(uint8_t dev_addr, uint16_t *vid, uint16_t *pid) {
  return tuh_vid_pid_get(dev_addr, vid, pid);
}

bool usb3sun_uhid_request_report(uint8_t dev_addr, uint8_t instance) {
  return tuh_hid_receive_report(dev_addr, instance);
}

uint8_t usb3sun_uhid_interface_protocol(uint8_t dev_addr, uint8_t instance) {
  return tuh_hid_interface_protocol(dev_addr, instance);
}

size_t usb3sun_uhid_parse_report_descriptor(usb3sun_hid_report_info *result, size_t result_len, const uint8_t *descriptor, size_t descriptor_len) {
  tuh_hid_report_info_t tuh_result[16];
  size_t tuh_result_len = tuh_hid_parse_report_descriptor(
    tuh_result, sizeof tuh_result / sizeof *tuh_result,
    descriptor, descriptor_len);
  for (size_t i = 0; i < tuh_result_len && i < result_len; i++) {
    result[i].report_id = tuh_result[i].report_id;
    result[i].usage = tuh_result[i].usage;
    result[i].usage_page = tuh_result[i].usage_page;
  }
  return tuh_result_len;
}

bool usb3sun_uhid_set_led_report(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t &led_report) {
  return tuh_hid_set_report(dev_addr, instance, report_id, HID_REPORT_TYPE_OUTPUT, &led_report, sizeof led_report);
}

void usb3sun_ch9350_init(void) {
  ch9350.begin(115200, SERIAL_8N1);
}

int usb3sun_ch9350_read(void) {
  return ch9350.read();
}

size_t usb3sun_ch9350_write(uint8_t *data, size_t len) {
  return ch9350.write(data, len);
}

void usb3sun_debug_init(int (*printf)(const char *format, ...)) {
#if defined(DEBUG_LOGGING)
  DEBUG_RP2040_PRINTF = printf;
#endif
}

int usb3sun_debug_uart_read(void) {
  return pinout.debugUart ? pinout.debugUart->read() : -1;
}

int usb3sun_debug_cdc_read(void) {
  return pinout.debugCdc ? pinout.debugCdc->read() : -1;
}

bool usb3sun_debug_write(const char *data, size_t len) {
  bool ok = true;
  if (pinout.debugCdc) {
    if (pinout.debugCdc->write(data, len) < len) {
      ok = false;
    } else {
      pinout.debugCdc->flush();
    }
  }
  if (pinout.debugUart) {
    if (pinout.debugUart->write(data, len) < len) {
      ok = false;
    } else {
      pinout.debugUart->flush();
    }
  }
  return ok;
}

void usb3sun_allow_debug_over_cdc(void) {
  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);
  pinout.debugCdc = &Serial;
}

void usb3sun_allow_debug_over_uart(void) {
  DEBUG_UART.end();
  DEBUG_UART.setPinout(DEBUG_UART_TX, DEBUG_UART_RX);
  DEBUG_UART.setFIFOSize(4096);
  DEBUG_UART.begin(DEBUG_UART_BAUD, SERIAL_8N1);
#if CFG_TUSB_DEBUG
  TinyUSB_Serial_Debug = &DEBUG_UART;
#endif
  pinout.debugUart = &DEBUG_UART;
}

bool usb3sun_fs_init(void) {
  LittleFSConfig cfg;
  cfg.setAutoFormat(true);
  LittleFS.setConfig(cfg);
  return LittleFS.begin();
}

bool usb3sun_fs_wipe(void) {
  return LittleFS.format();
}

bool usb3sun_fs_read(const char *path, char *data, size_t len) {
  if (File f = LittleFS.open(path, "r")) {
    size_t result = f.readBytes(data, len);
    f.close();
    return result == len;
  }
  return false;
}

bool usb3sun_fs_write(const char *path, const char *data, size_t len) {
  if (File f = LittleFS.open(path, "w")) {
    size_t result = f.write(data, len);
    f.close();
    return result == len;
  }
  return false;
}

void usb3sun_mutex_lock(usb3sun_mutex *mutex) {
  mutex_enter_blocking(mutex);
}

void usb3sun_mutex_unlock(usb3sun_mutex *mutex) {
  mutex_exit(mutex);
}

bool usb3sun_fifo_push(uint32_t value) {
  return rp2040.fifo.push_nb(value);
}

bool usb3sun_fifo_pop(uint32_t *result) {
  return rp2040.fifo.pop_nb(result);
}

void usb3sun_reboot(void) {
  rp2040.reboot();
}

uint64_t usb3sun_micros(void) {
  return micros();
}

void usb3sun_sleep_micros(uint64_t micros) {
  sleep_us(micros);
}

uint32_t usb3sun_clock_speed(void) {
  return clock_get_hz(clk_sys);
}

void usb3sun_panic(const char *format, ...) {
  // TODO handle format args
  panic("%s", format);
}

static int64_t alarm(alarm_id_t, void *callback) {
  reinterpret_cast<void (*)(void)>(callback)();
  return 0; // don’t reschedule
}

void usb3sun_alarm(uint32_t ms, void (*callback)(void)) {
  add_alarm_in_ms(ms, alarm, reinterpret_cast<void *>(callback), true);
}

bool usb3sun_gpio_read(uint8_t pin) {
  return gpio_get(pin);
}

void usb3sun_gpio_write(uint8_t pin, bool value) {
  digitalWrite(pin, value ? HIGH : LOW);
}

void usb3sun_gpio_set_as_inverted(uint8_t pin) {
  gpio_set_outover(pin, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(pin, GPIO_OVERRIDE_INVERT);
}

void usb3sun_gpio_set_as_output(uint8_t pin) {
  pinMode(pin, OUTPUT);
}

void usb3sun_gpio_set_as_input_pullup(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
}

void usb3sun_gpio_set_as_input_pulldown(uint8_t pin) {
  pinMode(pin, INPUT_PULLDOWN);
}

void usb3sun_i2c_set_pinout(uint8_t scl, uint8_t sda) {
  Wire.setSCL(scl);
  Wire.setSDA(sda);
}

void usb3sun_buzzer_start(uint32_t pitch) {
  analogWriteRange(100);
  analogWriteFreq(pitch);
  analogWrite(BUZZER_PIN, 50);
}

void usb3sun_buzzer_stop(void) {
  digitalWrite(BUZZER_PIN, false);
}

void usb3sun_display_init(void) {
  display.begin(SSD1306_SWITCHCAPVCC, /* SCREEN_ADDRESS */ 0x3C);
  display.setRotation(DISPLAY_ROTATION);
  display.cp437(true);
  display.setTextWrap(false);
  display.clearDisplay();
  // display.drawXBitmap(0, 0, splash_bits, 128, 32, SSD1306_WHITE);
  display.display();
}

void usb3sun_display_flush(void) {
  display.display();
}

void usb3sun_display_clear(void) {
  display.clearDisplay();
}

void usb3sun_display_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t border_radius, bool inverted, bool filled) {
  if (filled) {
    display.fillRoundRect(x, y, w, h, border_radius, inverted ? SSD1306_BLACK : SSD1306_WHITE);
  } else {
    display.drawRoundRect(x, y, w, h, border_radius, inverted ? SSD1306_BLACK : SSD1306_WHITE);
  }
}

#elifdef USB3SUN_HAL_LINUX_NATIVE

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct {
  size_t version = 1;
} pinout;

static uint64_t start_micros = usb3sun_micros();
static std::vector<Entry> history{};
static uint64_t history_filter;
static bool display_current[32][128]{};
static bool display_next[32][128]{};

static void draw_dot(int16_t x_, int16_t y_, bool inverted) {
  auto x = static_cast<size_t>(x_), y = static_cast<size_t>(y_);
  if (y < sizeof display_next / sizeof *display_next) {
    if (x < sizeof *display_next / sizeof **display_next) {
      display_next[y][x] = !inverted;
    }
  }
}

// <https://en.cppreference.com/w/cpp/container/vector/vector#Example>
std::ostream &operator<<(std::ostream &s, const std::vector<uint8_t> &v) {
  std::ofstream old{};
  old.copyfmt(s);
  if (v.size() == 0) {
    return s << "<>";
  }
  char separator = '<';
  for (const auto &e : v) {
    s << separator << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (unsigned)e;
    separator = ' ';
  }
  s << '>';
  s.copyfmt(old);
  return s;
}

std::ostream &operator<<(std::ostream &s, const Op &o) {
  std::visit([&s](const auto &o) { s << o; }, o);
  return s;
}

std::ostream &operator<<(std::ostream &s, const Entry &v) {
  return s << v.micros << " (@" << (v.micros - start_micros) << ") " << v.op;
}

static void push_history(Op op) {
  if (!(std::visit([](const auto &op) { return op.id; }, op) & history_filter)) {
    return;
  }
  uint64_t micros = usb3sun_micros();
  Entry entry{micros, op};
  history.push_back(entry);
}

void usb3sun_test_init(uint64_t history_filter_mask) {
  history_filter = history_filter_mask;
  usb3sun_mock_gpio_read(PINOUT_V2_PIN, true);
}

static bool mock_gpio_values[32]{};
void usb3sun_mock_gpio_read(usb3sun_pin pin, bool value) {
  mock_gpio_values[pin] = value;
}

static std::vector<uint8_t> mock_sunk_input{};
void usb3sun_mock_sunk_read(const char *data, size_t len) {
  for (size_t i = 0; i < len; i++)
    mock_sunk_input.push_back(data[i]);
}

bool usb3sun_mock_sunk_read_has_input(void) {
  return mock_sunk_input.size() > 0;
}

static uint16_t mock_uhid_vid = 0, mock_uhid_pid = 0;
static bool mock_uhid_vid_pid_result = false;
void usb3sun_mock_usb_vid_pid(bool result, uint16_t vid, uint16_t pid) {
  mock_uhid_vid = vid;
  mock_uhid_pid = pid;
  mock_uhid_vid_pid_result = result;
}

static std::vector<usb3sun_hid_report_info> mock_uhid_report_infos{};
void usb3sun_mock_uhid_parse_report_descriptor(const std::vector<usb3sun_hid_report_info> &infos) {
  mock_uhid_report_infos = infos;
}

static uint8_t mock_if_protocol = 0;
void usb3sun_mock_uhid_interface_protocol(uint8_t if_protocol) {
  mock_if_protocol = if_protocol;
}

static bool mock_uhid_request_report_result = false;
void usb3sun_mock_uhid_request_report_result(bool result) {
  mock_uhid_request_report_result = result;
}

static bool (*mock_fs_read)(const char *path, char *data, size_t data_len, size_t &actual_len) = nullptr;
void usb3sun_mock_fs_read(bool (*mock)(const char *path, char *data, size_t data_len, size_t &actual_len)) {
  mock_fs_read = mock;
}

static std::optional<int> mock_display_fd{};
void usb3sun_mock_display_output(int fd) {
  mock_display_fd = fd;
}

const std::vector<Entry> &usb3sun_test_get_history(void) {
  return history;
}

void usb3sun_test_clear_history(void) {
  history.clear();
}

static bool exit_on_reboot = false;
void usb3sun_test_exit_on_reboot(void) {
  exit_on_reboot = true;
}

void usb3sun_test_terminal_demo_mode(bool enabled) {
  struct termios termios;
  if (tcgetattr(0, &termios) == -1) {
    perror("tcgetattr");
  } else {
    if (enabled) {
      // turn off canonical mode, so we can read input immediately.
      termios.c_lflag &= ~ICANON;
      // turn off local echo, so we can echo input our own way.
      termios.c_lflag &= ~ECHO;
    } else {
      termios.c_lflag |= ICANON;
      termios.c_lflag |= ECHO;
    }
    if (tcsetattr(0, TCSAFLUSH, &termios) == -1) {
      perror("tcsetattr");
    }
  }
}

size_t usb3sun_pinout_version(void) {
  return pinout.version;
}

void usb3sun_pinout_v2(void) {
  push_history(PinoutV2Op {});
  pinout.version = 2;
}

void usb3sun_sunk_init(void) {
  push_history(SunkInitOp {});
}

int usb3sun_sunk_read(void) {
  push_history(SunkReadOp {});
  if (mock_sunk_input.size() > 0) {
    int result = *mock_sunk_input.begin();
    mock_sunk_input.erase(mock_sunk_input.begin());
    return result;
  }
  return -1;
}

size_t usb3sun_sunk_write(uint8_t *data, size_t len) {
  push_history(SunkWriteOp {{data, data+len}});
  return 0;
}

void usb3sun_sunm_init(uint32_t baud) {
  push_history(SunmInitOp {baud});
}

size_t usb3sun_sunm_write(uint8_t *data, size_t len) {
  push_history(SunmWriteOp {{data, data+len}});
  return 0;
}

void usb3sun_sunk_sniffer_init(void) {
  push_history(SunkSnifferInitOp {});
}

int usb3sun_sunk_sniff_tx(void) {
  push_history(SunkSniffTxOp {});
  return -1;
}

void usb3sun_usb_init(void) {}

void usb3sun_usb_task(void) {}

bool usb3sun_usb_vid_pid(uint8_t dev_addr, uint16_t *vid, uint16_t *pid) {
  (void) dev_addr;
  *vid = mock_uhid_vid;
  *pid = mock_uhid_pid;
  return mock_uhid_vid_pid_result;
}

bool usb3sun_uhid_request_report(uint8_t dev_addr, uint8_t instance) {
  push_history(UhidRequestReportOp {dev_addr, instance});
  return mock_uhid_request_report_result;
}

uint8_t usb3sun_uhid_interface_protocol(uint8_t dev_addr, uint8_t instance) {
  (void) dev_addr;
  (void) instance;
  return mock_if_protocol;
}

size_t usb3sun_uhid_parse_report_descriptor(usb3sun_hid_report_info *result, size_t result_len, const uint8_t *descriptor, size_t descriptor_len) {
  (void) descriptor;
  (void) descriptor_len;
  for (size_t i = 0; i < result_len && i < mock_uhid_report_infos.size(); i++) {
    result[i] = mock_uhid_report_infos[i];
  }
  return mock_uhid_report_infos.size();
}

bool usb3sun_uhid_set_led_report(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t &led_report) {
  // TODO: stub
  (void) dev_addr;
  (void) instance;
  (void) report_id;
  (void) led_report;
  return false;
}

void usb3sun_debug_init(int (*printf)(const char *format, ...)) {
  // TODO: stub
  (void) printf;
}

int usb3sun_debug_uart_read(void) {
  if (fcntl(0, F_SETFL, O_NONBLOCK) == 0) {
    uint8_t result;
    if (read(0, &result, sizeof result) == sizeof result) {
      return result;
    }
  }
  return -1;
}

int usb3sun_debug_cdc_read(void) {
  // TODO: same as debug uart read in this hal, is that ok?
  if (fcntl(0, F_SETFL, O_NONBLOCK) == 0) {
    uint8_t result;
    if (read(0, &result, sizeof result) == sizeof result) {
      return result;
    }
  }
  return -1;
}

bool usb3sun_debug_write(const char *data, size_t len) {
  bool ok = fwrite(data, 1, len, stdout) == len;
  fflush(stdout);
  return ok;
}

void usb3sun_allow_debug_over_cdc(void) {}

void usb3sun_allow_debug_over_uart(void) {}

bool usb3sun_fs_init(void) {
  return false;
}

bool usb3sun_fs_wipe(void) {
  return false;
}

bool usb3sun_fs_read(const char *path, char *data, size_t len) {
  if (!mock_fs_read) {
    push_history(FsReadOp {path, len, {}});
    return false;
  }
  size_t actual_len = 0xAAAAAAAAAAAAAAAA;
  if (!mock_fs_read(path, data, len, actual_len)) {
    push_history(FsReadOp {path, len, {}});
    return false;
  }
  push_history(FsReadOp {path, len, {{data, data + actual_len}}});
  return actual_len == len;
}

bool usb3sun_fs_write(const char *path, const char *data, size_t len) {
  push_history(FsWriteOp {path, {data, data + len}});
  return false;
}

void usb3sun_mutex_lock(usb3sun_mutex *mutex) {
  // TODO: stub
  (void) mutex;
}

void usb3sun_mutex_unlock(usb3sun_mutex *mutex) {
  // TODO: stub
  (void) mutex;
}

bool usb3sun_fifo_push(uint32_t value) {
  // TODO: stub
  (void) value;
  return false;
}

bool usb3sun_fifo_pop(uint32_t *result) {
  // TODO: stub
  (void) result;
  return false;
}

void usb3sun_reboot(void) {
  push_history(RebootOp {});
  if (exit_on_reboot) {
    usb3sun_test_terminal_demo_mode(false);
    exit(0);
  }
}

uint64_t usb3sun_micros(void) {
  static uint64_t result = 0;
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
    result = (uint64_t)ts.tv_sec * 1'000'000
      + (uint64_t)ts.tv_nsec / 1'000;
  }
  return result;
}

void usb3sun_sleep_micros(uint64_t micros) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
    ts.tv_nsec += micros * 1'000;
    while (ts.tv_nsec > 999'999'999) {
      ts.tv_sec += 1;
      ts.tv_nsec -= 1'000'000'000;
    }
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr) == EINTR);
  }
}

uint32_t usb3sun_clock_speed(void) {
  return 120'000'000;
}

void usb3sun_panic(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vfprintf(stdout, format, ap);
  va_end(ap);
  fflush(stdout);
  abort();
}

void usb3sun_alarm(uint32_t ms, void (*callback)(void)) {
  (void) callback;
  push_history(AlarmOp {ms});
}

bool usb3sun_gpio_read(usb3sun_pin pin) {
  bool value = mock_gpio_values[pin];
  push_history(GpioReadOp {pin, value});
  return value;
}

void usb3sun_gpio_write(usb3sun_pin pin, bool value) {
  push_history(GpioWriteOp {pin, value});
}

void usb3sun_gpio_set_as_inverted(usb3sun_pin pin) {
  // TODO: stub
  (void) pin;
}

void usb3sun_gpio_set_as_output(usb3sun_pin pin) {
  // TODO: stub
  (void) pin;
}

void usb3sun_gpio_set_as_input_pullup(usb3sun_pin pin) {
  // TODO: stub
  (void) pin;
}

void usb3sun_gpio_set_as_input_pulldown(usb3sun_pin pin) {
  // TODO: stub
  (void) pin;
}

void usb3sun_i2c_set_pinout(usb3sun_pin scl, usb3sun_pin sda) {
  // TODO: stub
  (void) scl;
  (void) sda;
}

void usb3sun_buzzer_start(uint32_t pitch) {
  push_history(BuzzerStartOp {pitch});
}

void usb3sun_display_init(void) {}

static bool mock_display_write(const char *text) {
  if (write(*mock_display_fd, text, strlen(text)) == -1) {
    perror("write");
    return false;
  }
  return true;
}

void usb3sun_display_flush(void) {
  if (!mock_display_fd.has_value()) {
    return;
  }
  static bool first = true;
  if (first) {
    if (!mock_display_write(/* CUP 1;1 ED 0 */ "\033[H\033[J")) return;
    first = false;
  }
  for (size_t y = 0; y < sizeof display_next / sizeof *display_next; y++) {
    for (size_t x = 0; x < sizeof *display_next / sizeof **display_next; x++) {
      if (display_next[y][x] != display_current[y][x]) {
        if (!mock_display_write(/* CUP 1;1 */ "\033[H")) return;
        if (!mock_display_write("╔")) return;
        for (size_t x = 0; x < sizeof *display_next / sizeof **display_next; x++) {
          if (!mock_display_write("═")) return;
        }
        if (!mock_display_write("╗")) return;
        if (!mock_display_write("\n")) return;
        for (size_t y = 0; y < sizeof display_next / sizeof *display_next; y += 2) {
          if (!mock_display_write("║")) return;
          for (size_t x = 0; x < sizeof *display_next / sizeof **display_next; x++) {
            constexpr const char *const output[] {" ", "▄", "▀", "█"};
            if (!mock_display_write(output[display_next[y][x] << 1 | display_next[y+1][x]])) return;
            display_current[y][x] = display_next[y][x];
            display_current[y+1][x] = display_next[y+1][x];
          }
          if (!mock_display_write("║\n")) return;
        }
        if (!mock_display_write("╚")) return;
        for (size_t x = 0; x < sizeof *display_next / sizeof **display_next; x++) {
          if (!mock_display_write("═")) return;
        }
        if (!mock_display_write("╝")) return;
        if (!mock_display_write("\n")) return;
        return;
      }
    }
  }
}

void usb3sun_display_clear(void) {
  for (size_t y = 0; y < sizeof display_next / sizeof *display_next; y++) {
    for (size_t x = 0; x < sizeof *display_next / sizeof **display_next; x++) {
      display_next[y][x] = false;
    }
  }
}

void usb3sun_display_rect(
    int16_t x0_, int16_t y0_, int16_t w_, int16_t h_,
    int16_t border_radius, bool inverted, bool filled) {
  // TODO border radius
  (void) border_radius;
  if (w_ <= 0 || h_ <= 0) return; // avoid underflow
  auto x0 = static_cast<size_t>(x0_), y0 = static_cast<size_t>(y0_);
  auto w = static_cast<size_t>(w_), h = static_cast<size_t>(h_);
  size_t y1 = y0 + h - 1, x1 = x0 + w - 1;
  for (size_t y = y0; y <= y1; y++) {
    for (size_t x = x0; x <= x1; x++) {
      if (y == y0 || y == y1 || x == x0 || x == x1 || filled) {
        draw_dot(x, y, inverted);
      }
    }
  }
}

#endif

void usb3sun_display_hline(int16_t x0, int16_t y, int16_t w, bool inverted, int16_t every) {
  size_t x1 = x0 + w - 1;
  if (every > 1) {
    draw_dot(x0, y, inverted);
    draw_dot(x1, y, inverted);
  } else if (every < 1) {
    every = 1;
  }
  for (size_t x = x0; x <= x1; x += every) {
    draw_dot(x, y, inverted);
  }
}

void usb3sun_display_vline(int16_t x, int16_t y0, int16_t h, bool inverted, int16_t every) {
  size_t y1 = y0 + h - 1;
  if (every > 1) {
    draw_dot(x, y0, inverted);
    draw_dot(x, y1, inverted);
  } else if (every < 1) {
    every = 1;
  }
  for (size_t y = y0; y <= y1; y += every) {
    draw_dot(x, y, inverted);
  }
}

void usb3sun_display_text(int16_t x0_, int16_t y0_, bool inverted, const char *text, bool opaque) {
  constexpr size_t advance = 6;
  constexpr size_t glyph_width = 5;
  constexpr size_t glyph_height = 8;
  auto x0 = static_cast<size_t>(x0_), y0 = static_cast<size_t>(y0_);
  for (; *text != '\0'; text += 1) {
    uint8_t i = *text;
    if (i != (uint8_t)'\xFF') {
      for (size_t x = x0; x < x0 + glyph_width; x++) {
        uint8_t line = adafruit_gfx_classic[i * glyph_width + (x - x0)];
        for (size_t y = y0; y < y0 + glyph_height; y++, line >>= 1) {
          if (!!(line & 1)) {
            draw_dot(x, y, inverted);
          } else if (opaque) {
            draw_dot(x, y, !inverted);
          }
        }
      }
    }
    x0 += advance;
  }
}
