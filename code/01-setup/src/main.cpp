#include <Arduino.h>

// 電源が入ったときに、1回だけ実行される
void setup() {
  Serial.begin(115200);  // PCとの通信を始める
}

// setup() のあと、繰り返し実行される
void loop() {
  Serial.println("Hello, ESP32!");  // PCに文字列を送る
  delay(1000);                      // 1000ミリ秒（1秒）待つ
}
