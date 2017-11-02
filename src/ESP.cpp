#include "../headers/ESP.h"

HardwareSerial &ESP = Serial;

String ESP_responce(const String &command)
{
    ESP.println(command);
    delay(WAIT_ESP);
    String res;
    while (ESP.available() > 0) {
        res += static_cast<char> (ESP.read());
    }
    res.trim();
    return res;

}
void ESP_init()
{
    ESP.begin(115200);
    ESP_clearbuf();
    Serial.println(ESP_responce("AT+RST")); // reset ESP
    ESP_responce("AT+RESTORE");
    ESP_responce("AT+CWMODE=1"); // connect as client
    ESP_responce("AT+CIPMODE=0");
    ESP_connectWifi(WIFI_NAME, WIFI_PASS);
}

bool ESP_connectWifi(const String &name, const String &pass)
{

    String connect = "AT+CWJAP=\"";
    connect += name + "\",\"";
    connect += pass + "\"";
    ESP_responce(connect);
    ESP_responce("AT+CIPMUX=0");
    connect = "AT+CIPSTART=\"TCP\",";
    connect += SERVER_ADDR;
    connect += ",";
    connect += SERVER_PORT;
    return ESP_responce(connect).indexOf("OK") >= 0;
}

bool ESP_send(const char *msg)
{
    String com = "AT+CIPSEND=";
    com += String(strlen(msg));
    ESP_responce(com);
    delay(WAIT_ESP);
    return ESP_responce(msg).indexOf("OK") >= 0;
}

void ESP_clearbuf()
{
    while (ESP.available() > 0) {
        ESP.read();
    }
}
