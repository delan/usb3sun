# current status:
# - picocom works
# - vscode extension works
# - debugging in vscode works
# - build (pio run) works
# - upload (pio run -t upload) works
# - running openocd manually works
# - picotool upload protocol works
# - cmsis-dap upload protocol works
# advice that didn’t work:
# - https://nixos.wiki/wiki/Platformio#Use_in_vscode
# - https://github.com/platformio/platformio-vscode-ide/issues/3739
# advice that did work:
# - https://github.com/NixOS/nixpkgs/pull/237313#issuecomment-1586885212
# more context:
# - https://discourse.nixos.org/t/how-to-use-platformio-with-vscode/15805
# - https://community.platformio.org/t/non-standard-platformio-core-settings-ignored-by-vscode-extension/34265
{ pkgs ? import (builtins.fetchTarball {
    # NixOS/nixpkgs#237313 = ppenguin:refactor-platformio-fix-ide
    url = "https://github.com/NixOS/nixpkgs/archive/3592b10a67b518700002f1577e301d73905704fe.tar.gz";
  }) {},
}:
let
  envname = "platformio-fhs";
  mypython = pkgs.python3.withPackages (ps: with ps; [ platformio ]);
in
(pkgs.buildFHSEnv {
  name = envname;

  targetPkgs = pkgs: (with pkgs; [
    # picocom -fn -b115200 --imap lfcrlf /dev/ttyACM0
    picocom

    # for pio cli and vscode extension
    platformio-core
    mypython

    # for running openocd manually
    openocd

    # for running openocd via pio cli or vscode extension
    libusb1 # libusb-1.0.so.0
    hidapi # libhidapi-hidraw.so.0

    # for platformio debugging in vscode
    # (ldd ~/.platformio/packages/toolchain-rp2040-earlephilhower/bin/arm-none-eabi-gdb)
    ncurses5 # libtinfo.so.5
    mpfr # libmpfr.so.6
  ]);

  # clear LD_LIBRARY_PATH (NixOS/nixpkgs#263201, NixOS/nixpkgs#262775, NixOS/nixpkgs#262080)
  # then add path for platformio debugging in vscode
  # (ldd ~/.platformio/packages/toolchain-rp2040-earlephilhower/bin/arm-none-eabi-gdb)
  runScript = "env LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [ pkgs.ncurses5 ]} bash";
}).env
