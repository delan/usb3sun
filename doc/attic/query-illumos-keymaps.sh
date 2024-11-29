#!/usr/bin/env zsh
# usage: query-illumos-keymaps.sh <path/to/illumos-gate> <type4|type5|other>
# illumos-gate: git clone https://github.com/illumos/illumos-gate.git
set -euo pipefail -o bsdecho
script_dir=${0:a:h}
illumos_gate_dir=$1; shift
layouts=$1; shift

cd -- "$illumos_gate_dir/usr/src/cmd/loadkeys/type_4"
case "$layouts" in
(type4)
    set -- reset us germany swiss_german belgium_france uk swiss_french netherlands sweden_finland denmark norway italy spain_latin_america portugal canada
    ;;
(type5)
    set -- reset *_5
    ;;
(other)
    set -- reset
    for layout in *; do
        case "$layout" in
        (reset|us|germany|swiss_german|belgium_france|uk|swiss_french|netherlands|sweden_finland|denmark|norway|italy|spain_latin_america|portugal|canada|*_5|Makefile) ;;
        (*) set -- "$@" "$layout" ;;
        esac
    done
    ;;
(*)
    >&2 echo "second argument must be <type4|type5|other>"
    exit 1
    ;;
esac
for i in {0..127}; do
    printf '%d (%02Xh)\n' $i $i
    (
        cd -- "$script_dir/../../src"
        rg --with-filename '^  [{].+, 0x'$(printf \%02X $i)', 0x[^}]+[}],' bindings.h | cat || if [ $? -eq 2 ]; then exit 2; fi
    )
    # per solaris keytables(4), “Note that the single character may be any character in an 8-bit
    # character set, such as ISO 8859/1.”, and all of the languages below seem to be best covered
    # by ISO 8859-1 (Latin-1)
    rg --sort=path '^key\s+'$i'\s+' "$@" | iconv -f ISO8859-1 -t UTF-8
    echo
done
