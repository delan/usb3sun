#ifndef USB3SUN_BUZZER_H
#define USB3SUN_BUZZER_H

#include <atomic>

#include <CoreMutex.h>

struct Buzzer {
  static const unsigned long plugDuration = 125'000u;
  static const unsigned long unplugDuration = 125'000u;
  static const unsigned int bellPitch = 1'000'000u / 480u; // 480 us period
  static const unsigned int plugPitch = 261.6255653005986346778499935233; // C4
  static const unsigned int plugPitch2 = 391.99543598174929408569953045983; // G4

  typedef enum class State {
    NONE,
    BELL,
    CLICK,
    PLUG,
    PLUG2,
    UNPLUG,
    UNPLUG2,
  } _;

  State current;
  unsigned long since;

  void update();
  void click();
  void plug();
  void unplug();

private:
  bool isExpired(unsigned long t, unsigned long duration);
  void setCurrent(unsigned long t, State value);
  void update0();
};

extern Buzzer buzzer;
extern mutex_t buzzerMutex;

#endif
