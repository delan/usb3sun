#include "config.h"
#include "view.h"

#include <cstddef>

#include "panic.h"

static View *views[3]{};
static size_t viewsLen = 0;

void View::push(View *view) {
  if (viewsLen >= sizeof(views) / sizeof(*views))
    panic2("View stack overflow");

  views[viewsLen++] = view;
}

void View::pop() {
  if (viewsLen == 0)
    panic2("View stack underflow");

  views[--viewsLen] = nullptr;
}

void View::paint() {
  if (viewsLen == 0)
    panic2("View stack empty");

  views[viewsLen - 1]->handlePaint();
}

void View::key(const UsbkChanges &changes) {
  if (viewsLen == 0)
    panic2("View stack empty");

  views[viewsLen - 1]->handleKey(changes);
}
