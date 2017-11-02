#ifndef ARDUINO_ESP_CLIENT_ESP_H
#define ARDUINO_ESP_CLIENT_ESP_H

#include <Arduino.h>
#include <HardwareSerial.h>

#define WIFI_NAME "PORNO-A188"
#define WIFI_PASS "PIZDA702"
#define SERVER_ADDR "\"192.168.1.7\""
#define SERVER_PORT "5000"

#define WAIT_ESP 500

bool ESP_connectWifi(const String &name, const String &pass);
String ESP_responce(const String &command);
void ESP_init();
void ESP_clearbuf();
bool ESP_send(const char *msg);


#endif //ARDUINO_ESP_CLIENT_ESP_H
