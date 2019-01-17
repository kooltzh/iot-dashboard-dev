#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "spirit-iot-test.firebaseio.com"
#define FIREBASE_AUTH "wdNQYOzKXPZqIFcld8etSOALxhSvaOKNQUtV5RR2"

const char* ssid     = "amusemefirst";  // The SSID (name) of the Wi-Fi network
const char* password = "?3502679?";     // The password of the Wi-Fi network

int j = 0;
void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  String maindata;
  String tFB, hFB, pFB, sFB, cFB;
  int commaindex, ci2, ci3, ci4, terminate;
  j = 0;

  maindata = Serial.readString();
  commaindex = maindata.indexOf(",");
  ci2 = maindata.indexOf(",", commaindex + 1);
  ci3 = maindata.indexOf(",", ci2 + 1);
  ci4 = maindata.indexOf(",", ci3 + 1);
  terminate = maindata.indexOf("T");

  tFB = maindata.substring(0, commaindex);
  hFB = maindata.substring(commaindex + 1, ci2);
  pFB = maindata.substring(ci2 + 1, ci3);
  sFB = maindata.substring(ci3 + 1, ci4);
  cFB = maindata.substring(ci4 + 1, terminate);

  Serial.println(tFB);
  Serial.println(hFB);
  Serial.println(pFB);
  Serial.println(sFB);
  Serial.println(cFB);


  if (tFB.startsWith("T") || hFB.startsWith("T") || pFB.startsWith("T") || sFB.startsWith("T") || cFB.startsWith("T") ||
      tFB.endsWith(",") || hFB.endsWith(",") || pFB.endsWith(",") || sFB.endsWith(",") || cFB.endsWith(",")) { // if data is corrupted
    // skip sending to firebase
  }
  else { // if data is Okay
    if (WiFi.status() == WL_CONNECTED) {

      /// SEND TO FIREBASE
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["temperature"] = tFB;
      root["humidity"] = hFB;
      root["pir"] = pFB;
      root["soundDB"] = sFB;
      root["co"] = cFB;
      String name = Firebase.push("/downtown", root);

      if (Firebase.failed()) {
        Serial.print("Firebase Pushing /downtown failed:");
        Serial.println(Firebase.error());
        return;
      } else {
        Serial.print("Firebase Pushed /downtown success:");
        Serial.println(name);
      }
    }
    else { // if(WiFi.status()!=WL_CONNECTED)
      Serial.println("Error in WiFi connection...");
    }
    delay(100);
  }
} // end of void loop()
