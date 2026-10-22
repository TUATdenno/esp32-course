#include <Arduino.h>

const int LED_PIN = 13;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("1: LED ON / 0: LED OFF");
}

void loop() {
  // 受信したデータがあるか確認する
  if (Serial.available() > 0) {
    char c = Serial.read();  // 1文字読み取る

    if (c == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    } else if (c == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    } else if (c != '\r' && c != '\n') {  // 改行コードは無視する
      Serial.printf("unknown command: %c\n", c);
    }
  }
}
