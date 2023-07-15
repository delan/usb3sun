#ifndef USB3SUN_MENU_H
#define USB3SUN_MENU_H

#include <cstdint>

struct Menu {
  void draw();
  void key(uint8_t usbkSelector, bool make);
};

#endif
