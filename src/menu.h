#ifndef USB3SUN_MENU_H
#define USB3SUN_MENU_H

#include <cstdint>

struct Menu {
  int16_t marqueeX = 0;
  unsigned marqueeTick = 0;

  void draw();
  void key(uint8_t usbkSelector, bool make);
};

#endif
