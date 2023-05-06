#include "config.h"
#include "buzzer.h"

#include <Arduino.h>

#include "state.h"

void Buzzer::update0() {
  const auto t = micros();

  // starting tone is not entirely idempotent, so avoid restarting it.
  // spamming it every 10 ms will just pop and then silence in practice.
  switch (current) {
    case _::NONE:
      break;
    case _::BELL:
      if (state.bell) {
        return;
      }
      break;
    case _::CLICK:
      if (!isExpired(t, state.clickDuration * 1'000uL)) {
        return;
      }
      break;
    case _::PLUG:
      if (!isExpired(t, plugDuration)) {
        return;
      } else {
        tone(BUZZER_PIN, plugPitch2, plugDuration);
        setCurrent(t, Buzzer::_::PLUG2);
        return;
      }
      break;
    case _::PLUG2:
      if (!isExpired(t, plugDuration)) {
        return;
      }
      break;
    case _::UNPLUG:
      if (!isExpired(t, plugDuration)) {
        return;
      } else {
        tone(BUZZER_PIN, plugPitch, plugDuration);
        setCurrent(t, Buzzer::_::UNPLUG2);
        return;
      }
      break;
    case _::UNPLUG2:
      if (!isExpired(t, plugDuration)) {
        return;
      }
      break;
  }
  if (state.bell) {
    tone(BUZZER_PIN, bellPitch);
    setCurrent(t, Buzzer::_::BELL);
  } else if (current != Buzzer::_::NONE) {
    noTone(BUZZER_PIN);
    setCurrent(t, Buzzer::_::NONE);
  }
}

bool Buzzer::isExpired(unsigned long t, unsigned long duration) {
  return t - since >= duration || since < duration;
}

void Buzzer::setCurrent(unsigned long t, Buzzer::State value) {
  Sprintf("buzzer: setCurrent %d\n", value);
  current = value;
  since = t;
}

void Buzzer::update() {
  CoreMutex m{&buzzerMutex};
  update0();
}

void Buzzer::click() {
  CoreMutex m{&buzzerMutex};
  if (!state.clickEnabled)
    return;

  if (current <= Buzzer::_::CLICK) {
    // violation of sparc keyboard spec :) but distinguishable from bell!
    tone(BUZZER_PIN, 1'000u, state.clickDuration);
    setCurrent(micros(), Buzzer::_::CLICK);
  }
}

void Buzzer::plug() {
  CoreMutex m{&buzzerMutex};
  if (current <= Buzzer::_::PLUG2) {
    tone(BUZZER_PIN, plugPitch, plugDuration);
    setCurrent(micros(), Buzzer::_::PLUG);
  }
}

void Buzzer::unplug() {
  CoreMutex m{&buzzerMutex};
  if (current <= Buzzer::_::UNPLUG2) {
    tone(BUZZER_PIN, plugPitch2, plugDuration);
    setCurrent(micros(), Buzzer::_::UNPLUG);
  }
}
