#ifndef USB3SUN_STATE_H
#define USB3SUN_STATE_H

#include <cstdint>

struct State {
  bool bell;
  bool clickEnabled = true;
  bool caps = true;
  bool compose = true;
  bool scroll = true;
  bool num = true;
  uint8_t lastModifiers;
  uint8_t lastKeys[6];
  uint8_t lastButtons;
  bool inMenu = false;
  unsigned selectedMenuItem = 0u;
  unsigned topMenuItem = 0u;
};

extern State state;

#endif
