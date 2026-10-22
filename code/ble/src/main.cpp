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
