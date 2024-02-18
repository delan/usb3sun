#ifndef USB3SUN_PINOUT_H
#define USB3SUN_PINOUT_H

#include "config.h"

#include <Arduino.h>
#include <SerialPIO.h>

struct Pinout {
  Pinout();
  void begin();
  void beginSun();
  void restartSunm();
  bool debugWrite(const char *data, size_t len);
  bool debugPrint(const char *text);
  bool debugPrintln();
  bool debugPrintln(const char *text);
  bool debugPrintf(const char *format, ...) __attribute__ ((format (printf, 2, 3)));

  int version = 1;
  HardwareSerial *sunk = &SUNK_UART_V1;
  HardwareSerial *sunm = &SUNM_UART_V1;
  Adafruit_USBD_CDC *debugCdc = nullptr;
  SerialUART *debugUart = nullptr;

private:
  pin_size_t sunkTx = SUN_KTX_V1;
  pin_size_t sunkRx = SUN_KRX_V1;
  pin_size_t sunmTx = SUN_MTX_V1;
  SerialUART *sunkUart = &SUNK_UART_V1;
  SerialUART &sunmV1 = SUNM_UART_V1;
  SerialPIO sunmV2;

  void v1();
  void v2();
  void allowDebugOverCdc();
  void allowDebugOverUart();
};

extern Pinout pinout;

#endif
