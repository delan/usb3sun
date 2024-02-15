#include "config.h"
#include "buzzer.h"

#include <Arduino.h>

#include "settings.h"
#include "state.h"

static int64_t alarm(alarm_id_t, void *) {
  buzzer.update();
  return 0; // don’t reschedule
}

void Buzzer::pwmTone(unsigned int pitch, std::optional<unsigned long> duration) {
  analogWriteRange(100);
  analogWriteFreq(pitch);
  analogWrite(BUZZER_PIN, 50);
  if (duration.has_value()) {
    auto unused = add_alarm_in_ms(*duration, alarm, nullptr, true);
  }
}

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
        setCurrent(t, Buzzer::_::PLUG2);
        pwmTone(plugPitch2, plugDuration);
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
        setCurrent(t, Buzzer::_::UNPLUG2);
        pwmTone(plugPitch, plugDuration);
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
    setCurrent(t, Buzzer::_::BELL);
    pwmTone(bellPitch);
  } else if (current != Buzzer::_::NONE) {
    setCurrent(t, Buzzer::_::NONE);
    digitalWrite(BUZZER_PIN, false);
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
    setCurrent(micros(), Buzzer::_::CLICK);
    pwmTone(1'000u, settings.clickDuration());
  }
}

void Buzzer::plug() {
  CoreMutex m{&buzzerMutex};
  if (current <= Buzzer::_::PLUG2) {
    setCurrent(micros(), Buzzer::_::PLUG);
    pwmTone(plugPitch, plugDuration);
  }
}

void Buzzer::unplug() {
  CoreMutex m{&buzzerMutex};
  if (current <= Buzzer::_::UNPLUG2) {
    setCurrent(micros(), Buzzer::_::UNPLUG);
    pwmTone(plugPitch2, plugDuration);
  }
}
