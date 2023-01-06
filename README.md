usb3sun
=======

usb3sun is a firmware for the RP2040 (Raspberry Pi Pico) that allows you to connect USB keyboards and mice to the Sun 8-pin mini-DIN interface.

see also:

* [USB2Sun](https://github.com/jgilje/USB2Sun) by @jgilje
* [sun2usb](https://github.com/jareklupinski/sun2usb) by @jareklupinski

instructions
------------

1. fix the -DCFG_TUSB_CONFIG_FILE in platformio.ini for your absolute path
2. apply tinyusb1.patch to <.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c> (version 1.16.0)
    * upstream patch: hathach/tinyusb#1786
3. apply tinyusb2.patch to <.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c> (version 1.16.0)

features
--------

* keyboard support
* mouse support
* power key support (experimental)
* LED indicators via SSD1306 (128x32)
* click and bell via passive piezo buzzer

bindings
--------

for more details, see [src/bindings.h](src/bindings.h), but here are the important ones:

| USB               | Sun                     |
|-------------------|-------------------------|
| context menu      | Compose                 |
| Left Alt          | Alt                     |
| Right Alt         | Graph/Alt               |
| Left GUI*         | left Meta (diamond)     |
| Right GUI*        | right Meta (diamond)    |
| Left Ctrl         | Control                 |
| Right Ctrl+Space  | (usb3sun settings menu) |
| Right Ctrl+Esc    | Front                   |
| Right Ctrl+Return | Line Feed               |
| Right Ctrl+F1     | Help                    |
| Right Ctrl+F4     | Props                   |
| Right Ctrl+=      | keypad =                |
| Right Ctrl+C      | Copy                    |
| Right Ctrl+F      | Find                    |
| Right Ctrl+O      | Front                   |
| Right Ctrl+P      | Power                   |
| Right Ctrl+V      | Paste                   |
| Right Ctrl+X      | Cut                     |
| Right Ctrl+Y      | Again                   |
| Right Ctrl+Z      | Undo                    |

\* aka Super, Mod4, Windows, etc

compatibility
-------------

- 04A5:8001 BenQ Zowie EC2 **mouse** — perfect
- 3367:1903 Endgame Gear XM1r **mouse** — buttons only (16-bit dx/dy, no boot protocol)
- 0461:4E24 HP KB71211 **keyboard** — almost perfect (lacks scroll lock or right meta)
- 0461:4E23 HP MOGIUO **mouse** — perfect
- 045E:0040 Microsoft Wheel **Mouse** Optical 1.1A — perfect
- 045E:0752 Microsoft Wired **Keyboard** 400 — perfect
- 045E:0750 Microsoft Wired **Keyboard** 600 — perfect
