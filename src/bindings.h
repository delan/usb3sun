#ifndef USB3SUN_BINDINGS_H
#define USB3SUN_BINDINGS_H

#include <cstdint>

#define USBK_RESERVED 0
#define USBK_ERROR_ROLLOVER 1
#define USBK_FIRST_KEYCODE 4
#define USBK_NUMLOCK 0x53
#define USBK_RIGHT 0x4F
#define USBK_LEFT 0x50
#define USBK_DOWN 0x51
#define USBK_UP 0x52
#define USBK_RETURN 0x28
#define USBK_ENTER 0x58
#define USBK_SPACE 0x2C
#define USBK_ESCAPE 0x29
#define USBK_1 0x1E
#define USBK_2 0x1F
#define USBK_3 0x20
#define USBK_4 0x21
#define USBK_5 0x22
#define USBK_6 0x23
#define USBK_7 0x24
#define USBK_8 0x25
#define USBK_9 0x26
#define USBK_0 0x27
#define USBK_A 0x04
#define USBK_B 0x05
#define USBK_C 0x06
#define USBK_D 0x07
#define USBK_E 0x08
#define USBK_F 0x09
#define USBK_CTRL_R ((uint8_t) (1u << 4))
#define USBM_LEFT ((uint8_t) (1u << 0))
#define USBM_RIGHT ((uint8_t) (1u << 1))
#define USBM_MIDDLE ((uint8_t) (1u << 2))
#define SUNM_LEFT ((uint8_t) (1u << 2))
#define SUNM_CENTER ((uint8_t) (1u << 1))
#define SUNM_RIGHT ((uint8_t) (1u << 0))
#define SUNK_RESET 0x01
#define SUNK_BELL_ON 0x02
#define SUNK_BELL_OFF 0x03
#define SUNK_CLICK_ON 0x0A
#define SUNK_CLICK_OFF 0x0B
#define SUNK_LED 0x0E
#define SUNK_LAYOUT 0x0F
#define SUNK_IDLE 0x7F
#define SUNK_LAYOUT_RESPONSE 0xFE
#define SUNK_RESET_RESPONSE 0xFF
#define SUNK_BREAK_BIT 0x80
#define SUNK_NUMLOCK 0x62
#define SUNK_CAPSLOCK 0x77
#define SUNK_SCROLLLOCK 0x17
#define SUNK_POWER 0x30
#define SUNK_SHIFT_L 0x63
#define SUNK_RETURN 0x59
#define SUNK_BACKSPACE 0x2B
#define SUNK_STOP 0x01

// sources:
// SPARC Keyboard Specification Version1 https://sparc.org/wp-content/uploads/2014/01/KBD.pdf.gz
// HID Usage Tables 1.3 https://usb.org/sites/default/files/hut1_3_0.pdf
// illumos usr/src/cmd/loadkeys/type_4/reset https://github.com/illumos/illumos-gate/blob/8b4261085e0d677be9a3253ff6b4c290e402576d/usr/src/cmd/loadkeys/type_4/reset
// illumos usr/src/uts/common/io/keytables.c https://github.com/illumos/illumos-gate/blob/3f3c90a958c5abf8ec0ed5d1fad2e40bd9905a50/usr/src/uts/common/io/keytables.c
// illumos usr/src/uts/common/io/kbtrans/usb_keytables.c https://github.com/illumos/illumos-gate/blob/8b4261085e0d677be9a3253ff6b4c290e402576d/usr/src/uts/common/io/kbtrans/usb_keytables.c
// illumos usr/src/uts/common/sys/kbd.h https://github.com/illumos/illumos-gate/blob/3f3c90a958c5abf8ec0ed5d1fad2e40bd9905a50/usr/src/uts/common/sys/kbd.h
// Table of special SUN keys and their various key codes https://kentie.net/article/sunkbd/sun%20keys.txt
// Linux kernel Sun serial MOUSE auto baud rate detection https://github.com/torvalds/linux/blob/3ecc37918c80ffdbfa8f08d3e75a0a9fca1c1979/drivers/tty/serial/suncore.c#L170-L232

