#include "config.h"
#include "menu.h"

#include <string>
#include <string_view>

#include "buzzer.h"
#include "bindings.h"
#include "hal.h"
#include "hostid.h"
#include "pinout.h"
#include "settings.h"
#include "state.h"
#include "sunk.h"
#include "view.h"

MenuView MENU_VIEW{};
WaitView WAIT_VIEW{};
SaveSettingsView SAVE_SETTINGS_VIEW{};

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, bool on, const char *fmt, Args... args);

static Settings newSettings{};

using MenuItemPainter = void (*)(int16_t &marqueeX, size_t i, bool on);
static const MenuItemPainter MENU_ITEM_PAINTERS[] = {
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Go back");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Force click: %s",
      newSettings.forceClick == ForceClick::_::NO ? "no"
      : newSettings.forceClick == ForceClick::_::OFF ? "off"
      : newSettings.forceClick == ForceClick::_::ON ? "on"
      : "?");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Click duration: %ju ms", newSettings.clickDuration);
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Mouse baud: %s",
      newSettings.mouseBaud == MouseBaud::_::S1200 ? "1200"
      : newSettings.mouseBaud == MouseBaud::_::S2400 ? "2400"
      : newSettings.mouseBaud == MouseBaud::_::S4800 ? "4800"
      : newSettings.mouseBaud == MouseBaud::_::S9600 ? "9600"
      : "?");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Hostid: %c%c%c%c%c%c",
      newSettings.hostid[0],
      newSettings.hostid[1],
      newSettings.hostid[2],
      newSettings.hostid[3],
      newSettings.hostid[4],
      newSettings.hostid[5]);
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Reprogram idprom");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Wipe idprom (AAh)");
  },
  [](int16_t &marqueeX, size_t i, bool on) {
    drawMenuItem(marqueeX, i, on, "Help: https://go.daz.cat/usb3sun");
  },
};

static const size_t MENU_ITEM_COUNT = sizeof(MENU_ITEM_PAINTERS) / sizeof(MENU_ITEM_PAINTERS[0]);

