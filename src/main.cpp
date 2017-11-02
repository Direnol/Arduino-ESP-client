#include "../headers/ESP.h"

void setup()
{
    ESP_init();
}


void loop()
{
    String buf(rand());
    const char *msg = buf.c_str();
    ESP_send(msg);
}
