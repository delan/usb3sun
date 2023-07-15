#include "config.h"
#include "menu.h"

#include "buzzer.h"
#include "bindings.h"
#include "display.h"
#include "settings.h"
#include "state.h"

template<typename... Args>
static void drawMenuItem(size_t i, const char *fmt, Args... args);

using MenuItemPainter = void (*)(size_t i);
static const MenuItemPainter MENU_ITEM_PAINTERS[] = {
  [](size_t i) {
    drawMenuItem(i, "Go back");
  },
  [](size_t i) {
    drawMenuItem(i, "Force click: %s",
      settings.forceClick() == ForceClick::_::NO ? "no"
      : settings.forceClick() == ForceClick::_::OFF ? "off"
      : settings.forceClick() == ForceClick::_::ON ? "on"
      : "?");
  },
  [](size_t i) {
    drawMenuItem(i, "Click duration: %u ms", settings.clickDuration());
  },
};

static const size_t MENU_ITEM_COUNT = sizeof(MENU_ITEM_PAINTERS) / sizeof(MENU_ITEM_PAINTERS[0]);

template<typename... Args>
static void drawMenuItem(int16_t x, int16_t y, bool on, const char *fmt, Args... args) {
  if (on) {
    display.fillRect(x + 4, y, 120, 8, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(x + 8, y);
    display.printf(fmt, args...);
  } else {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(x + 8, y);
    display.printf(fmt, args...);
  }
}

template<typename... Args>
static void drawMenuItem(size_t i, const char *fmt, Args... args) {
  if (i >= state.topMenuItem && i <= state.topMenuItem + 2)
    drawMenuItem(0, 8 * (1 + i - state.topMenuItem), state.selectedMenuItem == i, fmt, args...);
}

void Menu::draw() {
  display.setCursor(0, 8);
  for (size_t i = 0; i < MENU_ITEM_COUNT; i++)
    MENU_ITEM_PAINTERS[i](i);
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
        if (state.selectedMenuItem < MENU_ITEM_COUNT - 1u)
          state.selectedMenuItem += 1u;
        if (state.selectedMenuItem - state.topMenuItem > 2u)
          state.topMenuItem += 1u;
        break;
      case USBK_UP:
        if (state.selectedMenuItem > 0u)
          state.selectedMenuItem -= 1u;
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
