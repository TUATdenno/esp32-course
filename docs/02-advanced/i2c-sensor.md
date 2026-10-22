# I2C センサー

## このステップでやること

I2C という通信方式でセンサーとつなぎ、温度・湿度・気圧を測ってシリアルモニタに表示します。
I2C は、センサーやディスプレイなど多くの部品で使われている通信方式です。一度使い方を覚えれば、ほかの I2C 部品にも応用できます。

## 前提ステップ・使う部品

- 前提：第1部
- 部品：
    - BME280 モジュール（温度・湿度・気圧センサー）× 1
    - ジャンパワイヤ

!!! note "センサーが違う場合"
    部室にあるセンサーが BME280 でない場合でも、I2C の部分（配線と I2C スキャナー）は同じです。センサー本体のプログラムは、そのセンサー用のライブラリに合わせて書き換えてください。

## I2C とは

**I2C**（アイ・スクエア・シー）は、2 本の信号線で複数の部品と通信できる方式です。

| 信号線 | 役割 |
|---|---|
| SDA | データをやり取りする線 |
| SCL | 通信のタイミングを合わせるためのクロックの線 |

I2C の部品には、それぞれ **アドレス**（`0x76` などの番号）が決まっています。ESP32 はアドレスを指定して通信相手を選ぶので、2 本の線に複数の部品を並べてつなげます。

!!! todo "画像：I2C で複数の部品がつながっている図"

## 配線

ESP32 では、GPIO21 が SDA、GPIO22 が SCL として標準で使われます。

| ESP32 | BME280 |
|---|---|
| 3V3 | VIN（VCC） |
| GND | GND |
| GPIO21 | SDA |
| GPIO22 | SCL |

!!! todo "画像：配線図"

!!! todo "画像：実際の配線写真"

## I2C スキャナー

まずは、センサーが正しくつながっているかを確認します。次のプログラムは、つながっている I2C 部品のアドレスを調べて表示します。

```cpp title="scanner/src/main.cpp"
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
```

`found: 0x76` または `found: 0x77` と表示されれば、BME280 が正しくつながっています。表示されたアドレスは、次のプログラムで使うので控えておいてください。

## センサーの値を読む

BME280 用のライブラリを使います。新しくプロジェクトを作り、`platformio.ini` に `lib_deps` を追加します。

```ini title="bme280/platformio.ini"
[env:esp32dev]
platform = espressif32 @ 6.9.0
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    adafruit/Adafruit BME280 Library @ ^2.2.4
```

```cpp title="bme280/src/main.cpp"
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
```

センサーに息を吹きかけると、温度と湿度が上がることを確認してください。

!!! mame "I2C のアドレスが 2 種類ある理由"
    同じ部品を 2 つ使いたい場合に備えて、多くの I2C 部品はアドレスを切り替えられるようになっています。BME280 のモジュールでは、基板上の端子のつなぎ方で `0x76` と `0x77` を切り替えられます。

!!! mame "プルアップ抵抗"
    I2C の信号線は、プルアップ抵抗で 3.3 V に引き上げておく必要があります（[4. ボタン入力](../01-basics/04-button.md) のプルアップと同じ考え方です）。
    市販のセンサーモジュールの多くは、プルアップ抵抗が基板に載っているので、そのままつなぐだけで使えます。

## 発展課題

- [ ] 温度が 28 ℃ を超えたら LED を点灯させる
- [ ] 気圧から高度を計算して表示する（ヒント：`bme.readAltitude(1013.25)`）
- [ ] 10 秒ごとの値を記録し、1 分間の平均を表示する

## うまくいかないとき

??? question "I2C スキャナーで `no device found` と表示される"
    - SDA と SCL が逆になっていないか確認してください
    - センサーの VIN と GND が正しくつながっているか確認してください
    - センサーモジュールのピンヘッダがはんだ付けされているか確認してください。挿しているだけでは接触不良になります

??? question "`BME280 not found` と表示される"
    - `SENSOR_ADDRESS` が、I2C スキャナーで表示されたアドレスと一致しているか確認してください
    - 見た目がよく似た **BMP280**（湿度を測れない）の可能性があります。その場合は `Adafruit BMP280 Library` を使ってください

## チェックポイント

!!! success "チェックポイント"
    温度・湿度・気圧がシリアルモニタに表示されたら、先輩に見せて進捗表にチェックをもらってください。
