#ifndef USB3SUN_MENU_H
#define USB3SUN_MENU_H

#include <cstddef>
#include <cstdint>

struct Menu {
  bool open = false;
  size_t selectedItem = 0u;
  size_t topItem = 0u;
  int16_t marqueeX = 0;
  unsigned marqueeTick = 0;

  void toggle();
  void draw();
  void key(uint8_t usbkSelector, bool make);
};

#endif
