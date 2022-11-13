// beep with digital GPIO, because the following doesn’t work for
// longer than a second or so at a time:
//    analogWriteFreq(440);
//    analogWriteRange(255);
//    analogWrite(A2, 128);

struct Beep {
  unsigned count{};
  void pop();
  void beep();
  void min(unsigned count);
  static void task(void *beep);
  void task();
};
