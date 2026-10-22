#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const int LED_PIN = 13;

// ほかの人と重ならない名前にする
const char *SSID = "ESP32-yourname";
const char *PASSWORD = "esp32pass";  // 8 文字以上

WebServer server(80);  // 80 番ポートで待ち受ける
bool ledOn = false;

// ブラウザに返す HTML を作る
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 LED</title>
  <style>
    body { font-family: sans-serif; text-align: center; }
    a { display: inline-block; margin: 8px; padding: 16px 32px;
        color: white; background: #c62828; border-radius: 8px; text-decoration: none; }
  </style>
</head>
<body>
  <h1>ESP32 LED</h1>
)rawliteral";
  html += "<p>LED: ";
  html += ledOn ? "ON" : "OFF";
  html += "</p>";
  html += R"rawliteral(
  <a href="/on">ON</a>
  <a href="/off">OFF</a>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", makePage());
}

void handleOn() {
  ledOn = true;
  digitalWrite(LED_PIN, HIGH);
  server.sendHeader("Location", "/");  // トップページに戻す
  server.send(303);
}

void handleOff() {
  ledOn = false;
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.softAP(SSID, PASSWORD);  // AP モードで Wi-Fi を始める
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());  // 通常は 192.168.4.1

  // URL ごとに、呼び出す関数を登録する
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
}

void loop() {
  server.handleClient();  // ブラウザからのリクエストを処理する
}
