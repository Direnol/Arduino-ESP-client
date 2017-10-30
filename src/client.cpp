#include <SoftwareSerial.h>
#include <Arduino.h>

HardwareSerial &ESPport = Serial;
////////////////////// RX, TX

// если у вас MEGA, то закомментируйте верхние две строчки и раскомментируйте нижеследующую.
// ESP подключите к Serial1 (18, 19), скорость можно сделать 57600
// HardwareSerial & ESPport = Serial1;

void otvet_klienty(int ch_id);

String GetResponse(const String &AT_Command, unsigned long wait);

void clearSerialBuffer();

boolean connectWiFi(const String &NetworkSSID, const String &NetworkPASS);

void clearBuffer();

const int ledPin = 13;
uint8_t ledState = HIGH;
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];
String button;

void setup()
{
    pinMode(ledPin, OUTPUT);
    //Serial.begin(9600); // Терминал
    ESPport.begin(115200); // ESP8266
    clearSerialBuffer();

    GetResponse("AT+RST", 3400); // перезагрузка ESP
    GetResponse("AT+CWMODE=1", 300); // режим клиента
    connectWiFi("myrouter", "parolparol"); // подключаемся к домашнему роутеру (имя точки, пароль)
    GetResponse("AT+CIPMODE=0", 300); // сквозной режим передачи данных.
    GetResponse("AT+CIPMUX=1", 300); // multiple connection.

    GetResponse("AT+CIPSERVER=1,88", 300); // запускаем ТСР-сервер на 88-ом порту
    GetResponse("AT+CIPSTO=2", 300); // таймаут сервера 2 сек
    GetResponse("AT+CIFSR", 300); // узнаём адрес
    digitalWrite(ledPin, ledState);
}

///////////////////основной цикл, принимает запрос от клиента///////////////////
void loop()
{
    int ch_id, packet_len;
    char *pb;
    ESPport.readBytesUntil('\n', buffer, BUFFER_SIZE);

    if (strncmp(buffer, "+IPD,", 5) == 0) {
        sscanf(buffer + 5, "%d,%d", &ch_id, &packet_len);
        if (packet_len > 0) {
            pb = buffer + 5;
            while (*pb != ':') pb++;
            pb++;
            if ((strncmp(pb, "GET / ", 6) == 0) || (strncmp(pb, "GET /?", 6) == 0)) {
                clearSerialBuffer();

                if (ledState == LOW) {
                    ledState = HIGH;
                    button = "VKL";
                } else {
                    ledState = LOW;
                    button = "OTKL";
                }

                digitalWrite(ledPin, ledState);
                otvet_klienty(ch_id);
            }
        }
    }
    clearBuffer();
}

//////////////////////формирование ответа клиенту////////////////////
void otvet_klienty(int ch_id)
{
    String Header;

    Header = "HTTP/1.1 200 OK\r\n";
    Header += "Content-Type: text/html\r\n";
    Header += "Connection: close\r\n";

    String Content;

    Content = "<body><form action='' method='GET'><input type='submit' value='VKL/OTKL'> " + button;
    Content += "</form></body></html>";

    Header += "Content-Length: ";
    Header += (int) (Content.length());
    Header += "\r\n\r\n";

    ESPport.print("AT+CIPSEND="); // ответ клиенту
    ESPport.print(ch_id);
    ESPport.print(",");
    ESPport.println(Header.length() + Content.length());
    delay(20);

    if (ESPport.find(const_cast<char *>(">"))) {
        ESPport.print(Header);
        ESPport.print(Content);
        delay(200);
    }
}

/////////////////////отправка АТ-команд/////////////////////
String GetResponse(const String &AT_Command, unsigned long wait)
{
    String tmpData;

    ESPport.println(AT_Command);
    delay(wait);
    while (ESPport.available() > 0) {
        char c = static_cast<char>(ESPport.read());
        tmpData += c;

        if (tmpData.indexOf(AT_Command) > -1)
            tmpData = "";
        else
            tmpData.trim();

    }
    return tmpData;
}

//////////////////////очистка ESPport////////////////////
void clearSerialBuffer()
{
    while (ESPport.available() > 0) {
        ESPport.read();
    }
}

////////////////////очистка буфера////////////////////////
void clearBuffer()
{
    for (char &i : buffer) {
        i = 0;
    }
}

////////////////////подключение к wifi/////////////////////
boolean connectWiFi(const String &NetworkSSID, const String &NetworkPASS)
{
    String cmd = "AT+CWJAP=\"";
    cmd += NetworkSSID;
    cmd += "\",\"";
    cmd += NetworkPASS;
    cmd += "\"";

    ESPport.println(cmd);
    delay(6500);

}
