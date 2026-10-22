# ESP-NOW

## このステップでやること

ESP-NOW という通信方式で、2 台の ESP32 を直接つなぎます。送信側のボタンを押すと、受信側の LED が切り替わるようにします。
Wi-Fi ルーターを使わずに ESP32 同士で通信できるので、ラジコンのコントローラーなどに向いています。

!!! todo "画像：2 台の ESP32 で、ボタンを押すともう一方の LED が切り替わる様子"

## 前提ステップ・使う部品

- 前提：第1部（特に [4. ボタン入力](../01-basics/04-button.md)）
- 部品：
    - ESP32 ボード × **2**
    - 送信側：タクトスイッチ × 1（GPIO4 → タクトスイッチ → GND）
    - 受信側：ステップ2の LED の回路（GPIO13 → 抵抗 → LED → GND）

!!! tip "2 人で組むと進めやすい"
    ボードが 2 台必要なので、ほかの受講者とペアになって、送信側と受信側を分担すると効率よく進められます。

## ESP-NOW とは

**ESP-NOW** は、Espressif が開発した ESP32 同士の通信方式です。Wi-Fi と同じ電波を使いますが、アクセスポイントにつながる手順を省いて、相手に直接データを送ります。

| | Wi-Fi（Web サーバ） | BLE | ESP-NOW |
|---|---|---|---|
| 通信相手 | スマホ・PC など | スマホなど | ESP32 同士 |
| 接続までの時間 | 数秒 | 数秒 | ほぼ 0（すぐ送れる） |
| 遅延 | 大きめ | 中くらい | 小さい（数 ms） |
| 1 回に送れるデータ | 大きい | 小さい | 250 バイトまで |

## データの送り方

この講座では、**ブロードキャスト**（周囲のすべての ESP32 に向けて送る方法）を使います。相手の MAC アドレスを調べる必要がないので、手軽に試せます。

ただし、部室で何組も同時に試すと、ほかの組のデータも受信してしまいます。そこで、送るデータに **グループ番号** を入れ、受信側は自分と同じ番号のデータだけを使うようにします。

送るデータは、構造体にまとめます。送信側と受信側で **まったく同じ構造体** を使う必要があります。

```cpp
struct Message {
  uint8_t group;  // グループ番号（ほかの組と重ならない番号にする）
  bool ledOn;     // LED を点灯するか
};
```

## 送信側のコード

```cpp title="sender/src/main.cpp"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

const int BUTTON_PIN = 4;
const uint8_t GROUP = 1;  // ペアの相手と同じ番号にする

struct Message {
  uint8_t group;
  bool ledOn;
};

// FF:FF:FF:FF:FF:FF はブロードキャスト（全員宛て）のアドレス
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bool lastState = HIGH;
bool ledOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);  // ESP-NOW を使うには Wi-Fi を有効にする必要がある
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  // 送信先を登録する
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void loop() {
  bool state = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && state == LOW) {
    ledOn = !ledOn;

    Message message;
    message.group = GROUP;
    message.ledOn = ledOn;
    esp_now_send(broadcastAddress, (uint8_t *)&message, sizeof(message));

    Serial.println(ledOn ? "send: ON" : "send: OFF");
    delay(50);  // チャタリング対策
  }

  lastState = state;
}
```

## 受信側のコード

```cpp title="receiver/src/main.cpp"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

const int LED_PIN = 13;
const uint8_t GROUP = 1;  // 送信側と同じ番号にする

struct Message {
  uint8_t group;
  bool ledOn;
};

// データを受信したときに呼ばれる
void onReceive(const uint8_t *mac, const uint8_t *data, int length) {
  if (length != sizeof(Message)) {
    return;  // 大きさが違うデータは無視する
  }

  Message message;
  memcpy(&message, data, sizeof(message));

  if (message.group != GROUP) {
    return;  // ほかのグループのデータは無視する
  }

  digitalWrite(LED_PIN, message.ledOn ? HIGH : LOW);
  Serial.printf("receive from %02X:%02X:%02X:%02X:%02X:%02X: %s\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                message.ledOn ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);  // 受信したときに呼ぶ関数を登録する
}

void loop() {
  // 受信は onReceive で行うので、ここでは何もしない
}
```

受信側では、データが届くたびに `onReceive()` が自動で呼ばれます。このように、何かが起きたときに呼ばれるよう登録しておく関数を **コールバック関数** と言います。

!!! mame "MAC アドレス"
    ネットワークにつながる機器それぞれに割り振られた、世界で 1 つの番号です（`24:6F:28:AB:CD:EF` のような形）。
    ブロードキャストではなく特定の相手だけに送りたい場合は、相手の MAC アドレスを `broadcastAddress` の代わりに指定します。ESP32 の MAC アドレスは `WiFi.macAddress()` で調べられます。

!!! warning "Arduino core のバージョンによる違い"
    ESP-NOW の受信のコールバック関数の引数は、Arduino core のバージョン 3 以降で変わっています。インターネットで調べたサンプルがビルドできない場合は、どのバージョン向けのコードかを確認してください。この講座のコードは、`platformio.ini` で指定しているバージョン（Arduino core 2 系）向けです。

## 発展課題

- [ ] 送信側に可変抵抗を追加し、その値を送って受信側の LED の明るさを変える（構造体にメンバを追加します）
- [ ] 受信側からも送信できるようにして、双方向で通信する
- [ ] 一定時間データが届かなかったら、受信側の LED を点滅させる（通信が切れたことを知らせる）

## うまくいかないとき

??? question "受信側が反応しない"
    - 送信側と受信側の `GROUP` が同じか確認してください
    - 送信側と受信側の `Message` 構造体が、まったく同じか確認してください
    - 送信側のシリアルモニタに `send: ON` と表示されているか確認してください

??? question "ほかの人のボタンでも LED が切り替わる"
    `GROUP` の番号がほかの組と重なっています。重ならない番号に変えてください。

## チェックポイント

!!! success "チェックポイント"
    送信側のボタンを押すたびに、受信側の LED が切り替わったら、先輩に見せて進捗表にチェックをもらってください。
