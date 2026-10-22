#include <Arduino.h>

const int LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // だんだん明るくする
  for (int duty = 0; duty <= 255; duty++) {
    analogWrite(LED_PIN, duty);  // デューティ比を 0〜255 で指定する
    delay(5);
  }
  // だんだん暗くする
  for (int duty = 255; duty >= 0; duty--) {
    analogWrite(LED_PIN, duty);
    delay(5);
  }
}
