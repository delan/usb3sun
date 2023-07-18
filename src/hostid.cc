#include "config.h"
#include "hostid.h"

#include <cstring>

#include "bindings.h"
#include "display.h"

HostidView HOSTID_VIEW{};

void HostidView::handlePaint() {
    display.setCursor(8, 8);
    display.printf("Hostid:");
    display.setCursor(8 + 6 * cursorIndex, 18);
    display.print("_");
    display.setCursor(8, 16);
    display.printf("%c%c%c%c%c%c", hostid[0], hostid[1], hostid[2], hostid[3], hostid[4], hostid[5]);
    display.setCursor(60, 8);
    display.print("ENTER: ok");
    display.setCursor(60, 16);
    display.print("ESC: cancel");
  }

void HostidView::handleKey(const UsbkChanges &changes) {
  for (size_t i = 0; i < changes.selLen; i++) {
    if (changes.sel[i].make) {
      switch (changes.sel[i].usbkSelector) {
        case USBK_RETURN:
        case USBK_ENTER:
          ok();
          break;
        case USBK_ESCAPE:
          cancel();
          break;
        case USBK_LEFT:
          left();
          break;
        case USBK_RIGHT:
          right();
          break;
        case USBK_1:
          type('1');
          break;
        case USBK_2:
          type('2');
          break;
        case USBK_3:
          type('3');
          break;
        case USBK_4:
          type('4');
          break;
        case USBK_5:
          type('5');
          break;
        case USBK_6:
          type('6');
          break;
        case USBK_7:
          type('7');
          break;
        case USBK_8:
          type('8');
          break;
        case USBK_9:
          type('9');
          break;
        case USBK_0:
          type('0');
          break;
        case USBK_A:
          type('A');
          break;
        case USBK_B:
          type('B');
          break;
        case USBK_C:
          type('C');
          break;
        case USBK_D:
          type('D');
          break;
        case USBK_E:
          type('E');
          break;
        case USBK_F:
          type('F');
          break;
      }
    }
  }
}

void HostidView::open(const Hostid currentHostid) {
  if (isOpen)
    return;
  isOpen = true;
  memcpy(hostid, currentHostid, 6);
  cursorIndex = 0;
  View::push(&HOSTID_VIEW);
}

void HostidView::ok() {
  memcpy(settings.hostid(), hostid, 6);
  settings.write(settings.hostid_field);
  cancel();
}

void HostidView::cancel() {
  if (!isOpen)
    return;
  View::pop();
  isOpen = false;
}

void HostidView::left() {
  cursorIndex = cursorIndex > 0 ? cursorIndex - 1 : 5;
}

void HostidView::right() {
  cursorIndex = cursorIndex < 5 ? cursorIndex + 1 : 0;
}

void HostidView::type(unsigned char digit) {
  hostid[cursorIndex] = digit;
  right();
}
