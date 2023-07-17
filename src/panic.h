#ifndef USB3SUN_PANIC_H
#define USB3SUN_PANIC_H

#include "config.h"

template<typename... Args>
void panic2(const char *fmt, Args... args) {
    Sprint("panic: ");
    Sprintf(fmt, args...);
    Sprintln();
    panic(fmt, args...);
}

#endif
