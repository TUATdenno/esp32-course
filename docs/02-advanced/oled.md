# OLED ディスプレイ

## このステップでやること

小型の OLED ディスプレイに文字や図形を表示します。可変抵抗の値を、数値とバーグラフで表示するプログラムを作ります。
ディスプレイがあれば、PC につながなくても作品の状態を確認できるようになります。

!!! todo "画像：OLED にバーグラフが表示されている様子"

## 前提ステップ・使う部品

- 前提：第1部、[I2C センサー](i2c-sensor.md)（I2C の説明）
- 部品：
    - OLED ディスプレイ（SSD1306、128 × 64、I2C 接続）× 1
    - 可変抵抗（10 kΩ）× 1
    - ジャンパワイヤ

## 配線

| ESP32 | 接続先 |
|---|---|
| 3V3 | OLED の VCC、可変抵抗の端の足 |
| GND | OLED の GND、可変抵抗のもう一方の端の足 |
| GPIO21 | OLED の SDA |
| GPIO22 | OLED の SCL |
| GPIO32 | 可変抵抗の中央の足 |

!!! todo "画像：配線図"

!!! todo "画像：実際の配線写真"

!!! warning "ピンの並び順に注意"
    OLED モジュールは、製品によってピンの並び順（VCC と GND の位置など）が異なります。基板に印刷されている表記を必ず確認してからつないでください。

## コード

```ini title="platformio.ini"
[env:esp32dev]
platform = espressif32 @ 6.9.0
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    adafruit/Adafruit SSD1306 @ ^2.5.13
    adafruit/Adafruit GFX Library @ ^1.12.0
```

```cpp title="src/main.cpp"
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
```

- 画面の座標は、**左上が (0, 0)** で、右に行くほど x が、下に行くほど y が大きくなります
- 描画の関数は、ESP32 の中のメモリ（バッファ）に絵を描くだけです。`display.display()` を呼んだときに、まとめて画面に送られます

| 関数 | 動作 |
|---|---|
| `setCursor(x, y)` | 文字を書き始める位置を決める |
| `setTextSize(n)` | 文字の大きさを n 倍にする |
| `drawLine(x0, y0, x1, y1, 色)` | 線を描く |
| `drawRect(x, y, 幅, 高さ, 色)` | 四角形の枠を描く |
| `fillRect(x, y, 幅, 高さ, 色)` | 塗りつぶした四角形を描く |
| `drawCircle(x, y, 半径, 色)` | 円を描く |

!!! mame "OLED"
    **O**rganic **L**ight **E**mitting **D**iode（有機 EL）の略です。液晶と違ってバックライトがなく、画素そのものが光るので、黒がはっきり表示され、消費電力も小さいのが特徴です。スマホの画面にも使われています。

!!! mame "日本語の表示"
    このライブラリで表示できるのは、英数字と一部の記号だけです。日本語を表示したい場合は、日本語フォントを含むライブラリ（U8g2 など）を使うか、文字を画像として用意して表示します。

## 発展課題

- [ ] [I2C センサー](i2c-sensor.md) と組み合わせて、温度と湿度を表示する（2 つの部品を同じ SDA・SCL につなげます）
- [ ] 可変抵抗の値に合わせて、円の大きさが変わるようにする
- [ ] 可変抵抗の値の変化を、折れ線グラフで表示する

## うまくいかないとき

??? question "`SSD1306 not found` と表示される"
    - [I2C センサー](i2c-sensor.md) の I2C スキャナーでアドレスを確認してください。`0x3D` の場合は、コードの `0x3C` を書き換えてください
    - VCC と GND の位置を、基板の表記で確認してください

??? question "画面に何も表示されない"
    `display.display()` を呼んでいるか確認してください。呼ばないと画面は更新されません。

## チェックポイント

!!! success "チェックポイント"
    可変抵抗の値が数値とバーグラフで表示されたら、先輩に見せて進捗表にチェックをもらってください。
