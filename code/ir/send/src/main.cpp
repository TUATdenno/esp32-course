#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const int IR_SEND_PIN = 23;
const int BUTTON_PIN = 4;

// 受信したときに表示された Code に書き換える
const uint64_t CODE = 0x20DF10EF;

IRsend irsend(IR_SEND_PIN);
bool lastState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  irsend.begin();
}

void loop() {
  bool state = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && state == LOW) {
    irsend.sendNEC(CODE, 32);  // NEC 方式で 32 ビット送る
    Serial.println("sent");
    delay(50);
  }

  lastState = state;
}
