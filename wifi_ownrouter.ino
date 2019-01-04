#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266HTTPClient.h>;
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
}

void loop() {
  String maindata;
  String para[2];
  String temperature;
  String humidity;
  int commaindex;
  j = 0;
//char inData[11]; // Allocate some space for the string
//char inChar=-1; // Where to store the character read
//byte index = 0; // Index into array; where to store the character
 // char main[11];  
//  if (Serial.available()) {
//    //sprintf(main, "%d", Serial.read());
//    Serial.write(Serial.read());
//  }
//
//    while (Serial.available() > 0) // Don't read unless
//                                   // there you know there is data
//    {
//        if(index < 10) // One less than the size of the array
//        {
//            inChar = Serial.read(); // Read a character
//            inData[index] = inChar; // Store it
//            index++; // Increment where to write next
//            inData[index] = '\0'; // Null terminate the string
//        }
//    }
  //if(Serial.available()){
      maindata = Serial.readString();
  //  para = strtok(maindata,",");
//    for(int i=0;i<10;i++){
//      para[j][i-(j*4)] = dString(maindata,i);  
//    }
    commaindex = maindata.indexOf(",");   
    temperature = "maindata="+maindata.substring(0,commaindex);
    humidity = maindata.substring(commaindex+1,11); 
    //Serial.println(para[0]);
    //Serial.println(commaindex);
    Serial.println(temperature);
    Serial.println(humidity);
 // }
    
  if(WiFi.status()==WL_CONNECTED){
     
   // sprintf(main, "%s", Serial.read());
    //Serial.print(inData[0]);
    //Serial.write(maindata);
    
    HTTPClient http;
    http.begin("http://192.168.0.102:5000/api/post");
    //http.addHeader("Content-Type", "text/plain");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    //int httpCode = http.GET();
    int httpCode = http.POST(temperature);
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
    }
    else{
      Serial.println("Error in WiFi connection...");
    }
    delay(1000);
}
