#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int POT_PIN = 32;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 0x3C は OLED の I2C アドレス
    Serial.println("SSD1306 not found");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  int raw = analogRead(POT_PIN);
  int barWidth = map(raw, 0, 4095, 0, SCREEN_WIDTH);

  display.clearDisplay();  // 画面を消す（内部のバッファを消すだけで、まだ表示は変わらない）

  // 文字を表示する
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Potentiometer");

  display.setTextSize(2);  // 2 倍の大きさ
  display.setCursor(0, 16);
  display.println(raw);

  // バーグラフを描く
  display.drawRect(0, 44, SCREEN_WIDTH, 16, SSD1306_WHITE);  // 枠
  display.fillRect(0, 44, barWidth, 16, SSD1306_WHITE);      // 中身

  display.display();  // バッファの内容を画面に送る
  delay(50);
}
