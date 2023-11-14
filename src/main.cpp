#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h> // SPIFFS（ファイルシステム）用のライブラリをインクルード
#include <FS.h>

const int webSocketPort = 81;
WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

// WebServer server(80); // ポート80でWebサーバーを開始
AsyncWebServer server(80);

int counter = 0;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_CONNECTED:
    Serial.printf("WebSocket client connected: %u\n", num);
    break;
  case WStype_TEXT:
    // Handle text messages from WebSocket clients if needed
    break;
  case WStype_DISCONNECTED:
    Serial.printf("WebSocket client disconnected: %u\n", num);
    break;
  }
}

void setup()
{
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(10);
  M5.Lcd.setCursor(10, 10);
  int i = 1;
  Serial.begin(115200);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    if (i++ > 1)
    { // 10 seconds
      Serial.println("Smart Config Start!");
      WiFi.beginSmartConfig();
      while (!WiFi.smartConfigDone())
      {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Smart Config Done!");
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // SPIFFS（ファイルシステム）を初期化
  if (SPIFFS.begin())
  {
    Serial.println("SPIFFS initialized.");
  }
  else
  {
    Serial.println("Error initializing SPIFFS.");
  }
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false); });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();
  // Increase the counter value every second
  counter++;
  if (counter % 10000 == 0)
  {
    String message = String(counter / 10000);
    webSocket.broadcastTXT(message);
    M5.Display.clear();
    M5.Lcd.setCursor(10, 10);
    M5.Display.println(message);
  }
}