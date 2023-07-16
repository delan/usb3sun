[README](../README.md) >

how to release
==============

1. generate pio_pkg_list.txt
    1. `set PYTHONIOENCODING=utf-8`
    2. `%USERPROFILE%\.platformio\penv\Scripts\pio pkg list > pio_pkg_list.txt`
2. update USB3SUN_VERSION in platformio.ini
3. update README.md
4. commit and push
5. `git tag -am [tag] [tag]`
6. `git push --tags`
7. create release on github
