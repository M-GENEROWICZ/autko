#include "web.h"
#include "config.h"

void notifyClients() {
  String message = String(VIBRATION);
  webSocket.broadcastTXT(message);
}

void handleWebSocketMessage(uint8_t num, uint8_t *payload, size_t length) {
  String message = String((char*) payload);
  Serial.println(message);

  // Spróbuj najpierw sparsować JSON
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, message);

  if (!error) {
    String type = doc["type"];
    String key = doc["key"];

    if (type == "joystick" && key == "left-joystick-move") {
      float x = doc["x"]; // joystick lewo-prawo od -1.0 do 1.0

      // Mapowanie wartości x (-1.0 do 1.0) na kąt serwa (0 do 180 stopni)
      int angle = map(x * 100, -100, 100, 0, 180);

      angle = constrain(angle, 0, 180);

      myServo.write(angle);

      Serial.print("Joystick X: ");
      Serial.print(x);
      Serial.print(" -> Angle: ");
      Serial.println(angle);
      return;
    }
  }

  // Jeśli to nie JSON - obsługa starych prostych komunikatów
  if (message.startsWith("LSB__")) LEFT_SHOULDER_BOTTOM = message.substring(5).toFloat();
  if (message.startsWith("RSV__")) RIGHT_SHOULDER_BOTTOM = message.substring(5).toFloat();
  if (message.startsWith("LAS_X")) LEFT_ANALOG_STICK_X = message.substring(5).toFloat();
  if (message.startsWith("LAS_Y")) LEFT_ANALOG_STICK_Y = message.substring(5).toFloat();
  if (message.startsWith("RAS_X")) RIGHT_ANALOG_STICK_X = message.substring(5).toFloat();
  if (message.startsWith("RAS_Y")) RIGHT_ANALOG_STICK_Y = message.substring(5).toFloat();
  
  if (message.startsWith("F1___")) FACE_1 = message.substring(5).toInt();
  if (message.startsWith("F2___")) FACE_2 = message.substring(5).toInt();
  if (message.startsWith("F3___")) FACE_3 = message.substring(5).toInt();
  if (message.startsWith("F4___")) FACE_4 = message.substring(5).toInt();
  
  if (message.startsWith("D1___")) DPAD_UP = message.substring(5).toInt();
  if (message.startsWith("D2___")) DPAD_DOWN = message.substring(5).toInt();
  if (message.startsWith("D3___")) DPAD_LEFT = message.substring(5).toInt();
  if (message.startsWith("D4___")) DPAD_RIGHT = message.substring(5).toInt();

  if (message.startsWith("LS___")) LEFT_SHOULDER = message.substring(5).toInt();
  if (message.startsWith("RS___")) RIGHT_SHOULDER = message.substring(5).toInt();
  if (message.startsWith("LAB__")) LEFT_ANALOG_BUTTON = message.substring(5).toInt();
  if (message.startsWith("RAB__")) RIGHT_ANALOG_BUTTON = message.substring(5).toInt();
  
  if (message.startsWith("ST___")) START = message.substring(5).toInt();
  if (message.startsWith("SL___")) SELECT = message.substring(5).toInt();
  if (message.startsWith("H____")) HOME = message.substring(5).toInt();
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("WebSocket client #%u connected\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client #%u disconnected\n", num);
      break;
    case WStype_TEXT:
      handleWebSocketMessage(num, payload, length);
      break;
    default:
      break;
  }
}

void initWebSocket() {
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

String processor(const String& var) {
  Serial.println(var);
  return String();
}

void web() {
  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/Joycon.js", HTTP_GET, []() {
    File file = LittleFS.open("/Joycon.js", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "application/javascript");
    file.close();
  });
}



