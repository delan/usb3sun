#include "config.h"
#include "menu.h"

#include "buzzer.h"
#include "bindings.h"
#include "display.h"
#include "settings.h"
#include "state.h"

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, const char *fmt, Args... args);

using MenuItemPainter = void (*)(int16_t &marqueeX, size_t i);
static const MenuItemPainter MENU_ITEM_PAINTERS[] = {
  [](int16_t &marqueeX, size_t i) {
    drawMenuItem(marqueeX, i, "Go back");
  },
  [](int16_t &marqueeX, size_t i) {
    drawMenuItem(marqueeX, i, "Force click: %s",
      settings.forceClick() == ForceClick::_::NO ? "no"
      : settings.forceClick() == ForceClick::_::OFF ? "off"
      : settings.forceClick() == ForceClick::_::ON ? "on"
      : "?");
  },
  [](int16_t &marqueeX, size_t i) {
    drawMenuItem(marqueeX, i, "Click duration: %u ms", settings.clickDuration());
  },
};

static const size_t MENU_ITEM_COUNT = sizeof(MENU_ITEM_PAINTERS) / sizeof(MENU_ITEM_PAINTERS[0]);

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, int16_t y, bool on, const char *fmt, Args... args) {
  if (on) {
    int width = snprintf(NULL, 0, fmt, args...) * 6;
    display.fillRect(4, y, 120, 8, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    if (width > 112) {
      display.setCursor(8 - marqueeX, y);
      display.printf(fmt, args...);
      display.setCursor(8 - marqueeX + width + 112 / 2, y);
      display.printf(fmt, args...);
      marqueeX %= width + 112 / 2;
    } else {
      display.setCursor(8, y);
      display.printf(fmt, args...);
    }
    display.fillRect(0, y, 4, 8, SSD1306_BLACK);
    display.fillRect(124, y, 4, 8, SSD1306_BLACK);
  } else {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(8, y);
    display.printf(fmt, args...);
    display.fillRect(0, y, 8, 8, SSD1306_BLACK);
    display.fillRect(120, y, 8, 8, SSD1306_BLACK);
  }
}

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, const char *fmt, Args... args) {
  if (i >= state.topMenuItem && i <= state.topMenuItem + 2)
    drawMenuItem(marqueeX, 8 * (1 + i - state.topMenuItem), state.selectedMenuItem == i, fmt, args...);
}

void Menu::draw() {
  marqueeTick = (marqueeTick + 1) % 2;
  if (marqueeTick == 0)
    marqueeX += 1;

  for (size_t i = 0; i < MENU_ITEM_COUNT; i++)
    MENU_ITEM_PAINTERS[i](marqueeX, i);
}

void Menu::key(uint8_t usbkSelector, bool make) {
  if (make) {
    switch (usbkSelector) {
      case USBK_RIGHT:
        switch (state.selectedMenuItem) {
          case 1:
            ++settings.forceClick();
            settings.write(settings.forceClick_field);
            break;
          case 2:
            if (settings.clickDuration() < 96u) {
              settings.clickDuration() += 5u;
              settings.write(settings.clickDuration_field);
              buzzer.click();
            }
            break;
        }
        break;
      case USBK_LEFT:
        switch (state.selectedMenuItem) {
          case 1:
            --settings.forceClick();
            settings.write(settings.forceClick_field);
            break;
          case 2:
            if (settings.clickDuration() > 4u) {
              settings.clickDuration() -= 5u;
              settings.write(settings.clickDuration_field);
              buzzer.click();
            }
            break;
        }
        break;
      case USBK_DOWN:
        if (state.selectedMenuItem < MENU_ITEM_COUNT - 1u) {
          state.selectedMenuItem += 1u;
          marqueeX = 0;
        }
        if (state.selectedMenuItem - state.topMenuItem > 2u)
          state.topMenuItem += 1u;
        break;
      case USBK_UP:
        if (state.selectedMenuItem > 0u) {
          state.selectedMenuItem -= 1u;
          marqueeX = 0;
        }
        if (state.selectedMenuItem < state.topMenuItem)
          state.topMenuItem -= 1u;
        break;
      case USBK_RETURN:
      case USBK_ENTER:
        switch (state.selectedMenuItem) {
          // case N:
          //   state.foo = !state.foo;
          //   break;
        }
        state.inMenu = false;
        break;
    }
  }
}
