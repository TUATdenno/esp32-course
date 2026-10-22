#include <Arduino.h>

const int LED_PIN = 13;
const int BUTTON_PIN = 4;

bool lastState = HIGH;  // 前回読み取ったボタンの状態
bool ledOn = false;     // LED が点灯しているか

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  bool state = digitalRead(BUTTON_PIN);

  // 前回は離していて、今回は押している → 押された瞬間
  if (lastState == HIGH && state == LOW) {
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    Serial.println(ledOn ? "LED ON" : "LED OFF");
    delay(50);  // チャタリング対策
  }

  lastState = state;
}
