#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const int IR_RECEIVE_PIN = 34;

IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // 受信を始める
  Serial.println("waiting for IR signal...");
}

void loop() {
  if (irrecv.decode(&results)) {  // 信号を受信して、解読できた
    Serial.print(resultToHumanReadableBasic(&results));  // 方式とデータを表示する
    irrecv.resume();  // 次の信号を受信する
  }
}
