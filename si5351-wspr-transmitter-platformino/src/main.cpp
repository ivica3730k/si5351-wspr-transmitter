#define LED_ON 0
#define LED_OFF 1
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
#ifdef BOARD_FAMILY
#if BOARD_FAMILY == ESP8266
    Serial.println("ESP8266");
#elif BOARD_FAMILY == AVR
    Serial.println("AVR");
#endif
#endif
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_OFF);
}
void loop()
{
    delay(1000);
}