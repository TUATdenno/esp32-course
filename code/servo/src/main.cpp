#include <Arduino.h>
#include <ESP32Servo.h>

const int SERVO_PIN = 18;
const int POT_PIN = 32;

Servo servo;

void setup() {
  Serial.begin(115200);
  servo.setPeriodHertz(50);           // 50 Hz（20 ms 周期）
  servo.attach(SERVO_PIN, 500, 2400); // 0° と 180° のパルス幅（µs）
}

void loop() {
  int raw = analogRead(POT_PIN);
  int angle = map(raw, 0, 4095, 0, 180);

  servo.write(angle);  // 角度を指定する（0〜180°）
  Serial.printf("angle = %d\n", angle);
  delay(20);
}
