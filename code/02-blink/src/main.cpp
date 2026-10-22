#include <Arduino.h>

const int LED_PIN = 13;  // LED をつないだピン

void setup() {
  pinMode(LED_PIN, OUTPUT);  // ピンを出力モードにする
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // 3.3 V を出す → 点灯
  delay(500);
  digitalWrite(LED_PIN, LOW);   // 0 V にする → 消灯
  delay(500);
}