// see also:
// Sun type 4 keyboard https://twitter.com/MiodVallat/status/1246128759641645060
// Meta key https://en.wikipedia.org/wiki/Meta_key
// Model M keyboard https://en.wikipedia.org/wiki/Model_M_keyboard
// System request https://en.wikipedia.org/wiki/System_request
// USB keyboard scancodes https://www.win.tue.nl/~aeb/linux/kbd/scancodes-14.html
// Sun Ray Client Hot Keys https://docs.oracle.com/cd/E25749_01/E25745/html/Clients-Hot-Keys.html
// Pause/Break key on modern keyboards https://superuser.com/q/12525
// What are/were the keys on a Sun keyboard for? https://unix.stackexchange.com/q/45686
// Sun Keyboard on a PC? https://linuxgazette.net/140/misc/lg/sun_keyboard_on_a_pc.html
// Sun keyboard https://everything2.com/title/Sun+keyboard
// Sun Type 4 https://deskthority.net/viewtopic.php?t=10793
// Sun keyboard to USB converter https://kentie.net/article/sunkbd/index.htm
// PC-Sun keyboard mapping in Rose multi-platform KVM switches http://www.rose-electronics.de/additional/sun_keyboard_mapping.pdf

struct UsbkToSunk {
  uint8_t dv[256]{};
  uint8_t sel[256]{};
  uint8_t special[256]{};

  UsbkToSunk();
};

extern UsbkToSunk USBK_TO_SUNK;

struct DvBinding {
  uint8_t usbkModifier;
  uint8_t sunkMake;
  uint8_t sunkBreak;
};

struct SelBinding {
  uint8_t usbkSelector;
  uint8_t sunkMake;
  uint8_t sunkBreak;
};

struct DvSelBinding {
  uint8_t usbkModifier;
  uint8_t usbkSelector;
  uint8_t sunkMake;
  uint8_t sunkBreak;
};

static const DvBinding DV_BINDINGS[] = {
  // direct equivalents
  {1u << 1, 0x63, 0xE3}, // 81. left “Shift”
  {1u << 5, 0x6E, 0xEE}, // 92. right “Shift”

  // HID Usage Tables 1.3 §10:
  // Keyboard Left GUI[33] Windowing environment key, examples are Microsoft Left Win key, Mac Left Apple key, Sun Left Meta key
  // Keyboard Right GUI[34] Windowing environment key, examples are Microsoft®RIGHT WIN key, Macintosh®RIGHT APPLE key, Sun®RIGHT META key.
  {1u << 3, 0x78, 0xF8}, // 101. left meta aka “(L Triangle)”
  {1u << 7, 0x7A, 0xFA}, // 102. right meta aka “(R triangle)”

  // by popular convention (source: dude trust me)
  {1u << 2, 0x13, 0x93}, // 100. Alt
  {1u << 6, 0x0D, 0x8D}, // 105. Graph	Alt

  // author’s preference
  {1u << 0, 0x4C, 0xCC}, // CtrlL → 63. Control
  // {1u << 4, none, none}, // CtrlR → usb3sun settings and DV_SEL_BINDINGS
};

