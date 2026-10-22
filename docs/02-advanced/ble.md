# BLE

## このステップでやること

BLE（Bluetooth Low Energy）を使って、スマホのアプリと ESP32 の間でデータをやり取りします。

- スマホ → ESP32：送った値で LED を操作する
- ESP32 → スマホ：カウンターの値を 1 秒ごとに送る

BLE は、ワイヤレスイヤホンやスマートウォッチ、各種センサーなどで広く使われている通信方式です。

## 前提ステップ・使う部品

- 前提：第1部
- 部品：
    - ステップ2の LED の回路（GPIO13 → 抵抗 → LED → GND）
    - スマホ
- アプリ：**nRF Connect for Mobile**（Nordic Semiconductor 製、iOS / Android 対応、無料）

## BLE の仕組み

BLE では、データを次のような階層で整理して公開します。

| 用語 | 意味 | この講座の例 |
|---|---|---|
| **サービス** | 機能のまとまり | 「LED 操作サービス」 |
| **キャラクタリスティック** | サービスの中の、1 つ 1 つのデータ | 「LED の状態」「カウンター」 |
| **UUID** | サービスやキャラクタリスティックを区別するための番号 | `4fafc201-...` |

キャラクタリスティックごとに、スマホから **書き込める（Write）**、**読める（Read）**、**変化したときに通知できる（Notify）** といった性質を決めます。

## コード

```ini title="platformio.ini"
[env:esp32dev]
platform = espressif32 @ 6.9.0
board = esp32dev
framework = arduino
monitor_speed = 115200
board_build.partitions = huge_app.csv  ; BLE のライブラリは大きいので、プログラム領域を広げる
```

```cpp title="src/main.cpp"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

const int LED_PIN = 13;

// UUID は https://www.uuidgenerator.net/ などで作れる
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define LED_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define COUNT_UUID   "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

BLECharacteristic *countCharacteristic;
bool connected = false;
int count = 0;

// 接続・切断されたときに呼ばれる
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override {
    connected = true;
    Serial.println("connected");
  }
  void onDisconnect(BLEServer *server) override {
    connected = false;
    Serial.println("disconnected");
    BLEDevice::startAdvertising();  // 再び接続を受け付ける
  }
};

// LED のキャラクタリスティックに書き込まれたときに呼ばれる
class LedCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    std::string value = characteristic->getValue();
    if (value.length() > 0) {
      bool on = (value[0] != 0);  // 0 以外なら点灯
      digitalWrite(LED_PIN, on ? HIGH : LOW);
      Serial.println(on ? "LED ON" : "LED OFF");
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  BLEDevice::init("ESP32-yourname");  // スマホに表示される名前
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService *service = server->createService(SERVICE_UUID);

  // LED：スマホから書き込める
  BLECharacteristic *ledCharacteristic = service->createCharacteristic(
      LED_UUID, BLECharacteristic::PROPERTY_WRITE);
  ledCharacteristic->setCallbacks(new LedCallbacks());

  // カウンター：スマホから読める・通知できる
  countCharacteristic = service->createCharacteristic(
      COUNT_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  countCharacteristic->addDescriptor(new BLE2902());  // 通知に必要な設定

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();  // 周囲に存在を知らせる
  Serial.println("waiting for connection...");
}

void loop() {
  if (connected) {
    countCharacteristic->setValue(count);  // 値を更新する
    countCharacteristic->notify();         // スマホに通知する
    count++;
  }
  delay(1000);
}
```

## 動かしてみる

1. `BLEDevice::init("ESP32-yourname")` の名前を自分の名前などに変えてから書き込む
2. スマホで nRF Connect を開き、「SCAN」を押す
3. 一覧に表示された `ESP32-yourname` の「CONNECT」を押す
4. 「Unknown Service」（UUID が `4fafc201-...`）を開く

**LED を操作する**

1. UUID が `beb5483e-...` のキャラクタリスティックの、上向き矢印（Write）のアイコンを押す
2. 形式を `UINT8` にして `1` を送ると点灯、`0` を送ると消灯する

**カウンターを受け取る**

1. UUID が `1c95d5e3-...` のキャラクタリスティックの、下向き矢印が複数並んだアイコン（Notify）を押す
2. 値が 1 秒ごとに増えていくことを確認する

!!! todo "画像：nRF Connect の操作画面"

!!! mame "Bluetooth Classic と BLE"
    Bluetooth には、音楽などの大きなデータを連続して送るための **Bluetooth Classic** と、少量のデータを省電力でやり取りするための **BLE** の 2 種類があります。
    ESP32 は両方に対応していますが、iPhone は Bluetooth Classic のシリアル通信に対応していないので、スマホと通信する場合は BLE を使うのが一般的です。

!!! mame "アドバタイズ"
    BLE の機器は、接続される前から「自分はここにいる」という短い信号を周期的に発信しています。これを **アドバタイズ** と言います。スマホのスキャンで一覧に表示されるのは、この信号を受信しているためです。

## 発展課題

- [ ] カウンターの代わりに、可変抵抗の値を通知する
- [ ] スマホから `0`〜`255` の値を送り、LED の明るさを変える
- [ ] 送られてきた値が変わったときだけ、シリアルモニタに表示する

## うまくいかないとき

??? question "ビルドで `text section exceeds available space` のようなエラーが出る"
    プログラムが大きすぎて入りません。`platformio.ini` に `board_build.partitions = huge_app.csv` があるか確認してください。

??? question "スキャンしても表示されない"
    - スマホの Bluetooth と位置情報がオンになっているか確認してください（Android は、BLE のスキャンに位置情報の権限が必要です）
    - 一度切断したあとに表示されなくなった場合は、ESP32 の EN ボタンを押して再起動してください

## チェックポイント

!!! success "チェックポイント"
    スマホから LED を操作でき、カウンターの値がスマホに届いたら、先輩に見せて進捗表にチェックをもらってください。
