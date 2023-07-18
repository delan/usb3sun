#include "config.h"
#include "menu.h"

#include "buzzer.h"
#include "bindings.h"
#include "display.h"
#include "settings.h"
#include "state.h"
#include "view.h"

MenuView MENU_VIEW{};

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, bool on, const char *fmt, Args... args);

using MenuItemPainter = void (*)(int16_t &marqueeX, size_t i, bool on);
static const MenuItemPainter MENU_ITEM_PAINTERS[] = {
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Go back");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Force click: %s",
      settings.forceClick() == ForceClick::_::NO ? "no"
      : settings.forceClick() == ForceClick::_::OFF ? "off"
      : settings.forceClick() == ForceClick::_::ON ? "on"
      : "?");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Click duration: %u ms", settings.clickDuration());
  },
};

static const size_t MENU_ITEM_COUNT = sizeof(MENU_ITEM_PAINTERS) / sizeof(MENU_ITEM_PAINTERS[0]);

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, bool on, const char *fmt, Args... args) {
  int16_t y = 8 * (1 + i);
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

void MenuView::open() {
  if (isOpen)
    return;
  isOpen = true;
  selectedItem = 0u;
  topItem = 0u;
  View::push(&MENU_VIEW);
}

void MenuView::close() {
  if (!isOpen)
    return;
  View::pop();
  isOpen = false;
}

void MenuView::handlePaint() {
  marqueeTick = (marqueeTick + 1) % 2;
  if (marqueeTick == 0)
    marqueeX += 1;

  for (size_t i = topItem; i <= topItem + 2 && i < MENU_ITEM_COUNT; i++)
    MENU_ITEM_PAINTERS[i](marqueeX, i - topItem, selectedItem == i);
}

void MenuView::handleKey(const UsbkChanges &changes) {
  for (size_t i = 0; i < changes.selLen; i++)
    if (changes.sel[i].make)
      sel(changes.sel[i].usbkSelector);
}

void MenuView::sel(uint8_t usbkSelector) {
  switch (usbkSelector) {
    case USBK_RIGHT:
      switch (selectedItem) {
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
      switch (selectedItem) {
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
      if (selectedItem < MENU_ITEM_COUNT - 1u) {
        selectedItem += 1u;
        marqueeX = 0;
      }
      if (selectedItem - topItem > 2u)
        topItem += 1u;
      break;
    case USBK_UP:
      if (selectedItem > 0u) {
        selectedItem -= 1u;
        marqueeX = 0;
      }
      if (selectedItem < topItem)
        topItem -= 1u;
      break;
    case USBK_RETURN:
    case USBK_ENTER:
      switch (selectedItem) {
        // case N:
        //   state.foo = !state.foo;
        //   break;
      }
      close();
      break;
  }
}
