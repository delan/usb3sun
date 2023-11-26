#include "config.h"
#include "menu.h"

#include <string>
#include <string_view>

#include "buzzer.h"
#include "bindings.h"
#include "display.h"
#include "hostid.h"
#include "gpio.h"
#include "settings.h"
#include "state.h"
#include "sunk.h"
#include "view.h"

MenuView MENU_VIEW{};
WaitView WAIT_VIEW{};

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, bool on, const char *fmt, Args... args);

enum class MenuItem : size_t {
  GoBack,
  ForceClick,
  ClickDuration,
  MouseBaud,
  Hostid,
  ReprogramIdprom,
  WipeIdprom,
};

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
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Mouse baud: %s",
      settings.mouseBaud() == MouseBaud::_::S1200 ? "1200"
      : settings.mouseBaud() == MouseBaud::_::S2400 ? "2400"
      : settings.mouseBaud() == MouseBaud::_::S4800 ? "4800"
      : settings.mouseBaud() == MouseBaud::_::S9600 ? "9600"
      : "?");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Hostid: %c%c%c%c%c%c",
      settings.hostid()[0],
      settings.hostid()[1],
      settings.hostid()[2],
      settings.hostid()[3],
      settings.hostid()[4],
      settings.hostid()[5]);
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Reprogram idprom");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Wipe idprom (AAh)");
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

unsigned decodeHex(unsigned char digit) {
  if (digit >= '0' && digit <= '9')
    return digit - '0';
  if (digit >= 'A' && digit <= 'F')
    return digit - 'A' + 10;
  if (digit >= 'a' && digit <= 'f')
    return digit - 'a' + 10;
  return 0;
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
        case (size_t)MenuItem::ForceClick:
          ++settings.forceClick();
          settings.write(settings.forceClick_field);
          break;
        case (size_t)MenuItem::ClickDuration:
          if (settings.clickDuration() < 96u) {
            settings.clickDuration() += 5u;
            settings.write(settings.clickDuration_field);
            buzzer.click();
          }
          break;
        case (size_t)MenuItem::MouseBaud:
          ++settings.mouseBaud();
          settings.write(settings.mouseBaud_field);
          pinout.restartSunm();
          break;
      }
      break;
    case USBK_LEFT:
      switch (selectedItem) {
        case (size_t)MenuItem::ForceClick:
          --settings.forceClick();
          settings.write(settings.forceClick_field);
          break;
        case (size_t)MenuItem::ClickDuration:
          if (settings.clickDuration() > 4u) {
            settings.clickDuration() -= 5u;
            settings.write(settings.clickDuration_field);
            buzzer.click();
          }
          break;
        case (size_t)MenuItem::MouseBaud:
          --settings.mouseBaud();
          settings.write(settings.mouseBaud_field);
          pinout.restartSunm();
          break;
      }
      break;
    case USBK_RETURN:
    case USBK_ENTER:
      switch (selectedItem) {
        case (size_t)MenuItem::Hostid:
          HOSTID_VIEW.open(settings.hostid());
          break;
        case (size_t)MenuItem::ReprogramIdprom: {
          WAIT_VIEW.open("Reprogramming...");

          unsigned hostid24 =
            decodeHex(settings.hostid()[0]) << 20
            | decodeHex(settings.hostid()[1]) << 16
            | decodeHex(settings.hostid()[2]) << 12
            | decodeHex(settings.hostid()[3]) << 8
            | decodeHex(settings.hostid()[4]) << 4
            | decodeHex(settings.hostid()[5]);

          unsigned i = 0;
          // https://funny.computer.daz.cat/sun/nvram-hostid-faq.txt
          // version 1
          sunkSend("1 %x mkp\n", i++);

          // hostid byte 1/4 (system type)
          sunkSend("real-machine-type %x mkp\n", i++);

          // ethernet address oui (always 08:00:20)
          sunkSend("8 %x mkp\n", i++);
          sunkSend("0 %x mkp\n", i++);
          sunkSend("20 %x mkp\n", i++);

          // set ethernet address lower half such that hostid bytes 2/3/4
          // cancels it out in the checksum
          sunkSend("%x %x mkp\n", hostid24 >> 16 & 0xFF, i++);
          sunkSend("%x %x mkp\n", hostid24 >> 8 & 0xFF, i++);
          sunkSend("%x %x mkp\n", hostid24 >> 0 & 0xFF, i++);

          // set date of manufacture such that the system type byte
          // cancels it out in the checksum
          sunkSend("real-machine-type %x mkp\n", i++);
          sunkSend("0 %x mkp\n", i++);
          sunkSend("0 %x mkp\n", i++);
          sunkSend("0 %x mkp\n", i++);

          // hostid bytes 2/3/4
          sunkSend("%x %x mkp\n", hostid24 >> 16 & 0xFF, i++);
          sunkSend("%x %x mkp\n", hostid24 >> 8 & 0xFF, i++);
          sunkSend("%x %x mkp\n", hostid24 >> 0 & 0xFF, i++);

          // 01h ^ 08h ^ 20h = 29h
          // sunkSend("0 %x 0 do i idprom@ xor loop f mkp\n", i++);
          sunkSend("29 %x mkp\n", i++);

          // only needed for SS1000, but harmless otherwise
          sunkSend("update-system-idprom\n");

          sunkSend(".idprom\n");
          sunkSend("banner\n");

          WAIT_VIEW.close();
        } break;
        case (size_t)MenuItem::WipeIdprom: {
          WAIT_VIEW.open("Wiping...");
          for (unsigned i = 0; i < 0xF; i++)
            sunkSend("aa %x mkp\n", i);
          WAIT_VIEW.close();
        } break;
        default:
          close();
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
  }
}

void WaitView::handlePaint() {
  display.setCursor(8, 8);
  display.print(message);
}

void WaitView::handleKey(const UsbkChanges &) {}

void WaitView::open(const char *message) {
  if (isOpen)
    return;
  isOpen = true;
  this->message = message;
  View::push(&WAIT_VIEW);
}

void WaitView::close() {
  if (!isOpen)
    return;
  View::pop();
  isOpen = false;
}
