#include "cli.h"
#include "pinout.h"
#include "sunk.h"
#include "sunm.h"

void handleCliInput(char cur) {
  const size_t escAltTimeout = 100'000ul;
  static char input[256] = "";
  static size_t len = 0;
  static char prev = '\0';
  static bool prevIsEsc = false;
  auto t = micros();
  static auto tPrev = t;
  static auto tMouse = t;
  auto delta = t - tPrev;
  if (prevIsEsc) {
    if (delta < escAltTimeout) {
      bool didMouse = false;
      int8_t x = 0;
      int8_t y = 0;
      static bool left = false;
      static bool middle = false;
      static bool right = false;
      switch (cur) {
        case '1': didMouse = true; left = !left; break;
        case '2': didMouse = true; middle = !middle; break;
        case '3': didMouse = true; right = !right; break;
        case 'q': didMouse = true; x = -1, y = -1; break;
        case 'w': didMouse = true; x = 0, y = -1; break;
        case 'e': didMouse = true; x = 1, y = -1; break;
        case 'a': didMouse = true; x = -1, y = 0; break;
        case 's': didMouse = true; x = 0, y = 1; break;
        case 'd': didMouse = true; x = 1, y = 0; break;
        case 'z': didMouse = true; x = -1, y = 1; break;
        case 'x': didMouse = true; x = 0, y = 1; break;
        case 'c': didMouse = true; x = 1, y = 1; break;
        default:
          if (cur >= ' ' && cur <= '~') {
            Sprintf("\033[33m^[\033[0m%c", cur);
          } else {
            Sprintf("\033[33m^[%02X\033[0m", cur);
          }
          break;
      }
      if (didMouse) {
        const size_t streakTimeout = 100'000ul;
        const size_t accelNum = 3;
        const size_t accelDenom = 10;
        static size_t streak = 0;
        auto deltaMouse = t - tMouse;
        streak = deltaMouse < streakTimeout ? streak + 1 : 0;
        auto speed = 1 + streak * accelNum / accelDenom;
        sunmSend(x * speed, y * speed, left, middle, right);
        tMouse = t;
      }
      goto end;
    } else {
      Sprintf("\033[33m^[\033[0m", cur);
    }
  }
  if (cur >= ' ' && cur <= '~') {
    if (len < sizeof input - 1) {
      Sprintf("%c", cur);
      input[len++] = cur;
      input[len] = '\0';
    }
  } else switch (cur) {
    case '\x7F': // backspace in terminal (^?)
      if (len > 0) {
        Sprintf("\033[D\033[K");
        input[--len] = '\0';
      }
      break;
    case '\x1B': // escape/alt in terminal (^[)
      // do nothing; wait for more input
      break;
    case '\x08': // ^H
      sunkSend("\x7F"); // DEL
      break;
    case '\x0A': // ^J
      sunkSend("\n");
      break;
    case '\r': // enter in terminal (^M)
      Sprintf("\n", cur);
      if (len > 0) {
        // split into words and count
        char *word = input;
        size_t wordCount = 1;
        while (word - input < sizeof input && !!(word = strchr(word, ' '))) {
          word++[0] = '\0';
          wordCount++;
        }
        // now walk through the words
        word = input;
        if (strcmp(word, "type") == 0) {
          if (wordCount > 1) {
            for (size_t i = 1; i < wordCount; i++) {
              word += strlen(word) + 1;
              sunkSend(i == 1 ? "%s" : " %s", word);
            }
          } else {
            Sprintln("usage: type <text>");
          }
        } else if (strcmp(word, "stop") == 0) {
          sunkSend(true, SUNK_STOP);
          for (size_t i = 1; i < wordCount; i++) {
            word += strlen(word) + 1;
            sunkSend(word);
          }
          sunkSend(false, SUNK_STOP);
        } else if (strcmp(word, "enter") == 0) {
          sunkSend("\n");
        } else if (strcmp(word, "go") == 0) {
          sunkSend("go\n");
        } else if (strcmp(word, "help") == 0) {
          Sprintln("alt+WASD        sun mouse: move up/down/left/right");
          Sprintln("alt+QEZC        sun mouse: move diagonally");
          Sprintln("alt+123         sun mouse: toggle left/middle/right button");
          Sprintln("^H              sun keyboard: send {backspace}");
          Sprintln("^J              sun keyboard: send {enter}");
          Sprintln("type <text>     sun keyboard: send text");
          Sprintln("stop a          sun keyboard: send {stop+A}");
          Sprintln("enter           sun keyboard: send {enter}");
          Sprintln("go              sun keyboard: send go{enter}");
        } else {
          Sprintln("unknown command");
        }
      }
      Sprint("> ");
      len = 0;
      input[len] = '\0';
      break;
    default:
      Sprintf("\033[33m%02X\033[0m", cur);
      break;
  }
end:
  prev = cur;
  prevIsEsc = prev == '\x1B';
  tPrev = t;
}
