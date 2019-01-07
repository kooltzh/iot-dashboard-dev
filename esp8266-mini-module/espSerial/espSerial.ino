#include "SoftwareSerial.h"

#define rxPin 6
#define txPin 7

char okString[] = "OK";

SoftwareSerial espSerial(rxPin, txPin);  // RX pin, TX pin in Arduino Board
// * RX is digital pin 6 (connect to TX of ESP8266)
// * TX is digital pin 7 (connect to RX of ESP8266)

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  espSerial.begin(115200); // ESP8266 Serial Connection

  Serial.begin(9600); // Serial Monitor
}

void loop() {
  espSerial.println("AT");
    if (espSerial.available()) {
      if( espSerial.find(okString) ) Serial.println("ESP8266 Module OK");
    }
  delay(400);
}