const SelBinding SEL_BINDINGS[] = {
  // direct equivalents commonly found on 104-key layouts
  {58, 0x05, 0x85}, // 1. F1
  {59, 0x06, 0x86}, // 2. F2
  {60, 0x08, 0x88}, // 3. F3
  {61, 0x0A, 0x8A}, // 4. F4
  {62, 0x0C, 0x8C}, // 5. F5
  {63, 0x0E, 0x8E}, // 6. F6
  {64, 0x10, 0x90}, // 7. F7
  {65, 0x11, 0x91}, // 8. F8
  {66, 0x12, 0x92}, // 9. F9
  {67, 0x07, 0x87}, // 10. F10
  {68, 0x09, 0x89}, // 11. F11
  {69, 0x0B, 0x8B}, // 12. F12
  {49, 0x58, 0xD8}, // Keyboard \ and | = 13. \	|
  {100, 0x58, 0xD8}, // Keyboard Non-US \ and | = 13. \	|
  {76, 0x42, 0xC2}, // 14. Delete
  {83, 0x62, 0xE2}, // 20. Num Lock
  {41, 0x1D, 0x9D}, // 23. Esc
  {30, 0x1E, 0x9E}, // 24. 1	!
  {31, 0x1F, 0x9F}, // 25. 2	@
  {32, 0x20, 0xA0}, // 26. 3	#
  {33, 0x21, 0xA1}, // 27. 4	$
  {34, 0x22, 0xA2}, // 28. 5	%
  {35, 0x23, 0xA3}, // 29. 6	^
  {36, 0x24, 0xA4}, // 30. 7	&
  {37, 0x25, 0xA5}, // 31. 8	*
  {38, 0x26, 0xA6}, // 32. 9	(
  {39, 0x27, 0xA7}, // 33. 0	)
  {45, 0x28, 0xA8}, // 34. -	_
  {46, 0x29, 0xA9}, // 35. =	+
  {42, 0x2B, 0xAB}, // 36. Backspace
  {84, 0x2E, 0xAE}, // 38. /
  {85, 0x2F, 0xAF}, // 39. *
  {86, 0x47, 0xC7}, // 40. -
  {43, 0x35, 0xB5}, // 43. Tab
  {20, 0x36, 0xB6}, // 44. Q
  {26, 0x37, 0xB7}, // 45. W
  {8, 0x38, 0xB8}, // 46. E
  {21, 0x39, 0xB9}, // 47. R
  {23, 0x3A, 0xBA}, // 48. T
  {28, 0x3B, 0xBB}, // 49. Y
  {24, 0x3C, 0xBC}, // 50. U
  {12, 0x3D, 0xBD}, // 51. I
  {18, 0x3E, 0xBE}, // 52. O
  {19, 0x3F, 0xEF}, // 53. P
  {47, 0x40, 0xC0}, // 54. [	{
  {48, 0x41, 0xC1}, // 55. ]	}
  {40, 0x59, 0xD9}, // Keyboard Return (ENTER) (*not* “Keyboard Return”) → 56. Return
  {95, 0x44, 0xC4}, // 57. Home	7
  {96, 0x45, 0xC5}, // 58. (up cur)	8
  {97, 0x46, 0xC6}, // 59. PgUp	9
  {87, 0x7D, 0xFD}, // 60. +
  {4, 0x4D, 0xCD}, // 64. A
  {22, 0x4E, 0xCE}, // 65. S
  {7, 0x4F, 0xCF}, // 66. D
  {9, 0x50, 0xD0}, // 67. F
  {10, 0x51, 0xD1}, // 68. G
  {11, 0x52, 0xD2}, // 69. H
  {13, 0x53, 0xD3}, // 70. J
  {14, 0x54, 0xD4}, // 71. K
  {15, 0x55, 0xD5}, // 72. L
  {51, 0x56, 0xD6}, // 73. ;	:
  {52, 0x57, 0xD7}, // 74. '	"
  {53, 0x2A, 0xAA}, // 75. `	~
  {92, 0x5B, 0xDB}, // 76. (Left Cur)	4
  {93, 0x5C, 0xDC}, // 77. 5
  {94, 0x5D, 0xDD}, // 78. (Right Cur)	6
  {29, 0x64, 0xE4}, // 82. Z
  {27, 0x65, 0xE5}, // 83. X
  {6, 0x66, 0xE6}, // 84. C
  {25, 0x67, 0xE7}, // 85. V
  {5, 0x68, 0xE8}, // 86. B
  {17, 0x69, 0xE9}, // 87. N
  {16, 0x6A, 0xEA}, // 88. M
  {54, 0x6B, 0xEB}, // 89. ,	<
  {55, 0x6C, 0xEC}, // 90. .	>
  {56, 0x6D, 0xED}, // 91. /	?
  {89, 0x70, 0xF0}, // 94. End	1
  {90, 0x71, 0xF1}, // 95. (Dn Cur)	2
  {91, 0x72, 0xF2}, // 96. PgDn	3
  {88, 0x5A, 0xDA}, // Keypad ENTER → 97. Enter
  {57, 0x77, 0xF7}, // 99. Caps Lock
  {44, 0x79, 0xF9}, // 102. (Space Bar)
  {98, 0x5E, 0xDE}, // 106. Ins	0
  {99, 0x32, 0xB2}, // 107. Del	.

  // direct equivalents rarely found on 104-key layouts
  {120, 0x01, 0x81}, // Keyboard Stop → 15. Stop
  {121, 0x03, 0x83}, // Keyboard Again → 16. Again
  {163, 0x19, 0x99}, // Keyboard CrSel/Props → 21. Props
  {122, 0x1A, 0x9A}, // Keyboard Undo → 22. Undo
  {103, 0x2D, 0xAD}, // Keypad Equal Sign → 37. =
  {124, 0x33, 0xB3}, // Keyboard Copy → 42. Copy
  {158, 0x59, 0xD9}, // Keyboard Return (*not* “Keyboard Return (ENTER)”) → 56. Return
  {125, 0x49, 0xC9}, // Keyboard Paste → 62. Paste
  {126, 0x5F, 0xDF}, // Keyboard Find → 79. Find
  {123, 0x61, 0xE1}, // Keyboard Cut → 80. Cut
  {117, 0x76, 0xF6}, // Keyboard Help → 98. Help
  {102, 0x30, 0xB0}, // keyboard Power → bf(13) Power

  // near equivalents with slightly different legends
  {72, 0x15, 0x95}, // Pause/Break(!) aka “Keyboard Pause” → 17. Pause
  {70, 0x16, 0x96}, // PrintScreen/SysRq aka “Keyboard PrintScreen” → 70. Pr Sc
  {71, 0x17, 0x97}, // Keyboard Scroll Lock → 71. Break(!)	Scroll Lock

  // HID Usage Tables 1.3 §10:
  // Keyboard Application[11] Windows key for Windows 95, and Compose.
  // Keyboard Left GUI[11] Windows key for Windows 95, and Compose.
  // Keyboard Right GUI[11] Windows key for Windows 95, and Compose.
  {101, 0x43, 0xC3}, // context menu aka “Keyboard Application” → 101. Compose
};

