usb3sun
=======

usb3sun is an adapter that allows you to connect USB keyboards and mice to the Sun 8-pin mini-DIN interface.

project writeup: <https://cohost.org/delan/post/787278-usb3sun-a-usb-input>

see also:

* [USB2Sun](https://github.com/jgilje/USB2Sun) by @jgilje
* [sun2usb](https://github.com/jareklupinski/sun2usb) by @jareklupinski

features
--------

* keyboard support
* mouse support
* soft power key support
* LED indicators via SSD1306 (128x32)
* click and bell via passive piezo buzzer

documentation
-------------

* [firmware](doc/firmware.md)
* [breadboard prototype](doc/prototype.md)
* [pcb](hardware/pcb) coming soon!
* maintainer docs
    * [how to release](doc/releasing.md)

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
| Right Ctrl+.      | Stop                    |
| Right Ctrl+Esc    | Front                   |
| Right Ctrl+Return | Line Feed               |
| Right Ctrl+C      | Copy                    |
| Right Ctrl+F      | Find                    |
| Right Ctrl+O      | Open                    |
| Right Ctrl+P      | Power                   |
| Right Ctrl+V      | Paste                   |
| Right Ctrl+X      | Cut                     |
| Right Ctrl+Y      | Again                   |
| Right Ctrl+Z      | Undo                    |
| Right Ctrl+F1     | Help                    |
| Right Ctrl+F4     | Props                   |
| Right Ctrl+=      | keypad =                |

\* aka Super, Mod4, Windows, etc

compatibility
-------------

- 04A5:8001 BenQ Zowie EC2 **mouse** — perfect
- 046D:C063 Dell M-UAV-DEL8 **mouse** — perfect
- 3367:1903 Endgame Gear XM1r **mouse** — buttons only (16-bit dx/dy, no boot protocol)
- 0461:4E24 HP KB71211 **keyboard** — almost perfect (lacks scroll lock or right meta)
- 0461:4E23 HP MOGIUO **mouse** — perfect
- 17EF:608D Lenovo EMS-537A **mouse** — perfect
- 045E:0039 Microsoft Intelli**Mouse** Optical 1.1A — broken (“Control FAILED”)
- 045E:0040 Microsoft Wheel **Mouse** Optical 1.1A — perfect
- 045E:0752 Microsoft Wired **Keyboard** 400 — perfect
- 045E:0750 Microsoft Wired **Keyboard** 600 — perfect
- 045E:0773 Microsoft Explorer Touch **Mouse** (model 1490) — perfect

release notes
-------------

### firmware [1.2](https://github.com/delan/usb3sun/releases/tag/1.2) (2023-06-11)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/1.2/pio_pkg_list.txt) = dependency tree at time of release
* added **audible usb plug/unplug notifications**
* added **persistence for settings** — including click duration!
* added **“force click” setting** — this replaces the old disable/enable click option in the settings menu, which toggled the same state as the host machine, with an independent override
* changed the initial click mode state to off
* changed the initial led indicator states to off
* fixed theoretical(?) race condition in buzzer management
* fixed errors in the README bindings table

### pcb rev [A1](https://github.com/delan/usb3sun/releases/tag/A1) (2023-05-10)

* added tactile switches for reset and hard power key
* added header pins for debug logging over UART0
* added mounting holes for display mechanical support
* added LED indicators for VBUS and VSUN
* improved enclosure compatibility by moving mini-din beyond bottom edge
* improved ground plane islands by stitching vias
* made all corner mounting holes padded on both sides — previously this was only the case for the hole connected to GNDPWR, but not the other three

### firmware [1.1](https://github.com/delan/usb3sun/releases/tag/1.1) (2023-05-06)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/1.1/pio_pkg_list.txt) = dependency tree at time of release
* fixed regressions due to dependency updates
    * fixed panic regression with *Pico Pio USB* 0.5.2 by removing non-idempotent workaround for a bug that was fixed upstream (sekigon-gonnoc/Pico-PIO-USB#77)
    * worked around unsolved usb hid regression (adafruit/Adafruit_TinyUSB_Arduino#296) by pinning *Adafruit TinyUSB Library* to 2.0.1
* fixed panic when combining PICOPROBE_ENABLE and CFG_TINYUSB_DEBUG
* changed default DISPLAY_ROTATION to 0 — this better suits pcb rev A0 than the old value, and is equally suitable for the breadboard prototype
* changed debug/upload protocols to cmsis-dap — this fixes compatibility with new picoprobe firmware
* pinned sensitive dependencies to reduce future breakage — note that not all dependencies are pinned yet (platformio/platformio-core#4613), but see above for known-good dependency tree

### pcb rev [A0](https://github.com/delan/usb3sun/releases/tag/A0) (2023-04-28)

* initial release

### firmware [1.0](https://github.com/delan/usb3sun/releases/tag/1.0) (2023-01-08)

* initial release
