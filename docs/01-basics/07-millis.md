# 7. delay を使わない書き方

## このステップでやること

LED の点滅とボタンの読み取りを **同時に** 行うプログラムを作ります。
`delay()` を使わずに時間を扱う書き方は、複数の部品を組み合わせて何かを作るときに必ず必要になります。

## 前提ステップ・使う部品

- 前提：[4. ボタン入力](04-button.md)
- 部品：ステップ4の回路（LED とタクトスイッチ）をそのまま使います

## 配線

ステップ4と同じです。

| ESP32 | 接続先 |
|---|---|
| GPIO13 | 抵抗 220 Ω → LED → GND |
| GPIO4 | タクトスイッチ → GND |

## delay の問題点

「LED を 0.5 秒ごとに点滅させながら、ボタンが押されたら点滅の速さを変える」プログラムを、`delay()` で書いてみます。

```cpp
void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  if (digitalRead(BUTTON_PIN) == LOW) {
    // 点滅の速さを変える
  }
}
```

`delay(500)` の間、ESP32 は **何もせずに待っている** ので、ボタンを読み取るのは 1 秒に 1 回だけになります。タイミングよく押さないと反応しません。

## millis で時間を管理する

`millis()` は、電源が入ってからの経過時間をミリ秒で返す関数です。これを使い、「前回 LED を切り替えてから 500 ms 経ったか」を毎回確認します。
待たずにすぐ `loop()` の次の処理に進むので、その間にボタンの読み取りなど別の処理を行えます。

```cpp title="src/main.cpp"
#include <Arduino.h>

const int LED_PIN = 13;
const int BUTTON_PIN = 4;

unsigned long lastBlink = 0;  // 最後に LED を切り替えた時刻
unsigned long interval = 500; // 点滅の間隔（ms）
bool ledOn = false;

bool lastState = HIGH;
unsigned long lastPress = 0;  // 最後にボタンが押された時刻

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();

  // 処理1：interval ごとに LED を切り替える
  if (now - lastBlink >= interval) {
    lastBlink = now;
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  }

  // 処理2：ボタンが押されたら点滅の速さを変える
  bool state = digitalRead(BUTTON_PIN);
  if (lastState == HIGH && state == LOW && now - lastPress > 50) {  // 50 ms 以内の変化はチャタリングとして無視
    lastPress = now;
    interval = (interval == 500) ? 100 : 500;
    Serial.printf("interval = %lu ms\n", interval);
  }
  lastState = state;
}
```

`loop()` は 1 秒間に数十万回のペースで繰り返されます。その中で、処理1も処理2も「やるべきときが来ていればやる、来ていなければ何もしない」という書き方になっています。こうすることで、複数の処理が同時に進んでいるように動作します。

チャタリング対策も、`delay(50)` ではなく「前回押されてから 50 ms 経っているか」で判定しています。

!!! mame "millis の桁あふれ"
    `millis()` の値は `unsigned long`（32 ビット）なので、約 49.7 日で最大値を超えて 0 に戻ります。
    `now - lastBlink >= interval` のように **引き算で経過時間を求める** 書き方をしておけば、0 に戻ったときも符号なし整数の性質で正しく計算されます。`now >= lastBlink + interval` のように書くと、0 に戻ったときに誤動作します。

!!! mame "状態遷移"
    「点灯中」「消灯中」のように、プログラムがとりうる状態と、状態が切り替わる条件で動作を設計する考え方を **状態遷移**（ステートマシン）と言います。
    信号機や自動販売機のように、決まった手順で動く機械の多くはこの考え方で設計されています。複雑な動作を作るときに役立ちます。

## 発展課題

- [ ] LED をもう1つ追加し（GPIO14 など）、2つの LED を別々の間隔（300 ms と 700 ms）で点滅させる
- [ ] ボタンを押すたびに、点滅の間隔が 500 → 250 → 100 → 500 … と変わるようにする
- [ ] 5 秒ごとに、シリアルモニタに経過時間を表示する

## うまくいかないとき

??? question "ボタンを押しても反応しない・反応が悪い"
    - `loop()` の中に `delay()` が残っていないか確認してください
    - [4. ボタン入力](04-button.md) の配線を確認してください

## チェックポイント

!!! success "チェックポイント"
    LED が点滅している間も、ボタンを押すとすぐに点滅の速さが変わったら、先輩に見せて進捗表にチェックをもらってください。
