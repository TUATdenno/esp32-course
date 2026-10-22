#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();  // SDA = GPIO21、SCL = GPIO22 で I2C を始める
}

void loop() {
  Serial.println("Scanning...");
  int found = 0;

  for (int address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {  // 応答があった
      Serial.printf("  found: 0x%02X\n", address);
      found++;
    }
  }

  if (found == 0) {
    Serial.println("  no device found");
  }
  delay(3000);
}
