#include <Arduino.h>

const int LED_PIN = 13;
const int BUTTON_PIN = 4;

unsigned long lastBlink = 0;  // 最後に LED を切り替えた時刻
unsigned long interval = 500; // 点滅の間隔（ms）
bool ledOn = false;

bool lastState = HIGH;
unsigned long lastPress = 0;  // 最後にボタンが押された時刻

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();

  // 処理1：interval ごとに LED を切り替える
  if (now - lastBlink >= interval) {
    lastBlink = now;
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  }

  // 処理2：ボタンが押されたら点滅の速さを変える
  bool state = digitalRead(BUTTON_PIN);
  if (lastState == HIGH && state == LOW && now - lastPress > 50) {  // 50 ms 以内の変化はチャタリングとして無視
    lastPress = now;
    interval = (interval == 500) ? 100 : 500;
    Serial.printf("interval = %lu ms\n", interval);
  }
  lastState = state;
}
