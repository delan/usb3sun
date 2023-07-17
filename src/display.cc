#include "config.h"
#include "display.h"

Adafruit_SSD1306 display{128, 32, &Wire, /* OLED_RESET */ -1};
