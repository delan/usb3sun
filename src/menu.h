#ifndef USB3SUN_MENU_H
#define USB3SUN_MENU_H

#include <cstddef>
#include <cstdint>

#include "view.h"

struct MenuView : View {
  bool isOpen = false;
  size_t selectedItem = 0u;
  size_t topItem = 0u;
  int16_t marqueeX = 0;
  unsigned marqueeTick = 0;

  void handlePaint() override;
  void handleKey(const UsbkChanges &) override;
  void open();
  void close();
  void sel(uint8_t usbkSelector);
};

struct WaitView : View {
  bool isOpen = false;
  const char *message = "";

  void handlePaint() override;
  void handleKey(const UsbkChanges &) override;
  void open(const char *message);
  void close();
};

extern MenuView MENU_VIEW;

#endif
