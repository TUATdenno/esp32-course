#include <Arduino.h>

const int LED_PIN = 13;
const int POT_PIN = 32;  // 可変抵抗をつないだピン

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int raw = analogRead(POT_PIN);           // 0〜4095
  int mv = analogReadMilliVolts(POT_PIN);  // 電圧（mV）
  int duty = map(raw, 0, 4095, 0, 255);    // 0〜4095 を 0〜255 に変換する

  analogWrite(LED_PIN, duty);
  Serial.printf("raw = %4d, voltage = %4d mV, duty = %3d\n", raw, mv, duty);
  delay(100);
}
