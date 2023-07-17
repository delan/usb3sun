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
  using PaintHandler = void (*)();
  using KeyHandler = void (*)(const UsbkChanges &);

  PaintHandler handlePaint;
  KeyHandler handleKey;

  static void push(View *);
  static void pop();
  static void paint();
  static void key(const UsbkChanges &);
};

#endif
