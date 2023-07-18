#include "sunk.h"

#include <cstdint>

#include <Arduino.h>

#include "bindings.h"
#include "buzzer.h"

void sunkSend(bool make, uint8_t code) {
  static int activeCount = 0;
  if (make) {
    activeCount += 1;
    buzzer.click();
  } else {
    activeCount -= 1;
    code |= SUNK_BREAK_BIT;
  }

#ifdef SUNK_ENABLE
#ifdef SUNK_VERBOSE
  Sprintf("sun keyboard: tx command %02Xh\n", code);
#endif
  Serial1.write(code);
#endif

  if (activeCount <= 0) {
    activeCount = 0;
#ifdef SUNK_ENABLE
#ifdef SUNK_VERBOSE
  Sprintf("sun keyboard: idle\n");
#endif
  Serial1.write(SUNK_IDLE);
#endif
  }

  switch (code) {
    case SUNK_POWER:
      Sprintf("sun power: high\n");
      digitalWrite(POWER_KEY, HIGH);
      break;
    case SUNK_POWER | SUNK_BREAK_BIT:
      Sprintf("sun power: low\n");
      digitalWrite(POWER_KEY, LOW);
      break;
  }
}
