#include "config.h"
#include "gpio.h"
#include "settings.h"

#include <Adafruit_TinyUSB.h>

// TODO add Print::vprintf in ArduinoCore-API Print.h
#if defined(DEBUG_LOGGING)
static int printfDebug(const char *format, ...) {
  char buf[256];
  int len;
  va_list ap;
  va_start(ap, format);
  len = vsnprintf(buf, sizeof(buf), format, ap);
#if defined(DEBUG_OVER_CDC)
  Serial.write(buf);
  Serial.flush();
#endif
#if defined(DEBUG_OVER_UART)
  DEBUG_UART.write(buf);
  DEBUG_UART.flush();
#endif
  va_end(ap);
  return len;
}
#endif

Pinout::Pinout() : sunmV2(SUN_MTX_V2, SerialPIO::NOPIN) {
#if defined(SUNK_ENABLE)
  sunkUart->setPinout(sunkTx, sunkRx);
#endif
#if defined(SUNM_ENABLE)
  sunmV1.setPinout(sunkTx, sunkRx);
#endif
#if defined(DEBUG_LOGGING)
#if defined(DEBUG_OVER_CDC)
  Serial.begin(115200);
  DEBUG_RP2040_PRINTF = printfDebug;
#endif
#endif
}

void Pinout::v2() {
  sunk = &SUNK_UART_V2;
  sunm = &sunmV2;
  sunkTx = SUN_KTX_V2;
  sunkRx = SUN_KRX_V2;
  sunmTx = SUN_MTX_V2;
  sunkUart = &SUNK_UART_V2;
#if defined(DEBUG_LOGGING)
#if defined(DEBUG_OVER_UART)
  DEBUG_UART.end();
  DEBUG_UART.setPinout(DEBUG_UART_TX, DEBUG_UART_RX);
  DEBUG_UART.setFIFOSize(4096);
  DEBUG_UART.begin(DEBUG_UART_BAUD, SERIAL_8N1);
  DEBUG_RP2040_PRINTF = printfDebug;
#if CFG_TUSB_DEBUG
  TinyUSB_Serial_Debug = &DEBUG_UART;
#endif
#endif
#endif
}

void Pinout::beginSun() {
#if defined(SUNK_ENABLE)
  sunk->end();
  sunkUart->setPinout(sunkTx, sunkRx);
  sunk->begin(1200, SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  gpio_set_outover(sunkTx, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(sunkRx, GPIO_OVERRIDE_INVERT);
#endif
#if defined(SUNM_ENABLE)
  sunm->end();
  switch (version) {
    case 1:
      sunmV1.setPinout(SUN_MTX_V1, SUN_MRX_V1);
      break;
    case 2:
      // do nothing
      break;
  }
  sunm->begin(settings.mouseBaudReal(), SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  gpio_set_outover(sunmTx, GPIO_OVERRIDE_INVERT);
#endif
}

void Pinout::restartSunm() {
#if defined(SUNM_ENABLE)
  sunm->end();
  sunm->begin(settings.mouseBaudReal(), SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  gpio_set_outover(sunmTx, GPIO_OVERRIDE_INVERT);
#endif
}