const DvSelBinding DV_SEL_BINDINGS[] = {
  // no equivalent USB HID code; by analogy with Windows Alt+Esc
  {1u << 4, 41, 0x31, 0xB1}, // CtrlR+Esc → 41. Front

  // no equivalent USB HID code; by analogy with Windows Ctrl+O
  {1u << 4, 18, 0x48, 0xC8}, // CtrlR+O → 61. Open

  // no equivalent USB HID code; by analogy with the return key often typing a line feed
  {1u << 4, 40, 0x6F, 0xEF}, // CtrlR + Keyboard Return (ENTER) → 93. Line Feed

  // alternate for common 104-key layouts; by analogy with Windows Ctrl+Y for Redo/Repeat
  {1u << 4, 28, 0x03, 0x83}, // CtrlR+Y → 16. Again

  // alternate for common 104-key layouts; by analogy with “[full] stop”
  {1u << 4, 55, 0x01, 0x81}, // CtrlR+. → 15. Stop

  // alternate for common 104-key layouts; by analogy with Visual Studio F4 for Properties
  // (Alt+Enter is a more common convention on Windows, but it would be more awkward here)
  {1u << 4, 61, 0x19, 0x99}, // CtrlR+F4 → 21. Props

  // alternate for common 104-key layouts; by analogy with Windows Ctrl+Z
  {1u << 4, 29, 0x1A, 0x9A}, // CtrlR+Z → 22. Undo

  // alternate for common 104-key layouts; because they both type an equals sign
  {1u << 4, 46, 0x2D, 0xAD}, // “Keypad = and +” → 37. =

  // alternate for common 104-key layouts; by analogy with Windows Ctrl+C
  {1u << 4, 6, 0x33, 0xB3}, // CtrlR+C → 42. Copy

  // alternate for common 104-key layouts; by analogy with Windows Ctrl+V
  {1u << 4, 25, 0x49, 0xC9}, // CtrlR+V → 62. Paste

  // alternate for common 104-key layouts; by analogy with Windows Ctrl+F
  {1u << 4, 9, 0x5F, 0xDF}, // CtrlR+F → 79. Find

  // alternate for common 104-key layouts; by analogy with Windows Ctrl+X
  {1u << 4, 27, 0x61, 0xE1}, // CtrlR+X → 80. Cut

  // alternate for common 104-key layouts; by analogy with Windows F1
  {1u << 4, 58, 0x76, 0xF6}, // CtrlR+F1 → 98. Help

  // alternate for common 104-key layouts; P for Power
  {1u << 4, 19, 0x30, 0xB0}, // CtrlR+P → bf(13) Power
};

#endif
