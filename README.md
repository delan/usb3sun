usb3sun
=======

usb3sun is an adapter that allows you to connect USB keyboards and mice to the Sun 8-pin mini-DIN interface.

* [buy now](https://www.tindie.com/products/funnycomputer/usb3sun/) on tindie
* read more about
    * [the breadboard prototype](https://cohost.org/delan/post/787278-usb3sun-a-usb-input)
    * [the development of rev A1](https://cohost.org/delan/post/1650431-usb3sun-rev-a1-a-us)
    * [the development of rev A2](https://cohost.org/delan/post/2042852-usb3sun-rev-a2-now)
    * [the development of rev A3](https://cohost.org/delan/post/2973101-usb3sun-rev-a3-guar)
    * [the development of firmware 2.0](https://cohost.org/delan/post/5806830-usb3sun-2-0-faster)
    * [the development of rev B0](https://cohost.org/delan/post/6029886-usb3sun-rev-b0-a-us)
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

what’s new
----------

**rev B0** allows for debug logging (and the new debug cli) **without disabling the sun keyboard interface**, and fixes some [minor bugs](https://github.com/delan/usb3sun/blob/B0/doc/manual.md#errata) that happen when resetting the adapter.

new in **firmware 2.0**:

- support for **NeXTSTEP** and **Plan 9**, which require the mouse to run at 1200 baud
- configurable **mouse baud rate** — 9600 (default), 4800, 2400, or 1200 baud
- **debug cli** over UART_RX, allowing you to automate keyboard and mouse inputs

as of **firmware 1.5**, usb3sun can now [**reprogram your idprom**](https://funny.computer.daz.cat/sun/nvram-hostid-faq.txt) with just a few keystrokes, making it easier to boot your workstation with a dead nvram battery!

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
* mouse baud rate (for NeXTSTEP and Plan 9)
* your hostid

features planned for a future firmware version:

* settings for bell/click tone frequency
* led indicators on your USB keyboard itself

release notes
-------------

### firmware ?.? (????-??-??)

* added experimental support for **leds on your usb keyboard** — led updates are not yet reliable, and currently has bugs that can cause usb devices to stop responding
* added experimental support for **analysing other sun keyboards** — this allows us to improve our firmware by observing the behaviour of real sun keyboards

### firmware 2.1 (2025-05-26)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/2.1/pio_pkg_list.txt) = dependency tree at time of release
* [#15](https://github.com/delan/usb3sun/issues/15) — fixed “The IDPROM contents are invalid” when reprogramming the idprom on some machines, including the Ultra5
* [#20](https://github.com/delan/usb3sun/issues/20) — fixed a bug where the display may not work correctly on a cold start (affects rev B0 units starting from B1#7)

### pcb rev B1 (2025-04-20)

* this board **requires firmware 2.1** or newer — firmware 2.0 is affected by [#20](https://github.com/delan/usb3sun/issues/20)
* changed J1 from [USB-U267-BWS](https://jlcpcb.com/partdetail/HanboElectronic-USB_U267BWS/C2962392) to the compatible [USB-269-BRW](https://jlcpcb.com/partdetail/Xunpu-USB_268BRY/C720548) due to stock shortage
* migrated from kicad 7 to kicad 8

### pcb rev B0 (2024-05-25)

* this board **requires firmware 2.0** or newer
* [#2](https://github.com/delan/usb3sun/issues/2) — fixed a limitation where the workstation **drops you into an ok prompt** when resetting the adapter
* [#11](https://github.com/delan/usb3sun/issues/11) — fixed a bug where the adapter can **hang until power cycled** after being reset, especially when reset frequently
* [#10](https://github.com/delan/usb3sun/issues/10) — added support for logging over UART_TX/UART_RX **without disabling the sun keyboard interface**
* reworked component layout and routing for **sun interfaces** and **led indicators** — none of the user-facing component locations have changed
* replaced transistors Q1 through Q4 (2N7000) with dual transistors Q1 and Q3 (2N7002DW)
* removed solder jumpers JP1 and JP2, which were added in rev A2 in case of errata

### firmware 2.0 (2024-05-02)

* [pio_pkg_list.txt](https://github.com/delan/usb3sun/blob/2.0/pio_pkg_list.txt) = dependency tree at time of release
* added support for adapters with **pcb rev B0** — these use pinout v2, while older revs use pinout v1
* added a **mouse baud setting** that can be set to 9600 baud (default), 4800, 2400, or 1200 baud
    * this adds support for **NeXTSTEP** and **Plan 9**, which require the mouse to run at a lower baud rate ([#8](https://github.com/delan/usb3sun/issues/8))
* you can now choose whether to **save or discard settings changes** when you close the menu
    * this change was made as part of a workaround for usb devices that malfunction when saving settings ([#14](https://github.com/delan/usb3sun/issues/14)); if you have any affected devices, hold **Shift** while pressing **Enter** to reboot the adapter after saving settings
* the displayed **version number** can now include **uncommitted and/or unreleased changes** (`2.0+` / `2.0+1` / `2.0+1+`)
* several other improvements to the user interface
    * you can now **close the menu by pressing Esc**
    * the menu now **automatically closes** after *Reprogram idprom* and *Wipe idprom (AAh)*
    * the menu now **shows four menu items**, rather than three menu items and the version number
    * *Click duration* now **beeps to preview your changes**, regardless of whether clicks are enabled
    * *Reprogram idprom* now **tells you what hostid is being programmed** into your idprom
* **updated our usb host stack**, making usb devices enumerate faster
    * Adafruit TinyUSB Library (2.0.1 → 3.1.0), Pico PIO USB (0.5.2 → 0.5.3)
    * this may affect [compatibility](https://github.com/delan/usb3sun/blob/default/doc/manual.md#compatibility) with your usb devices; please [report any regressions or improvements](https://github.com/delan/usb3sun/issues?q=is%3Aissue+is%3Aopen+label%3Acompatibility)
* added support for logging over UART_TX/UART_RX **without disabling the sun keyboard interface** ([#10](https://github.com/delan/usb3sun/issues/10))
    * this feature requires **pcb rev B0** or newer, due to the pinout changes required
    * please report any regressions with the **buzzer** or **sun mouse interface**; the buzzer had to be moved from pio to hardware pwm, and the sun mouse had to be moved from hardware uart to pio
* added a **debug cli** over UART_RX — this lets you press keys and move the mouse without a usb keyboard or mouse
* the firmware can now be **built as a normal program for linux**, thanks to a new hardware abstraction layer; as a result:
    * added an **interactive demo** (`run-demo.sh`) that can be used to play with or develop the user interface
    * added a **test suite** (`run-tests.sh`) for the setup and pinout routines, sun keyboard interface (reset sequence), buzzer (click and bell), settings (reads, writes, upgrades), and menu (confirm-save, hostid, i/o)
    * we can **catch some memory access violations** with dynamic analysis by **AddressSanitizer** (asan)
* added some **build tests** (`run-build-tests.sh`) to catch compile errors with custom build flags
* fixed some potential future compatibility issues in the settings formats (variable-width integer types and variable struct padding)
    * your existing settings in their v1 formats will be upgraded automatically
* fixed a compile error when debug logging was enabled (**PICOPROBE_ENABLE**, now known as **DEBUG_OVER_UART**)
* fixed all compile warnings under -Wall with the current toolchains for `pico` and `linux` (via nix-shell)
* fixed incorrect debug log output for **report id**, **usage**, and **usage page** when enumerating usb hid devices
* fixed missing newline in debug log output when reporting ErrorRollOver inputs from usb keyboards
* fixed missing newline in UHID_VERBOSE output when reporting inputs from non-boot-protocol usb devices
* removed the splash screen from debug logging — this significantly slowed down the setup routine
* removed the fake sun emulation feature — this wasn’t too useful, and was broken by the pinout changes
* several changes to config.h
    * added **DEBUG_OVER_CDC** to disable logging over usb cdc
    * added **UHID_LED_ENABLE** to opt into experimental usb hid led support
    * added **UHID_LED_TEST** to blink leds on all usb keyboards
    * added pin definitions **PINOUT_V2_PIN**, **KTX_ENABLE**, **DISPLAY_ENABLE**, **DEBUG_UART**, **SUNK_UART_V1**, **SUNK_UART_V2**, and **SUNM_UART_V1**
    * removed **SUNM_BAUD** in favour of the new menu setting
    * renamed **PICOPROBE_ENABLE** to **DEBUG_OVER_UART**
    * renamed **PICOPROBE_BAUD** to **DEBUG_UART_BAUD**
    * renamed **PICOPROBE_TX** to **DEBUG_UART_TX**
    * renamed **PICOPROBE_RX** to **DEBUG_UART_RX**
    * **SUN_KTX**, **SUN_KRX**, **SUN_MTX**, and **SUN_MRX** were split into pinout v1 and v2 variants
    * **DEBUG_OVER_UART** no longer disables logging over usb cdc (see **DEBUG_OVER_CDC**)
    * **SUNK_ENABLE** was moved to platformio.ini, and takes precedence over **DEBUG_OVER_UART** in pinout v1
    * **SUNM_ENABLE** was moved to platformio.ini

### pcb rev [A3](https://github.com/delan/usb3sun/releases/tag/A3) (2023-10-15)

* fixed the inconsistent and excessive brightness of the led indicators (D2, D3)
* fixed a bug where the VBUS indicator led (D2) can be dimly lit without VBUS being connected
* changed J1 from [UJ2-ADH-W1-TH](https://jlcpcb.com/partdetail/CuiDevices-UJ2_ADH_W1TH/C5201729) to the compatible [USB-U267-BWS](https://jlcpcb.com/partdetail/HanboElectronic-USB_U267BWS/C2962392) due to stock shortage
* cleaned up unused kicad libraries, and checked in libraries that were missing from the repo

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
