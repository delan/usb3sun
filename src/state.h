#ifndef USB3SUN_STATE_H
#define USB3SUN_STATE_H

#include <cstdint>

struct State {
  bool bell = false;
  bool clickEnabled = false;
  bool caps = false;
  bool compose = false;
  bool scroll = false;
  bool num = false;
  uint8_t lastModifiers;
  uint8_t lastKeys[6];
  uint8_t lastButtons;
  bool inMenu = false;
  unsigned selectedMenuItem = 0u;
  unsigned topMenuItem = 0u;
};

extern State state;

#endif
