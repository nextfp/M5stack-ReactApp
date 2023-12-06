#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h> // SPIFFS（ファイルシステム）用のライブラリをインクルード

const char *ssid = "Buffalo-G-E120";    // WiFiのSSID
const char *password = "5xxh6ig4ruepn"; // WiFiのパスワード

const int webSocketPort = 81;
WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

AsyncWebServer server(80);

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
  pinMode(36, INPUT);
  Serial.begin(115200);
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() > 10000)
    {
      Serial.println("Smart Config Start!");
      WiFi.beginSmartConfig();
      while (!WiFi.smartConfigDone())
      {
        if (millis() % 500 == 0)
        {
          Serial.print(".");
          delay(1);
        }
      }
      Serial.println("Smart Config Done!");
    }
    if (millis() % 500 == 0)
    {
      Serial.print(".");
      delay(1);
    }
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

  double message = (analogReadMilliVolts(36) - 142) * 0.077 - 115; // * 0.077
  String messageStr = String(message);
  webSocket.broadcastTXT(messageStr);
  M5.Display.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Display.println(message);
  delay(100);
}

// 2748 422