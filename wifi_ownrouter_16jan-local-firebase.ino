#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>

//#define FIREBASE_HOST "green-tiger.firebaseio.com"
//#define FIREBASE_AUTH "W6xk9TE1LVmGhwzFNG8YZnZ1FMZLzfCucvHSdQ7n"

#define FIREBASE_HOST "spirit-iot-test.firebaseio.com"
#define FIREBASE_AUTH "wdNQYOzKXPZqIFcld8etSOALxhSvaOKNQUtV5RR2"

#define WIFI_SSID "amusemefirst"
#define WIFI_PASSWORD "?3502679?"

const char* ssid     = "amusemefirst";         // The SSID (name) of the Wi-Fi network you want to connect to
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
  String para[2];
  String temperature, humidity, pir, soundDB, co;
  String tFB, hFB, pFB, sFB, cFB;
  int commaindex, ci2, ci3, ci4,terminate;
  j = 0;
  
    maindata = Serial.readString();
    commaindex = maindata.indexOf(",");   
    ci2 = maindata.indexOf(",", commaindex+1);
    ci3 = maindata.indexOf(",", ci2+1);
    ci4 = maindata.indexOf(",", ci3+1);
    terminate = maindata.indexOf("T");
    
    temperature = "temperature="+maindata.substring(0,commaindex);
    tFB = maindata.substring(0,commaindex);
    
    humidity = "humidity="+maindata.substring(commaindex+1, ci2);
    hFB = maindata.substring(commaindex+1, ci2);
    
    pir = "pir="+maindata.substring(ci2+1, ci3);
    pFB = maindata.substring(ci2+1, ci3);
    
    soundDB = "soundDB="+maindata.substring(ci3+1, ci4);
    sFB = maindata.substring(ci3+1, ci4);
    
    co = "co="+maindata.substring(ci4+1, terminate);
    cFB = maindata.substring(ci4+1, terminate);
    
    Serial.println(temperature);
    Serial.println(humidity);
    Serial.println(pir);
    Serial.println(soundDB);
    Serial.println(co);
    
  if(WiFi.status()==WL_CONNECTED) {
    
    HTTPClient http;
    http.begin("http://192.168.0.102:5000/api/post");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(temperature);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();

    http.begin("http://192.168.0.102:5000/api/hmd");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode1 = http.POST(humidity);
    String payload_hmd = http.getString();
    Serial.println(httpCode1);
    Serial.println(payload_hmd);
    http.end();

    http.begin("http://192.168.0.102:5000/api/pir");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode2 = http.POST(pir);
    String payload_pir = http.getString();
    Serial.println(httpCode2);
    Serial.println(payload_pir);
    http.end();

    http.begin("http://192.168.0.102:5000/api/sound");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode3 = http.POST(soundDB);
    String payload_sound = http.getString();
    Serial.println(httpCode3);
    Serial.println(payload_sound);
    http.end();

    http.begin("http://192.168.0.102:5000/api/co");
    //http.addHeader("Content-Type", "text/plain");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode4 = http.POST(co);
    String payload_co = http.getString();
    Serial.println(httpCode4);
    Serial.println(payload_co);
    http.end();
    

  /// SEND TO FIREBASE

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = tFB;
  root["humidity"] = hFB; 
  root["pir"] = pFB; 
  root["soundDB"] = sFB; 
  root["co"] = cFB;  
  // append a new value to /logDHT
  String name = Firebase.push("/downtown", root);


        if (Firebase.failed()) {
            Serial.print("Firebase Pushing /downtown failed:");
            Serial.println(Firebase.error()); 
            // delay(500);             
            return;
        }else{
            Serial.print("Firebase Pushed /downtown success");
            Serial.println(name);
            // delay(500); 
        }

  } 
    else {
      Serial.println("Error in WiFi connection...");
    }
    delay(1000);
}
