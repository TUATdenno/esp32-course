#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

const uint8_t SENSOR_ADDRESS = 0x76;  // I2C スキャナーで表示されたアドレス

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);

  if (!bme.begin(SENSOR_ADDRESS)) {
    Serial.println("BME280 not found");
    while (true) {
      delay(1000);  // 見つからなければ、ここで止める
    }
  }
}

void loop() {
  float temperature = bme.readTemperature();      // ℃
  float humidity = bme.readHumidity();            // %
  float pressure = bme.readPressure() / 100.0F;   // Pa → hPa

  Serial.printf("%.1f C, %.1f %%, %.1f hPa\n", temperature, humidity, pressure);
  delay(1000);
}
