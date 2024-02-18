#ifndef USB3SUN_SUNK_H
#define USB3SUN_SUNK_H

#include "config.h"

#include <Arduino.h>

#include <cstddef>

#include "bindings.h"
#include "pinout.h"

// internal flags (not part of real keycode)
#define SUNK_SEND_SHIFT 0x100
#define SHIFT(code) (SUNK_SEND_SHIFT | (code))

const uint16_t ASCII_TO_SUNK[128] = {
    /* 00h */ 0, 0, 0, 0, 0, 0, 0, 0,
    /* 08h */ 0, 0, SUNK_RETURN, 0, 0, 0, 0, 0,
    /* 10h */ 0, 0, 0, 0, 0, 0, 0, 0,
    /* 18h */ 0, 0, 0, 0, 0, 0, 0, 0,
    /* 20h */ 0x79, SHIFT(0x1E), SHIFT(0x57), SHIFT(0x20), SHIFT(0x21), SHIFT(0x22), SHIFT(0x24), 0x57,
    /* 28h */ SHIFT(0x26), SHIFT(0x27), SHIFT(0x25), SHIFT(0x29), 0x6B, 0x28, 0x6C, 0x6D,
    /* 30h */ 0x27, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
    /* 38h */ 0x25, 0x26, SHIFT(0x56), 0x56, SHIFT(0x6B), 0x2A, SHIFT(0x6C), SHIFT(0x6D),
    /* 40h */ SHIFT(0x1F), SHIFT(0x4D), SHIFT(0x68), SHIFT(0x66), SHIFT(0x4F), SHIFT(0x38), SHIFT(0x50), SHIFT(0x51),
    /* 48h */ SHIFT(0x52), SHIFT(0x3D), SHIFT(0x53), SHIFT(0x54), SHIFT(0x55), SHIFT(0x6A), SHIFT(0x69), SHIFT(0x3E),
    /* 50h */ SHIFT(0x3F), SHIFT(0x36), SHIFT(0x39), SHIFT(0x4E), SHIFT(0x3A), SHIFT(0x3C), SHIFT(0x67), SHIFT(0x37),
    /* 58h */ SHIFT(0x65), SHIFT(0x3B), SHIFT(0x64), 0x40, 0x58, 0x41, SHIFT(0x23), SHIFT(0x28),
    /* 60h */ 0x2A, 0x4D, 0x68, 0x66, 0x4F, 0x38, 0x50, 0x51,
    /* 68h */ 0x52, 0x3D, 0x53, 0x54, 0x55, 0x6A, 0x69, 0x3E,
    /* 70h */ 0x3F, 0x36, 0x39, 0x4E, 0x3A, 0x3C, 0x67, 0x37,
    /* 78h */ 0x65, 0x3B, 0x64, SHIFT(0x40), SHIFT(0x58), SHIFT(0x41), SHIFT(0x2A), SUNK_BACKSPACE,
};

void sunkSend(bool make, uint8_t code);

template <typename... Args>
void sunkSend(const char *fmt, Args... args) {
  char result[256];
  size_t len = snprintf(result, sizeof(result) / sizeof(*result), fmt, args...);
  if (len >= sizeof(result) / sizeof(*result)) {
    Sprintf("sunk: macro buffer overflow");
    return;
  }
  for (auto i = 0; i < len; i++) {
    if (result[i] >= sizeof(ASCII_TO_SUNK) / sizeof(*ASCII_TO_SUNK) || ASCII_TO_SUNK[result[i]] == 0) {
      Sprintf("sunk: octet %02Xh not in ASCII_TO_SUNK\n", result[i]);
      return;
    }
  }
  Sprintf("sunk: sending macro <");
  Sprintf(fmt, args...);
  Sprintf(">\n");
  for (auto i = 0; i < len; i++) {
    if (!!(ASCII_TO_SUNK[result[i]] & SUNK_SEND_SHIFT))
      sunkSend(true, SUNK_SHIFT_L);
    sunkSend(true, ASCII_TO_SUNK[result[i]] & 0xFF);
    sunkSend(false, ASCII_TO_SUNK[result[i]] & 0xFF);
    if (!!(ASCII_TO_SUNK[result[i]] & SUNK_SEND_SHIFT))
      sunkSend(false, SUNK_SHIFT_L);
  }
}

#endif
