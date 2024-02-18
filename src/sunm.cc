#include "sunm.h"
#include "bindings.h"
#include "pinout.h"

#include <cstddef>

void sunmSend(int8_t x, int8_t y, bool left, bool middle, bool right) {
      // correct: https://web.archive.org/web/20220226000612/http://www.bitsavers.org/pdf/mouseSystems/300771-001_Mouse_Systems_Optical_Mouse_Technical_Reference_Models_M2_and_M3_1985.pdf
      // wrong: https://web.archive.org/web/20100213183456/http://privatewww.essex.ac.uk/~nbb/mice-pc.html
      // note in particular that:
      // • positive dx is right, but positive dy is up
      // • buttons are 0 when pressed and 1 when released
      uint8_t result[] = {
        0x80
          | (left ? 0 : SUNM_LEFT)
          | (middle ? 0 : SUNM_CENTER)
          | (right ? 0 : SUNM_RIGHT),
        (uint8_t) x, (uint8_t) -y, 0, 0,
      };
#ifdef SUNM_ENABLE
      size_t len = pinout.sunm->write(result, sizeof(result) / sizeof(*result));
#ifdef SUNM_VERBOSE
      Sprintf("sunm: tx %02Xh %02Xh %02Xh %02Xh %02Xh = %zu\n",
        result[0], result[1], result[2], result[3], result[4], len);
#endif
#else
      Sprintf("sunm: tx %02Xh %02Xh %02Xh %02Xh %02Xh (disabled)\n",
        result[0], result[1], result[2], result[3], result[4]);
#endif
}
