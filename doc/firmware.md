[README](../README.md) >

firmware
========

## how to build the firmware

1. fix the -DCFG_TUSB_CONFIG_FILE in platformio.ini for your absolute path
2. apply [tinyusb1.patch](tinyusb1.patch) to <.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c> (version 1.16.0)
    * upstream patch: hathach/tinyusb#1786
3. apply [tinyusb2.patch](tinyusb2.patch) to <.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c> (version 1.16.0)

### windows users

if you encounter problems with paths being too long:

1. open an admin command prompt (Win+X, A)
2. `git config --system core.longpaths true`
3. whenever you get `[WinError 3]` or `[WinError 145]`, delete `%USERPROFILE%\.platformio\.cache\tmp`
