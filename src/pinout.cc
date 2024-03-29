#include "config.h"
#include "pinout.h"
#include "settings.h"

#include <Arduino.h>
#include <Wire.h>
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

Pinout::Pinout() : sunmV2(SUN_MTX_V2, SerialPIO::NOPIN) {}

void Pinout::v1() {
#if defined(DEBUG_LOGGING)
#if defined(DEBUG_OVER_CDC)
  allowDebugOverCdc();
#endif
#if defined(DEBUG_OVER_UART) && !defined(SUNK_ENABLE)
  allowDebugOverUart();
#endif
#endif
}

void Pinout::v2() {
  version = 2;
  sunk = &SUNK_UART_V2;
  sunm = &sunmV2;
  sunkTx = SUN_KTX_V2;
  sunkRx = SUN_KRX_V2;
  sunmTx = SUN_MTX_V2;
  sunkUart = &SUNK_UART_V2;
#if defined(DEBUG_LOGGING)
#if defined(DEBUG_OVER_CDC)
  allowDebugOverCdc();
#endif
#if defined(DEBUG_OVER_UART)
  allowDebugOverUart();
#endif
#endif

  // set DISPLAY_ENABLE high to turn on the display via Q7.
  // some display modules need delay to start reliably. for example, i have one module with a C9 on
  // its pcb that needs no delay, but i have another without C9 that stays black every other reset
  // unless given 15 ms of delay. tested with Q7 = 2N7000, R18 = 4K7, resetting the pico in three
  // different patterns (reset/run ms): 50/200, 250/750, 3000/1000. let’s double that just in case.
  pinMode(DISPLAY_ENABLE, OUTPUT);
  digitalWrite(DISPLAY_ENABLE, HIGH);
  delay(30);
}

void Pinout::begin() {
  // pico led on, to be turned off at the end of setup()
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  analogWriteRange(100);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(POWER_KEY, OUTPUT);
  Wire.setSCL(DISPLAY_SCL);
  Wire.setSDA(DISPLAY_SDA);

#if defined(WAIT_PIN)
  pinMode(WAIT_PIN, INPUT_PULLUP);
#endif

  // check for pinout v2 (active high)
  pinMode(PINOUT_V2_PIN, INPUT_PULLDOWN);
  if (digitalRead(PINOUT_V2_PIN) == HIGH) {
    v2();
  } else {
    v1();
  }
}

void Pinout::beginSun() {
#if defined(SUNK_ENABLE)
  sunk->end();
  sunkUart->setPinout(sunkTx, sunkRx);
  sunk->begin(1200, SERIAL_8N1);
  // gpio invert must be set *after* setPinout/begin
  gpio_set_outover(sunkTx, GPIO_OVERRIDE_INVERT);
  gpio_set_inover(sunkRx, GPIO_OVERRIDE_INVERT);

  // break preventer: set KTX_ENABLE# low to connect sun keyboard tx.
  // the pin is high on reset and boot, which pulls INT_KTX low, which keeps the
  // KTX line connected and idle, preventing a break that would make the sun
  // machine drop you back to the ok prompt (and maybe kernel panic on resume).
  pinMode(KTX_ENABLE, OUTPUT);
  digitalWrite(KTX_ENABLE, LOW);
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

bool Pinout::debugWrite(const char *data, size_t len) {
  bool ok = true;
  if (debugCdc) {
    if (debugCdc->write(data, len) < len) {
      ok = false;
    } else {
      debugCdc->flush();
    }
  }
  if (debugUart) {
    if (debugUart->write(data, len) < len) {
      ok = false;
    } else {
      debugUart->flush();
    }
  }
  return ok;
}

bool Pinout::debugPrint(const char *text) {
  return debugWrite(text, strlen(text));
}

bool Pinout::debugPrintln() {
  return debugPrint("\n");
}

bool Pinout::debugPrintln(const char *text) {
  return debugPrintf("%s\n", text);
}

bool Pinout::debugPrintf(const char *format, ...) {
  if (!debugCdc && !debugUart) {
    return true;
  }
  va_list ap;
  char result[256];
  va_start(ap, format);
  int requiredLen = vsnprintf(result, sizeof result, format, ap);
  va_end(ap);
  if (requiredLen < 0) {
    return false;
  } else if (requiredLen > sizeof result - 1) {
    size_t len = requiredLen + 1;
    char *result = new char[len];
    if (!result) {
      return false;
    }
    va_start(ap, format);
    int actualLen = vsnprintf(result, len, format, ap);
    va_end(ap);
    bool ok = false;
    if (actualLen == requiredLen) {
      ok = debugWrite(result, actualLen);
    }
    delete[] result;
    return ok;
  }
  return debugWrite(result, requiredLen);
}

void Pinout::allowDebugOverCdc() {
  // needs to be done manually when using FreeRTOS and/or TinyUSB
  Serial.begin(115200);
  DEBUG_RP2040_PRINTF = printfDebug;
  debugCdc = &Serial;
}

void Pinout::allowDebugOverUart() {
  DEBUG_UART.end();
  DEBUG_UART.setPinout(DEBUG_UART_TX, DEBUG_UART_RX);
  DEBUG_UART.setFIFOSize(4096);
  DEBUG_UART.begin(DEBUG_UART_BAUD, SERIAL_8N1);
  DEBUG_RP2040_PRINTF = printfDebug;
#if CFG_TUSB_DEBUG
  TinyUSB_Serial_Debug = &DEBUG_UART;
#endif
  debugUart = &DEBUG_UART;
}
