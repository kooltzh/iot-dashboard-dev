//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseRoom_ESP8266 is a sample that demo using multiple sensors
// and actuactor with the FirebaseArduino library.

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "spirit-iot-test.firebaseio.com"
#define FIREBASE_AUTH "wdNQYOzKXPZqIFcld8etSOALxhSvaOKNQUtV5RR2"
#define WIFI_SSID "amusemefirst"
#define WIFI_PASSWORD "?3502679?"
int j = 0;
//const int grovePowerPin = 15;
//const int vibratorPin = 5;
//const int lightSensorPin = A0;
//const int ledPin = 12;
//const int buttonPin = 14;
//const int fanPin = 13;

void setup() {
  Serial.begin(115200);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  String maindata;
  String temperature, humidity, pir, soundDB, co, timestamp;
  int commaindex, ci2, ci3, ci4,terminate;
  j = 0;
  
  maindata = Serial.readString();
    commaindex = maindata.indexOf(",");   
    ci2 = maindata.indexOf(",", commaindex+1);
    ci3 = maindata.indexOf(",", ci2+1);
    ci4 = maindata.indexOf(",", ci3+1);
    terminate = maindata.indexOf("T");
    temperature = maindata.substring(0,commaindex);
    humidity = maindata.substring(commaindex+1, ci2);
    pir = maindata.substring(ci2+1, ci3);
    soundDB = maindata.substring(ci3+1, ci4); 
    co = maindata.substring(ci4+1, terminate); //format of substring, (startIndex, length)
    timestamp = "12345678";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["timestamp"] = timestamp;
  root["temperature"] = temperature;
  root["humidity"] = humidity;  
  // append a new value to /logDHT
  String name = Firebase.push("/sensor", root);


        if (Firebase.failed()) {
            Serial.print("Firebase Pushing /sensor failed:");
            Serial.println(Firebase.error()); 
            delay(2000);             
            return;
        }else{
            Serial.print("Firebase Pushed /sensor/dht ");
            Serial.println(name);
            delay(2000); 
        }


//  digitalWrite(ledPin, Firebase.getInt("redlight"));
//  digitalWrite(fanPin, Firebase.getInt("cooldown"));
//  digitalWrite(vibratorPin, Firebase.getInt("brrr"));
//  int newButton = digitalRead(buttonPin);
//  if (newButton != button) {
//    button = newButton;
//    Firebase.setInt("pushbutton", button);
//  }
//  float newLight = analogRead(lightSensorPin);
//  if (abs(newLight - light) > 100) {
//    light = newLight;
//    Firebase.setFloat("sunlight", light);
//  }
  delay(200);
}
