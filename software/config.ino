#include "config.h"



void config() {
  Serial.begin(115200);
  delay(100);
  debug("Welcome");

  if (!SPIFFS.begin()) {
    debug("SPIFFS.begin: ERR");
    return;
  } else {
    debug("SPIFFS.begin: OK");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.hostname("ESP8266");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debug("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  initWebSocket();
  web();
  asyncOTA();
  server.begin();

  debug("Config finish");
}