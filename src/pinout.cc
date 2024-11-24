#include "config.h"
#include "pinout.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "hal.h"
#include "settings.h"

// TODO add Print::vprintf in ArduinoCore-API Print.h
static int vprintfDebug(const char *format, va_list ap) {
  va_list ap1;
  va_copy(ap1, ap);
  char result[256];
  int requiredLen = vsnprintf(result, sizeof result, format, ap1);
  va_end(ap1);
  if (requiredLen < 0) {
    return false;
  } else if (static_cast<size_t>(requiredLen) > sizeof result - 1) {
    size_t len = static_cast<size_t>(requiredLen) + 1;
    char *result = new char[len];
    if (!result) {
      return false;
    }
    va_list ap2;
    va_copy(ap2, ap);
    int actualLen = vsnprintf(result, len, format, ap2);
    va_end(ap2);
    bool ok = false;
    if (actualLen == requiredLen) {
      ok = pinout.debugWrite(result, actualLen);
    }
    delete[] result;
    return ok;
  }
  return pinout.debugWrite(result, static_cast<size_t>(requiredLen));
}

static int printfDebug(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = vprintfDebug(format, ap);
  va_end(ap);
  return result;
}

Pinout::Pinout() {}

void Pinout::v1() {
#if defined(DEBUG_LOGGING)
#if defined(DEBUG_OVER_CDC)
  usb3sun_allow_debug_over_cdc();
#endif
#if defined(DEBUG_OVER_UART) && !defined(SUNK_ENABLE)
  usb3sun_allow_debug_over_uart();
#endif
  usb3sun_debug_init(printfDebug);
#endif
}

void Pinout::v2() {
  usb3sun_pinout_v2();
#if defined(DEBUG_LOGGING)
#if defined(DEBUG_OVER_CDC)
  usb3sun_allow_debug_over_cdc();
#endif
#if defined(DEBUG_OVER_UART)
  usb3sun_allow_debug_over_uart();
#endif
  usb3sun_debug_init(printfDebug);
#endif

  // set DISPLAY_ENABLE high to turn on the display via Q7.
  // some display modules need delay to start reliably. for example, i have one module with a C9 on
  // its pcb that needs no delay, but i have another without C9 that stays black every other reset
  // unless given 15 ms of delay. tested with Q7 = 2N7000, R18 = 4K7, resetting the pico in three
  // different patterns (reset/run ms): 50/200, 250/750, 3000/1000. let’s double that just in case.
  usb3sun_gpio_set_as_output(DISPLAY_ENABLE);
  usb3sun_gpio_write(DISPLAY_ENABLE, true);
  usb3sun_sleep_micros(30'000);
}

void Pinout::begin() {
  // pico led on, to be turned off at the end of setup()
  usb3sun_gpio_set_as_output(LED_PIN);
  usb3sun_gpio_write(LED_PIN, true);

  usb3sun_gpio_set_as_output(BUZZER_PIN);
  usb3sun_gpio_set_as_output(POWER_KEY);
  usb3sun_i2c_set_pinout(DISPLAY_SCL, DISPLAY_SDA);

#if defined(WAIT_PIN)
  usb3sun_gpio_set_as_input_pullup(WAIT_PIN);
#endif

  // check for pinout v2 (active high)
  usb3sun_gpio_set_as_input_pulldown(PINOUT_V2_PIN);
  if (usb3sun_gpio_read(PINOUT_V2_PIN)) {
    v2();
  } else {
    v1();
  }
}

void Pinout::beginSun() {
#if defined(SUNK_ENABLE)
  usb3sun_sunk_init();
#endif
#if defined(SUNK_SNIFFER_ENABLE)
  usb3sun_sunk_sniffer_init();
#endif
#if defined(SUNK_ENABLE) || defined(SUNK_SNIFFER_ENABLE)
  if (usb3sun_pinout_version() == 2) {
    // break preventer: set KTX_ENABLE# low to connect sun keyboard tx.
    // the pin is high on reset and boot, which pulls INT_KTX low, which keeps the
    // KTX line connected and idle, preventing a break that would make the sun
    // machine drop you back to the ok prompt (and maybe kernel panic on resume).
    usb3sun_gpio_set_as_output(KTX_ENABLE);
    usb3sun_gpio_write(KTX_ENABLE, false);
  }
#endif
#if defined(SUNM_ENABLE)
  usb3sun_sunm_init(settings.mouseBaudReal());
#endif
}

void Pinout::restartSunm() {
#if defined(SUNM_ENABLE)
  usb3sun_sunm_init(settings.mouseBaudReal());
#endif
}

bool Pinout::debugWrite(const char *data, size_t len) {
  return usb3sun_debug_write(data, len);
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
  va_list ap;
  va_start(ap, format);
  int result = vprintfDebug(format, ap);
  va_end(ap);
  return result;
}

void Pinout::allowDebugOverCdc() {
  usb3sun_allow_debug_over_cdc();
}

void Pinout::allowDebugOverUart() {
  usb3sun_allow_debug_over_uart();
}