template<typename... Args>
static void drawMenuItem(int16_t &marqueeX, size_t i, bool on, const char *fmt, Args... args) {
  char label[256];
  int label_len = snprintf(label, sizeof label, fmt, args...);
  if (static_cast<size_t>(label_len) >= sizeof label) {
    // truncate the text.
    label[sizeof label - 1] = '\0';
  }

  int16_t y = 8 * i;
  if (on) {
    int width = label_len * 6;
    usb3sun_display_rect(4, y, 120, 8, 0, false, true);
    if (width > 112) {
      usb3sun_display_text(8 - marqueeX, y, on, label);
      usb3sun_display_text(8 - marqueeX + width + 112 / 2, y, on, label);
      marqueeX %= width + 112 / 2;
    } else {
      usb3sun_display_text(8, y, on, label);
    }
    usb3sun_display_rect(0, y, 4, 8, 0, true, true);
    usb3sun_display_rect(124, y, 4, 8, 0, true, true);
  } else {
    usb3sun_display_text(8, y, on, label);
    usb3sun_display_rect(0, y, 8, 8, 0, true, true);
    usb3sun_display_rect(120, y, 8, 8, 0, true, true);
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
  newSettings = settings;
  View::push(&MENU_VIEW);
}

void MenuView::close() {
  if (!isOpen)
    return;
  if (newSettings != settings) {
    SAVE_SETTINGS_VIEW.open();
  } else {
    closeWithoutConfirmSave();
  }
}

void MenuView::closeWithoutConfirmSave() {
  if (!isOpen)
    return;
  View::pop();
  isOpen = false;
}

const char *MenuView::name() const {
  return "MenuView";
}

void MenuView::handlePaint() {
  marqueeTick = (marqueeTick + 1) % 2;
  if (marqueeTick == 0)
    marqueeX += 1;

  for (size_t i = topItem; i <= topItem + 3 && i < MENU_ITEM_COUNT; i++)
    MENU_ITEM_PAINTERS[i](marqueeX, i - topItem, selectedItem == i);
}

void MenuView::handleKey(const UsbkChanges &changes) {
  for (size_t i = 0; i < changes.selLen; i++)
    if (changes.sel[i].make)
      sel(changes.sel[i].usbkSelector);
}

void MenuView::sel(uint8_t usbkSelector) {
  switch (usbkSelector) {
    case USBK_ESCAPE:
      close();
      break;
    case USBK_RIGHT:
      switch (selectedItem) {
        case (size_t)MenuItem::ForceClick:
          ++newSettings.forceClick;
          break;
        case (size_t)MenuItem::ClickDuration:
          if (newSettings.clickDuration < 96u) {
            newSettings.clickDuration += 5u;
            buzzer.click(newSettings.clickDuration);
          }
          break;
        case (size_t)MenuItem::MouseBaud:
          ++newSettings.mouseBaud;
          break;
      }
      break;
    case USBK_LEFT:
      switch (selectedItem) {
        case (size_t)MenuItem::ForceClick:
          --newSettings.forceClick;
          break;
        case (size_t)MenuItem::ClickDuration:
          if (newSettings.clickDuration > 4u) {
            newSettings.clickDuration -= 5u;
            buzzer.click(newSettings.clickDuration);
          }
          break;
        case (size_t)MenuItem::MouseBaud:
          --newSettings.mouseBaud;
          break;
      }
      break;
    case USBK_RETURN:
    case USBK_ENTER:
      switch (selectedItem) {
        case (size_t)MenuItem::GoBack:
          close();
          break;
        case (size_t)MenuItem::Hostid:
          HOSTID_VIEW.open(&newSettings.hostid);
          break;
        case (size_t)MenuItem::ReprogramIdprom: {
          WAIT_VIEW.open("Reprogramming...", newSettings.hostid);

          unsigned hostid24 =
            decodeHex(newSettings.hostid[0]) << 20
            | decodeHex(newSettings.hostid[1]) << 16
            | decodeHex(newSettings.hostid[2]) << 12
            | decodeHex(newSettings.hostid[3]) << 8
            | decodeHex(newSettings.hostid[4]) << 4
            | decodeHex(newSettings.hostid[5]);

          unsigned i = 0;
          // https://funny.computer.daz.cat/sun/nvram-hostid-faq.txt
          // version 1
          sunkSend("1 %x mkp\n", i++);

          // hostid byte 1/4 (system type), falling back to 80h for machines without
          // real-machine-type, such as the ultra1 and ultra5
          // <https://funny.computer.daz.cat/sun/nvram-hostid-faq.txt> (“80 SPARCstation Classic, LX, 4, 5, SS1000, Voyager, Ultra 1”)
          // <https://docs.oracle.com/cd/E19127-01/ultra5.ws/805-7763-12/805-7763-12.pdf> (“Host ID: 808e7a85”, “808e7bb1”)
          sunkSend("80 %x mkp\n", i); // real-machine-type fallback, as above
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
          sunkSend("80 %x mkp\n", i); // real-machine-type fallback, as above
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
          close();
        } break;
        case (size_t)MenuItem::WipeIdprom: {
          WAIT_VIEW.open("Wiping...", {});
          for (unsigned i = 0; i < 0xF; i++)
            sunkSend("aa %x mkp\n", i);
          WAIT_VIEW.close();
          close();
        } break;
      }
      break;
    case USBK_DOWN:
      if (selectedItem < MENU_ITEM_COUNT - 1u) {
        selectedItem += 1u;
        marqueeX = 0;
      }
      if (selectedItem - topItem > 3u)
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

const char *WaitView::name() const {
  return "WaitView";
}

void WaitView::handlePaint() {
  usb3sun_display_text(8, 8, false, message);
  if (hostid.has_value()) {
    char hostidText[] = "Hostid: ??????";
    for (size_t i = 0; i < sizeof hostid->value; i++)
      hostidText[sizeof "Hostid: " - 1 + i] = hostid->value[i];
    usb3sun_display_text(8, 16, false, hostidText);
  }
}

void WaitView::handleKey(const UsbkChanges &) {}

void WaitView::open(const char *message, std::optional<HostidV2::Value> hostid) {
  if (isOpen)
    return;
  isOpen = true;
  this->message = message;
  this->hostid = hostid;
  View::push(&WAIT_VIEW);
}

void WaitView::close() {
  if (!isOpen)
    return;
  View::pop();
  isOpen = false;
}

const char *SaveSettingsView::name() const {
  return "SaveSettingsView";
}

void SaveSettingsView::handlePaint() {
  usb3sun_display_hline(4, 8 + 6, 6 * 20 - 1, false, 4);
  usb3sun_display_hline(4, 16 + 6, 6 * 20 - 1, false, 4);
  usb3sun_display_hline(4, 24 + 6, 6 * 20 - 1, false, 4);
  usb3sun_display_text(4, 0, false, "   Save settings?   ", true);
  usb3sun_display_text(4, 8, false, "ENTER\377\377\377save changes", true);
  usb3sun_display_text(4, 16, false, "N\377\377\377\377\377\377\377\377\377don't save", true);
  usb3sun_display_text(4, 24, false, "ESC\377\377\377\377\377\377\377\377\377\377go back", true);
}

void SaveSettingsView::handleKey(const UsbkChanges &changes) {
  for (size_t i = 0; i < changes.selLen; i++) {
    if (changes.sel[i].make) {
      switch (changes.sel[i].usbkSelector) {
        case USBK_RETURN:
        case USBK_ENTER: {
          bool doReboot = false;
          bool doRestartSunm = false;
          if (!!(changes.kreport.modifier & (USBK_SHIFT_L | USBK_SHIFT_R))) {
            doReboot = true;
          }
          if (newSettings.clickDuration != settings.clickDuration) {
            settings.clickDuration = newSettings.clickDuration;
            settings.write<ClickDurationV2>(settings.clickDuration);
          }
          if (newSettings.forceClick != settings.forceClick) {
            settings.forceClick = newSettings.forceClick;
            settings.write<ForceClickV2>(settings.forceClick);
          }
          if (newSettings.mouseBaud != settings.mouseBaud) {
            settings.mouseBaud = newSettings.mouseBaud;
            settings.write<MouseBaudV2>(settings.mouseBaud);
            doRestartSunm = true;
          }
          if (newSettings.hostid != settings.hostid) {
            settings.hostid = newSettings.hostid;
            settings.write<HostidV2>(settings.hostid);
          }
          if (doReboot) {
            usb3sun_reboot();
          } else if (doRestartSunm) {
            pinout.restartSunm();
          }
          close();
          MENU_VIEW.closeWithoutConfirmSave();
        } break;
        case USBK_N:
          close();
          MENU_VIEW.closeWithoutConfirmSave();
          break;
        case USBK_ESCAPE:
          close();
          break;
      }
    }
  }
}

void SaveSettingsView::open() {
  if (isOpen)
    return;
  isOpen = true;
  View::push(&SAVE_SETTINGS_VIEW);
}

void SaveSettingsView::close() {
  if (!isOpen)
    return;
  View::pop();
  isOpen = false;
}
