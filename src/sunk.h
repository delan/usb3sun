#ifndef USB3SUN_SUNK_H
#define USB3SUN_SUNK_H

#include "config.h"

#include <Arduino.h>

#include <cstddef>

#include "bindings.h"

// internal flags (not part of real keycode)
#define SUNK_SEND_SHIFT 0x100

const uint16_t ASCII_TO_SUNK[128] = {
    /* 00h */ 0, 0, 0, 0, 0, 0, 0, 0,
    /* 08h */ 0, 0, SUNK_RETURN, 0, 0, 0, 0, 0,
    /* 10h */ 0, 0, 0, 0, 0, 0, 0, 0,
    /* 18h */ 0, 0, 0, 0, 0, 0, 0, 0,
    /* 20h */ SUNK_SPACE, 0, 0, 0, 0, 0, 0, 0,
    /* 28h */ 0, 0, 0, 0, 0, SUNK_DASH, SUNK_DOT, 0,
    /* 30h */ SUNK_0, SUNK_1, SUNK_2, SUNK_3, SUNK_4, SUNK_5, SUNK_6, SUNK_7,
    /* 38h */ SUNK_8, SUNK_9, 0, 0, 0, 0, 0, 0,
    /* 40h */ SUNK_2 | SUNK_SEND_SHIFT, SUNK_A, SUNK_B, SUNK_C, SUNK_D, SUNK_E, SUNK_F, SUNK_G,
    /* 48h */ SUNK_H, SUNK_I, SUNK_J, SUNK_K, SUNK_L, SUNK_M, SUNK_N, SUNK_O,
    /* 50h */ SUNK_P, SUNK_Q, SUNK_R, SUNK_S, SUNK_T, SUNK_U, SUNK_V, SUNK_W,
    /* 58h */ SUNK_X, SUNK_Y, SUNK_Z, 0, 0, 0, 0, 0,
    /* 60h */ 0, SUNK_A, SUNK_B, SUNK_C, SUNK_D, SUNK_E, SUNK_F, SUNK_G,
    /* 68h */ SUNK_H, SUNK_I, SUNK_J, SUNK_K, SUNK_L, SUNK_M, SUNK_N, SUNK_O,
    /* 70h */ SUNK_P, SUNK_Q, SUNK_R, SUNK_S, SUNK_T, SUNK_U, SUNK_V, SUNK_W,
    /* 78h */ SUNK_X, SUNK_Y, SUNK_Z, 0, 0, 0, 0, 0,
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
