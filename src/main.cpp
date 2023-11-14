#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h> // SPIFFS（ファイルシステム）用のライブラリをインクルード

// WebServer server(80); // ポート80でWebサーバーを開始
AsyncWebServer server(80);

void setup()
{
  M5.begin();
  M5.Power.begin();
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
}

void loop()
{
}