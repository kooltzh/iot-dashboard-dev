#include "SoftwareSerial.h"

String ssid = "amusemefirst";  // The SSID (name) of the WiFi network
String pass = "?3502679?";     // The password of the WiFi network

String server = "192.168.0.102"; String port = "5000"; String uri = "/api/post";

char okString[] = "OK"; char errorString[] = "ERROR"; // can't be String (C++ error)

#define rxPin 6
#define txPin 7

SoftwareSerial espSerial(rxPin, txPin);  // RX pin, TX pin in Arduino Board
// * RX is digital pin 6 (connect to TX of ESP8266)
// * TX is digital pin 7 (connect to RX of ESP8266)

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  espSerial.begin(115200); // ESP8266 Serial Connection

  Serial.begin(9600); // Serial Monitor

  delay(500);

  closeServerConn(); // close connection to server if there is any existing connection

  espExec("AT+RST","ESP8266 Reset",500); // no status print
  espExec("AT","ESP8266 Status",500);
  espExec("AT+CWMODE=1","ESP8266 STA Mode Set",500);
  espExec("AT+CWMODE?","ESP8266 Mode Get",500);
  espExec("AT+CWJAP=\""+ssid+"\",\""+pass+"\"","ESP8266 WiFi Connect",5000); // no status print
  espExec("AT+CIFSR","ESP8266 IP Get",500);

  //openServerConn();
  
}

void loop() {
  delay(2500);
  httppost("maindata=30");
}

void espExec(String command, String desc, int timeDelay) {
  espSerial.println(command);
    if (espSerial.available()) {
      if( espSerial.find(okString) ) {
        Serial.println(desc + " [OK]");
      } else if ( espSerial.find(errorString) ) {
        Serial.println(desc + " [ERROR]");
      }
    } else {
      Serial.println("espSerial connection unavailable/dropped.");
    }
  delay(timeDelay);
}


void httppost (String postData) {
  openServerConn();
  
  String postRequest =
  "POST " + uri + " HTTP/1.0\r\n" +
  "Host: " + server + "\r\n" +
  "Accept: *" + "/" + "*\r\n" +
  "Content-Length: " + postData.length() + "\r\n" +
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  "\r\n" + postData;

  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  espSerial.print(sendCmd);
  espSerial.println(postRequest.length());

  delay(500);

  char arrowString[] = ">";
  if( espSerial.find(arrowString) ) {
    Serial.println("Sending postRequest..");
    //openServerConn();
    espSerial.print(postRequest);
    //closeServerConn();

    char sendOkString[] = "SEND OK";
    if( espSerial.find(sendOkString) ) {
      Serial.println("Packet sent");
      while (espSerial.available()) {
        String tmpResp = espSerial.readString();
        Serial.println(tmpResp);
      }
    }
  }
  closeServerConn();
}

void openServerConn() {
  // open the connection to server
  espExec("AT+CIPSTART=\"TCP\",\""+server+"\","+port,"ESP8266 to Server TCP Connect",500);
}

void closeServerConn() {
  // close the connection to server
  espExec("AT+CIPCLOSE","ESP8266 to Server TCP Disconnect",500);
}
