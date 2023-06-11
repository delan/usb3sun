[README](../README.md) >

how to release
==============

1. generate pio_pkg_list.txt
    1. `set PYTHONIOENCODING=utf-8`
    2. `%USERPROFILE%\.platformio\penv\Scripts\pio pkg list > pio_pkg_list.txt`
2. update README.md
3. commit and push
4. `git tag -am [tag] [tag]`
5. `git push --tags`
6. create release on github
