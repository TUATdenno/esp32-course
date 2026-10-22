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
