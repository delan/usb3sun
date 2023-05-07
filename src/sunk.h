#include <cstdint>

#define SUNK_SHIFT_L 0x63
#define SUNK_SPACE 0x79
#define SUNK_RETURN 0x59
#define SUNK_DASH 0x28
#define SUNK_DOT 0x6C
#define SUNK_1 0x1E
#define SUNK_2 0x1F
#define SUNK_3 0x20
#define SUNK_4 0x21
#define SUNK_5 0x22
#define SUNK_6 0x23
#define SUNK_7 0x24
#define SUNK_8 0x25
#define SUNK_9 0x26
#define SUNK_0 0x27
#define SUNK_Q 0x36
#define SUNK_W 0x37
#define SUNK_E 0x38
#define SUNK_R 0x39
#define SUNK_T 0x3A
#define SUNK_Y 0x3B
#define SUNK_U 0x3C
#define SUNK_I 0x3D
#define SUNK_O 0x3E
#define SUNK_P 0x3F
#define SUNK_A 0x4D
#define SUNK_S 0x4E
#define SUNK_D 0x4F
#define SUNK_F 0x50
#define SUNK_G 0x51
#define SUNK_H 0x52
#define SUNK_J 0x53
#define SUNK_K 0x54
#define SUNK_L 0x55
#define SUNK_Z 0x64
#define SUNK_X 0x65
#define SUNK_C 0x66
#define SUNK_V 0x67
#define SUNK_B 0x68
#define SUNK_N 0x69
#define SUNK_M 0x6A

// internal flags (not part of real keycode)
#define SUNK_SEND_SHIFT 0x100

const uint16_t SUNK_ASCII[128] = {
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
