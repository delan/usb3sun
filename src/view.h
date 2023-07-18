#ifndef USB3SUN_VIEW_H
#define USB3SUN_VIEW_H

#include <cstddef>
#include <cstdint>

#include <Adafruit_TinyUSB.h>

struct DvChange {
  uint8_t usbkModifier;
  bool make;
};

struct SelChange {
  uint8_t usbkSelector;
  bool make;
};

struct UsbkChanges {
  hid_keyboard_report_t *kreport = nullptr;
  DvChange dv[8 * 2]{};
  SelChange sel[6 * 2]{};
  size_t dvLen = 0;
  size_t selLen = 0;
};

struct View {
  virtual void handlePaint() = 0;
  virtual void handleKey(const UsbkChanges &) = 0;

  static void push(View *);
  static void pop();
  static void paint();
  static void key(const UsbkChanges &);
};

#endif
