#include "beep.h"

#include <Arduino.h>

#include <FreeRTOS.h>
#include <task.h>

void Beep::pop() {
  min(1);
}
void Beep::beep() {
  min(10);
}
void Beep::min(unsigned count) {
  if (count > this->count)
    this->count = count;
}
void Beep::task(void *beep) {
  ((Beep *) beep)->task();
}
void Beep::task() {
  vTaskCoreAffinitySet(nullptr, 1 << 1);
  pinMode(20, OUTPUT_12MA);
  while (true) {
    if (count == 0) {
      delay(50);
      continue;
    } else {
      count--;
    }
    digitalWrite(20, HIGH);
    delayMicroseconds(240);
    digitalWrite(20, LOW);
    delayMicroseconds(240);
  }
  vTaskDelete(nullptr);
}
