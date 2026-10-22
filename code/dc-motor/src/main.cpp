#include <Arduino.h>

const int AIN1 = 25;
const int AIN2 = 26;
const int PWMA = 27;
const int POT_PIN = 32;

// speed：-255（逆転・最大）〜 0（停止）〜 255（正転・最大）
void setMotor(int speed) {
  if (speed > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (speed < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
  analogWrite(PWMA, abs(speed));  // 速さは PWM のデューティ比で決める
}

void setup() {
  Serial.begin(115200);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
}

void loop() {
  int raw = analogRead(POT_PIN);
  int speed = map(raw, 0, 4095, -255, 255);  // つまみの中央で停止

  // 中央付近は停止にする（つまみを中央にしても完全に 0 にはならないため）
  if (abs(speed) < 30) {
    speed = 0;
  }

  setMotor(speed);
  Serial.printf("speed = %d\n", speed);
  delay(50);
}
