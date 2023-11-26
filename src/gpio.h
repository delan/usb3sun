#ifndef USB3SUN_GPIO_H
#define USB3SUN_GPIO_H

#include "config.h"

#include <Arduino.h>
#include <SerialPIO.h>

struct Pinout {
  Pinout();
  void v2();
  void beginSun();
  void restartSunm();

  int version = 1;
  HardwareSerial *sunk = &SUNK_UART_V1;
  HardwareSerial *sunm = &SUNM_UART_V1;

private:
  pin_size_t sunkTx = SUN_KTX_V1;
  pin_size_t sunkRx = SUN_KRX_V1;
  pin_size_t sunmTx = SUN_MTX_V1;
  SerialUART *sunkUart = &SUNK_UART_V1;
  SerialUART &sunmV1 = SUNM_UART_V1;
  SerialPIO sunmV2;
};

extern Pinout pinout;

#endif
