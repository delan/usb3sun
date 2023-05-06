#ifndef USB3SUN_SETTINGS_H
#define USB3SUN_SETTINGS_H

#include <algorithm>

#include <CoreMutex.h>
#include <LittleFS.h>

#define SETTING(_name, _version, _type, _default) \
  struct _name##Setting { \
    static const unsigned currentVersion = _version; \
    static constexpr const char *const path = "/" #_name; \
    unsigned version = currentVersion; \
    _type value = _default; \
  } _name##_field; \
  _type &_name() { \
    return _name##_field.value; \
  }

#define SETTING_ENUM(_name, ...) \
struct _name { \
  typedef enum class State: int { __VA_ARGS__, VALUE_COUNT } _; \
  State current; \
  operator State&() { return current; } \
  State& operator++() { \
    current = static_cast<State>(std::max(0, std::min(static_cast<int>(_::VALUE_COUNT) - 1, static_cast<int>(current) + 1))); \
    return current; \
  } \
  State& operator--() { \
    current = static_cast<State>(std::max(0, std::min(static_cast<int>(_::VALUE_COUNT) - 1, static_cast<int>(current) - 1))); \
    return current; \
  } \
};

extern mutex_t settingsMutex;

SETTING_ENUM(ForceClick, NO, OFF, ON);
struct Settings {
  SETTING(clickDuration, 1, unsigned long, 5uL); // [0,100]
  SETTING(forceClick, 1, ForceClick, {ForceClick::_::NO});

  static void begin();
  void readAll();
  template <typename T> void read(T& setting);
  template <typename T> void write(const T& setting);
};

extern Settings settings;

template <typename T>
void Settings::read(T& setting) {
  CoreMutex m{&settingsMutex};
  T result{};
  if (File f = LittleFS.open(T::path, "r")) {
    Sprintf("settings: read %s: opened\n", T::path);
    if (f.readBytes(reinterpret_cast<char *>(&result), sizeof result) == sizeof result) {
      if (result.version == T::currentVersion) {
        setting = result;
        Sprintf("settings: read %s: loaded\n", T::path);
        f.close();
        Sprintf("settings: read %s: closed\n", T::path);
        return;
      } else {
        Sprintf("settings: read %s: wrong version\n", T::path);
        f.close();
      }
    } else {
        f.close();
    }
  }
  setting = T{};
  Sprintf("settings: read %s: reset\n", T::path);
}

template <typename T>
void Settings::write(const T& setting) {
  CoreMutex m{&settingsMutex};
  if (File f = LittleFS.open(T::path, "w")) {
    Sprintf("settings: write %s: opened\n", T::path);
    if (f.write(reinterpret_cast<const char *>(&setting), sizeof setting) == sizeof setting) {
      Sprintf("settings: write %s: written\n", T::path);
      f.close();
      Sprintf("settings: write %s: closed\n", T::path);
      return;
    }
  } else {
    Sprintf("settings: write %s: open failed\n", T::path);
  }
}

#endif
