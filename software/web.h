#ifndef WEB_H_
#define WEB_H_

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

extern ESP8266WebServer server;
extern WebSocketsServer webSocket;

void web();
String processor(const String& var);

extern String message;

void notifyClients();
void handleWebSocketMessage(uint8_t num, uint8_t *data, size_t len);
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void initWebSocket();

#endif