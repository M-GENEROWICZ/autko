#include <WebSocketsServer.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>  // This is the base header
#include <LittleFS.h>  
#include <ESP8266WebServer.h>
// #include <ESPAsyncTCP.h>
#include <ElegantOTA.h>
#include "config.h"
#include "program.h"
#include "web.h"
#include <ArduinoJson.h>
#include <Servo.h>


unsigned long previousTask1 = 0;
unsigned long previousTask2 = 0;
Servo myServo;


void setup() {
  config();
  myServo.attach(13);

}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousTask1 >= 10) {  // co 10 ms (przeważnie dla cleanupów, wypisywania na serial)
    previousTask1 = currentMillis;
    Task1code();
  }

  if (currentMillis - previousTask2 >= 100) { // co 100 ms dla Task2
    previousTask2 = currentMillis;
    Task2code();
  }
}