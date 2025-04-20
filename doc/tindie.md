SPARCstations have [a unique serial-like interface](https://deskthority.net/wiki/Sun_keyboard/mouse_interface) for keyboard and mouse input, using a single 8-pin mini-din port. **usb3sun** allows you to use an ordinary USB keyboard and mouse with your SPARCstation!

in addition to the adapter itself, you will also need a male-to-male 8-pin mini-din cable (available as an option or [sold separately](https://www.tindie.com/products/funnycomputer/sun-mini-din-cable-2-metres/)), and your own USB keyboard and mouse (see below for compatibility).

**caution:** these cables are often sold elsewhere as a “mini-din serial cable”, but you **must** check that each pin is connected to the same pin on the other end. some cables, like the apple 590-0552-A, swap pins 1/2 + 3/5 + 6/8, and such cables must not be used.

* [user guide](https://github.com/delan/usb3sun/blob/default/doc/manual.md)
* [release notes](https://github.com/delan/usb3sun/releases)
* [project writeups and other links](https://go.daz.cat/usb3sun)

### testimonials

[europa64](https://linktr.ee/europa64) says:

“Honestly this is such a game changer, it really makes Suns so much more accessible for people who don’t have a hoard of vintage kit or the money to buy a keyboard on eBay. Getting these old machines into the hands of people is honestly one of the best ways to ensure they get preserved and your device really helps with that <3”

### what’s new

**rev B0** allows for debug logging (and the new debug cli) **without disabling the sun keyboard interface**, and fixes some [minor bugs](https://github.com/delan/usb3sun/blob/B0/doc/manual.md#errata) that happen when resetting the adapter.

**rev B1** is the same as rev B0, but with the USB-A connector changed due to stock shortage.

new in **firmware 2.0**:

- support for **NeXTSTEP** and **Plan 9**, which require the mouse to run at 1200 baud
- configurable **mouse baud rate** — 9600 (default), 4800, 2400, or 1200 baud
- **debug cli** over UART_RX, allowing you to automate keyboard and mouse inputs

as of **firmware 1.5**, usb3sun can now [**reprogram your idprom**](https://funny.computer.daz.cat/sun/nvram-hostid-faq.txt) with just a few keystrokes, making it easier to boot your workstation with a dead nvram battery!

### hardware features

usb3sun includes an **audible buzzer** to emulate the bell and click functions that would normally be included with a Sun keyboard.

usb3sun also emulates the **power key** in two ways:

* **soft power key** (Right Ctrl+P)
    * sends the power key scancodes (30h/B0h)
    * can turn on your workstation (only when usb3sun is powered externally)
* **onboard power button** (below the display)
    * doesn’t send any scancodes
    * can turn on your workstation (even when not powered externally)

usb3sun has an **oled display** that shows:

* led indicators (caps, compose, scroll, num)
* visual buzzer (click, bell)
* the settings menu

usb3sun has an **onboard reset button** in case the firmware gets stuck, and a **0.1″ debug header** with pins that allow you to power the adapter externally for flashing or debugging.

### firmware features

usb3sun has **persistent settings** for:

* forcing click mode (no/off/on)
* click duration (5–100 ms)
* mouse baud rate (for NeXTSTEP and Plan 9)
* your hostid

### compatibility

usb3sun is compatible with any machine that would normally use a

* Sun Compact 1
* Sun Type 4
* Sun Type 5
* Sun Type 5c
* Sun Type 6 (non-usb)

usb3sun has been tested successfully with

- SPARCstation IPC (sun4c)
- SPARCstation IPX (sun4c)
- SPARCstation 2 (sun4c)
- SPARCstation 5 (sun4m)
- SPARCstation 20 (sun4m)
- Ultra 5 (sun4u)
- 1209:A1E5 Atreus (Technomancy version) **keyboard** with QMK
- 04A5:8001 BenQ Zowie EC2 **mouse**
- 046D:C063 Dell M-UAV-DEL8 **mouse**
- FEED:1307 gBoards Gergo **keyboard** with QMK
- 03F0:6941 HP 150 Wired **Keyboard** (HSA-A013K, 664R5AA)
- 0461:4E24 HP KB71211 **keyboard** — no scroll lock or right meta
- 0461:4E23 HP MOGIUO **mouse**
- 04D9:1503 Inland 208397 **keyboard**
- 30FA:2031 Keji KECK801UKB **keyboard**
- 17EF:608D Lenovo EMS-537A **mouse**
- 17EF:6019 Lenovo MSU1175 **mouse**
- 045E:0040 Microsoft Wheel **Mouse** Optical 1.1A — enumeration is unreliable
- 045E:0752 Microsoft Wired **Keyboard** 400
- 045E:0750 Microsoft Wired **Keyboard** 600
- 045E:0773 Microsoft Explorer Touch **Mouse** (model 1490)
- FEED:6061 Preonic OLKB-60 **keyboard** with QMK

usb3sun is not yet compatible with

- [#4](https://github.com/delan/usb3sun/issues/4) — 05AC:024F Apple Magic **Keyboard** with Numeric Keypad (model A1243)
- [#5](https://github.com/delan/usb3sun/issues/5) — 1209:2303 Atreus (Keyboardio version) **keyboard** with Kaleidoscope
- [#6](https://github.com/delan/usb3sun/issues/6) — 3367:1903 Endgame Gear XM1r **mouse** — buttons only (16-bit dx/dy, no boot protocol)
- [#7](https://github.com/delan/usb3sun/issues/7) — 045E:0039 Microsoft Intelli**Mouse** Optical 1.1A — broken (“Control FAILED”)

### bindings

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

### specs

|  |  |
| --- | --- |
| width | 36 mm |
| length | 98 mm |
| height | 23 mm |
| ports | 1x Sun mini-din |
|  | 2x USB-A |
|  | 1x USB-C (debug only) |
| buttons | power (for workstation) |
|  | reset (for adapter) |
| led indicators | VBUS ok |
|  | VSUN ok |
| debug header pinout | 1. GND |
|  | 2. UART_TX |
|  | 3. UART_RX |
|  | 4. SWDIO |
|  | 5. SWCLK |
|  | 6. VBUS |
