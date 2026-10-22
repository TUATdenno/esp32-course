# 赤外線の送受信

## このステップでやること

テレビなどのリモコンの赤外線の信号を受信して、内容を表示します。さらに、受信した信号と同じものを ESP32 から送信します。
家電の操作や、赤外線を使った対戦ゲームなどに応用できます。

## 前提ステップ・使う部品

- 前提：第1部（特に [4. ボタン入力](../01-basics/04-button.md)）
- 部品：
    - 赤外線受信モジュール（OSRB38C9AA、VS1838B など、38 kHz 用）× 1
    - 赤外線 LED × 1
    - 抵抗 100 Ω × 1
    - タクトスイッチ × 1
    - 家電のリモコン（テレビ、エアコンなど）
    - ジャンパワイヤ

## 配線

| ESP32 | 接続先 |
|---|---|
| 3V3 | 受信モジュールの VCC（Vs） |
| GND | 受信モジュールの GND |
| GPIO34 | 受信モジュールの OUT |
| GPIO23 | 抵抗 100 Ω → 赤外線 LED のアノード（足が長い方） |
| GND | 赤外線 LED のカソード（足が短い方） |
| GPIO4 | タクトスイッチ → GND |

!!! todo "画像：配線図"

!!! todo "画像：実際の配線写真"

!!! warning "受信モジュールのピンの並び"
    赤外線受信モジュールは、型番によって 3 本の足の並び順（OUT・GND・VCC）が異なります。必ずデータシートや、モジュールの基板の表記を確認してからつないでください。

## 赤外線リモコンの仕組み

リモコンの赤外線 LED は、ただ点灯しているのではなく、**38 kHz で高速に点滅** しています。この点滅している時間と、していない時間の長さの組み合わせで、0 と 1 のデータを表しています。

受信モジュールは、38 kHz で点滅している赤外線だけを検出し、検出している間は `LOW` を出力します。そのため、太陽光や照明の赤外線には反応しにくくなっています。

!!! todo "画像：赤外線リモコンの信号の波形"

!!! mame "赤外線を見る方法"
    赤外線は人の目には見えませんが、スマホのカメラを通すと、紫っぽく光って見えることがあります（機種によっては、赤外線を遮るフィルタが付いていて見えません）。リモコンや赤外線 LED が光っているかを確かめるのに便利です。

## 信号を受信する

```ini title="platformio.ini"
[env:esp32dev]
platform = espressif32 @ 6.9.0
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    crankyoldgit/IRremoteESP8266 @ ^2.8.6
```

```cpp title="receive/src/main.cpp"
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
```

受信モジュールにリモコンを向けてボタンを押すと、次のように表示されます。

```
Protocol  : NEC
Code      : 0x20DF10EF (32 Bits)
```

`Protocol` は信号の方式、`Code` はボタンごとに決まったデータです。メーカーや機器によって方式が異なります。

!!! mame "赤外線リモコンの方式"
    日本の家電でよく使われる方式には、**NEC フォーマット** と **家製協（AEHA）フォーマット** があります。ソニーの製品は独自の **SONY フォーマット** を使っています。
    エアコンのリモコンは、温度や風量などの設定をまとめて毎回送るので、データがとても長くなります。

## 信号を送信する

受信して表示された `Code` を、ボタンを押したときに送信します。送信側のプログラムでは、`IRsend.h` を使います。

```cpp title="send/src/main.cpp"
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
```

`platformio.ini` は受信のときと同じです。赤外線 LED を家電に向けてボタンを押し、家電が反応するか確かめてください。

- NEC 以外の方式の場合は、`sendNEC` を方式に合った関数（`sendSony`、`sendPanasonic` など）に変えます
- `Protocol` が `UNKNOWN` の場合は、生のデータを送る `sendRaw` を使う必要があります

!!! mame "赤外線 LED の電流"
    赤外線 LED は、たくさん電流を流すほど遠くまで信号が届きます。ESP32 のピンから直接駆動すると 20 mA 程度しか流せないので、届く距離は 1〜2 m 程度です。
    遠くまで届かせたい場合は、トランジスタを使って 100 mA 程度の電流を流します（パルス状に光らせるので、平均の電流は小さくなります）。

## 発展課題

- [ ] リモコンの特定のボタン（例：電源ボタン）を受信したときだけ、LED を点灯させる
- [ ] ボタンを 2 つにして、テレビの音量の上げ下げを送信する
- [ ] 2 台の ESP32 で、片方から送信した信号をもう一方で受信する

## うまくいかないとき

??? question "リモコンを押しても何も表示されない"
    - 受信モジュールの足の並び（OUT・GND・VCC）を、データシートで確認してください
    - スマホのカメラで、リモコンが光っているか確認してください
    - リモコンの電池が切れていないか確認してください

??? question "送信しても家電が反応しない"
    - スマホのカメラで、赤外線 LED が光っているか確認してください
    - 赤外線 LED の向き（足が長い方がアノード）を確認してください
    - 家電に近づけて試してください
    - `sendNEC` が、受信したときの `Protocol` に合っているか確認してください

## チェックポイント

!!! success "チェックポイント"
    リモコンの信号を受信して表示でき、ESP32 から送信した信号で家電（または別の ESP32）が反応したら、先輩に見せて進捗表にチェックをもらってください。
