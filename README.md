usb3sun
=======

usb3sun is an adapter that allows you to connect USB keyboards and mice to the Sun 8-pin mini-DIN interface.

* [buy now](https://www.tindie.com/products/funnycomputer/usb3sun/) on tindie
* read more about
    * [the breadboard prototype](https://cohost.org/delan/post/787278-usb3sun-a-usb-input)
    * [the development of rev A1](https://cohost.org/delan/post/1650431-usb3sun-rev-a1-a-us)
    * [the funny computer museum](https://funny.computer.daz.cat/)

see also:

* [USB2Sun](https://github.com/jgilje/USB2Sun) by @jgilje
* [sun2usb](https://github.com/jareklupinski/sun2usb) by @jareklupinski

**note:** this repo uses submodules! clone with `--recursive`, or if you forgot to do that:

```sh
$ git submodule update --init --recursive
```

documentation
-------------

* **[user guide](doc/manual.md)**
* hacking and assembly docs
    * [firmware](doc/firmware.md)
    * [breadboard prototype](doc/prototype.md)
    * [pcb](hardware/pcb) docs coming soon!
* maintainer docs
    * [how to release](doc/releasing.md)

hardware features
-----------------

usb3sun includes an **audible buzzer** to emulate the bell and click functions that would normally be included with a Sun keyboard.

usb3sun also emulates the **power key** in two ways:

* **soft power key** (Right Ctrl+P)
    * sends the power key scancodes (30h/B0h)
    * can turn on your workstation (only when usb3sun is powered externally)
* **onboard power button** (near the display)
    * doesn’t send any scancodes
    * can turn on your workstation (even when not powered externally)

usb3sun has an **oled display** that shows:

* keyboard led indicators (caps, compose, scroll, num)
* visual buzzer (click, bell)
* the settings menu

usb3sun has an **onboard reset button** in case the firmware gets stuck, and a **0.1″ debug header** (pins available as an option) that allows you to power the adapter externally for flashing or debugging.

firmware features
-----------------

usb3sun has **persistent settings** for:

* forcing click mode (no/off/on)
* click duration (5–100 ms)

features planned for a future firmware version:

* settings for bell/click tone frequency
* led indicators on your USB keyboard itself
* an idprom macro, to [reprogram your hostid](https://funny.computer.daz.cat/sun/nvram-hostid-faq.txt)

release notes
-------------

### firmware [1.5](https://github.com/delan/usb3sun/releases/tag/1.5) (2023-07-18)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/1.5/pio_pkg_list.txt) = dependency tree at time of release
* added a **macro to reprogram your idprom** — this makes it easier to boot your workstation with a dead nvram battery
* added a **macro to wipe your idprom** — this makes your idprom contents invalid, as if you had a dead nvram battery
* added a **hostid setting** — this sets the hostid used when reprogramming your idprom
* changed behaviour of builtin led to turn on at the very start of setup — this may help troubleshoot problems with CDC serial port initialisation

### firmware [1.4](https://github.com/delan/usb3sun/releases/tag/1.4) (2023-07-17)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/1.4/pio_pkg_list.txt) = dependency tree at time of release
* **fixed a serious bug** where opening the settings menu can make the adapter start releasing keys at the wrong times
* **fixed a serious bug** where special key bindings (Right Ctrl) can get stuck down if Right Ctrl is released early
* you can now press special key bindings (Right Ctrl) together with other modifiers (e.g. [Shift+Undo](https://docs.oracle.com/cd/E19683-01/806-7612/startup-tbl-5/index.html) for Redo)
* improved performance of keyboard processing — this reduces the latency of key presses by up to 90 µs

you should update your firmware if you have any of the following symptoms:

* modifiers like Shift stop working after closing the settings menu
* modifiers like Shift appear to have a delayed effect after closing the settings menu
* when a key gets stuck down, it remains stuck even after pressing and releasing another key

you can also work around these bugs by resetting the adapter.

### firmware [1.3](https://github.com/delan/usb3sun/releases/tag/1.3) (2023-07-16)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/1.3/pio_pkg_list.txt) = dependency tree at time of release
* added **firmware version to display** — this replaces the logo, which became a bit redundant
* added **marquee display for long menu items** — this fixes a bug where they would wrap in an unreadable way

### pcb rev [A2](https://github.com/delan/usb3sun/releases/tag/A2) (2023-06-30)

* added usb-c port J4 — this makes firmware updates much easier, and makes it possible to debug the sun keyboard and mouse interfaces simultaneously
* added polyfuse F3 after power supply switches — this protects the adapter under more fault conditions, in addition to faults in the downstream usb ports
* changed led resistors R12 and R13 from 200R to 1K — this reduces the luminous intensity from ~80% down to ~10% relative to 20mA
* changed tactile switches SW1 and SW2 from [TS-1187A-C-H-B](https://jlcpcb.com/partdetail/XkbConnectivity-TS_1187A_C_HB/C528025) to [TS-1187A-B-A-B](https://jlcpcb.com/partdetail/XkbConnectivity-TS_1187A_B_AB/C318884) — this reduces the extended parts fee with jlcpcb

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

* added display mechanical support — **committed later in [74b367a12600d](https://github.com/delan/usb3sun/commit/74b367a12600d712eb7a05a9fc67c958bae3fe62)**
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
