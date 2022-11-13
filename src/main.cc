#include <Arduino.h>
#include <Keyboard.h>
#include <Wire.h>

#include <FreeRTOS.h>
#include <task.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "beep.h"

static Adafruit_SSD1306 display(128, 32, &Wire1, /* OLED_RESET */ -1);
static Beep beep;

void feedback(void *) {
  vTaskCoreAffinitySet(nullptr, 1 << 1);

  pinMode(18, OUTPUT_12MA);
  pinMode(19, OUTPUT_12MA);

  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Sun input emulator");
    display.setCursor(0, 12);
    display.print("CAP  CMP  SCR  NUM");
    display.drawRoundRect(0, 20, 18, 12, 4, SSD1306_WHITE);
    display.drawRoundRect(30, 20, 18, 12, 4, SSD1306_WHITE);
    display.drawRoundRect(60, 20, 18, 12, 4, SSD1306_WHITE);
    display.drawRoundRect(90, 20, 18, 12, 4, SSD1306_WHITE);
    display.display();

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(18, LOW);
    digitalWrite(19, HIGH);
    delay(420);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(18, HIGH);
    digitalWrite(19, LOW);
    delay(420);
  }

  vTaskDelete(nullptr);
}

void usb(void *) {
  while (true);

  vTaskDelete(nullptr);
}

void setup() {
  // needs to be done manually when using FreeRTOS
  Serial.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  // Wire1.setSDA(31);
  // Wire1.setSCL(32);
  display.begin(SSD1306_SWITCHCAPVCC, /* SCREEN_ADDRESS */ 0x3C);
  display.setRotation(2);
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.clearDisplay();

  xTaskCreate(feedback, "feedback", 2048, nullptr, 1, nullptr);
  xTaskCreate(Beep::task, "beep", 2048, &beep, 1, nullptr);
  // xTaskCreate(usb, "usb", 2048, nullptr, 1, nullptr);

  while (!Serial);
  Serial.write("\r\n\r\nSun input emulator");
}

void loop() {}

void serialEvent() {
  beep.pop();
  while (Serial.available() > 0)
    Serial.read();
}
