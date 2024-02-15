#include "config.h"
#include "buzzer.h"

#include <Arduino.h>

#include "settings.h"
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
      if (!isExpired(t, settings.clickDuration() * 1'000uL)) {
        return;
      }
      break;
    case _::PLUG:
      if (!isExpired(t, plugDuration)) {
        return;
      } else {
        analogWriteRange(100);
        analogWriteFreq(plugPitch2);
        analogWrite(BUZZER_PIN, 50);
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
        analogWriteRange(100);
        analogWriteFreq(plugPitch);
        analogWrite(BUZZER_PIN, 50);
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
    analogWriteRange(100);
    analogWriteFreq(bellPitch);
    analogWrite(BUZZER_PIN, 50);
    setCurrent(t, Buzzer::_::BELL);
  } else if (current != Buzzer::_::NONE) {
    digitalWrite(BUZZER_PIN, false);
    setCurrent(t, Buzzer::_::NONE);
  }
}

bool Buzzer::isExpired(unsigned long t, unsigned long duration) {
  return t - since >= duration || since < duration;
}

void Buzzer::setCurrent(unsigned long t, Buzzer::State value) {
#ifdef BUZZER_VERBOSE
  Sprintf("buzzer: setCurrent %d\n", value);
#endif
  current = value;
  since = t;
}

void Buzzer::update() {
  CoreMutex m{&buzzerMutex};
  update0();
}

void Buzzer::click() {
  CoreMutex m{&buzzerMutex};
  switch (settings.forceClick()) {
    case ForceClick::_::NO:
      if (!state.clickEnabled) return;
      break;
    case ForceClick::_::OFF:
      return;
    case ForceClick::_::ON:
      break;
  }

  if (current <= Buzzer::_::CLICK) {
    // violation of sparc keyboard spec :) but distinguishable from bell!
    analogWriteRange(100);
    analogWriteFreq(1'000u);
    analogWrite(BUZZER_PIN, 50);
    setCurrent(micros(), Buzzer::_::CLICK);
  }
}

void Buzzer::plug() {
  CoreMutex m{&buzzerMutex};
  if (current <= Buzzer::_::PLUG2) {
    analogWriteRange(100);
    analogWriteFreq(plugPitch);
    analogWrite(BUZZER_PIN, 50);
    setCurrent(micros(), Buzzer::_::PLUG);
  }
}

void Buzzer::unplug() {
  CoreMutex m{&buzzerMutex};
  if (current <= Buzzer::_::UNPLUG2) {
    analogWriteRange(100);
    analogWriteFreq(plugPitch2);
    analogWrite(BUZZER_PIN, 50);
    setCurrent(micros(), Buzzer::_::UNPLUG);
  }
}
