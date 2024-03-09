[README](../README.md) >

firmware
========

## how to build the firmware

1. `git apply picopiousb1.patch` ([picopiousb1.patch](../picopiousb1.patch), [#12](https://github.com/delan/usb3sun/issues/12))\
→ improves compatibility with Microsoft Wired Keyboard 600 (045E:0750)
2. `git apply tinyusb3.patch` ([tinyusb3.patch](../tinyusb3.patch), [#13](https://github.com/delan/usb3sun/issues/13))\
→ fixes a bug where dummy event callbacks shadow our own callbacks ([Adafruit_TinyUSB_Arduino#296](https://github.com/adafruit/Adafruit_TinyUSB_Arduino/issues/296))
3. `git apply debug1.patch` ([debug1.patch](../debug1.patch))\
→ makes TinyUSB debug logging configurable at runtime
4. `git -C ~/.platformio/packages/framework-arduinopico apply $PWD/debug2.patch` ([debug2.patch](../debug2.patch))\
→ makes arduino-pico debug logging configurable at runtime
5. `git -C ~/.platformio/packages/framework-arduinopico apply $PWD/debug3.patch` ([debug3.patch](../debug3.patch))\
→ makes TinyUSB debug logging configurable at runtime

### linux users

if you get “patch does not apply” errors:

```sh
$ dos2unix '.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c'
```

copy these commands to do all of the above:

```sh
dos2unix '.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c'
git apply tinyusb1.patch
git apply tinyusb2.patch
git apply debug1.patch
git -C ~/.platformio/packages/framework-arduinopico apply $PWD/debug2.patch
```

### windows users

if you encounter problems with paths being too long:

1. open an admin command prompt (Win+X, A)
2. `git config --system core.longpaths true`
3. whenever you get `[WinError 3]` or `[WinError 145]`, delete `%USERPROFILE%\.platformio\.cache\tmp`

if you get “patch does not apply” errors in step 2:

1. change the line endings from CRLF to LF in <.pio/libdeps/pico/Adafruit TinyUSB Library/src/host/usbh.c>

## how to get clangd (LSP) working

```sh
$ pio run -t compiledb
```
