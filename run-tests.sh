#!/bin/sh
set -eu

for ke in -DSUNK_ENABLE ''; do
for me in -DSUNM_ENABLE ''; do
    PLATFORMIO_BUILD_FLAGS="$ke $me" pio run -e linux
    .pio/build/linux/program ${1-all}
done
done

PLATFORMIO_BUILD_FLAGS="-DSUNK_SNIFFER_ENABLE" pio run -e linux
.pio/build/linux/program ${1-all}
