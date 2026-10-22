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
