#include "config.h"
#include "settings.h"

#include <LittleFS.h>

void Settings::begin() {
  LittleFSConfig cfg;
  cfg.setAutoFormat(true);
  LittleFS.setConfig(cfg);
  if (LittleFS.begin()) {
    Sprintln("settings: mounted");
  }
}

void Settings::readAll() {
  read(settings.clickDuration_field);
  read(settings.forceClick_field);
}
