#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
/*
const char* ssid = "Victor's iPhone X";
const char* pass = "a1b2c3d4";
WiFiClient client;

unsigned long channelID = 1629242; //your TS channal
const char * APIKey = "3MUXL34UG6G33BCM"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 20 * 1000;
*/
int arr[2] = {0, 0};
void setup() {

  Serial.begin(115200);
  //WiFi.begin(ssid, pass);
  while (!Serial) { // wait for serial port to connect. 
    ;
  }
}


void loop() {  

  if (Serial.available() > 3) {
    for (int i = 0; i<2; i++) {
      arr[i] = (int)Serial.parseInt();
      Serial.println(arr[i]);
    }
  }
  /*// Write data on Thingspeak
  ThingSpeak.begin(client);
  client.connect(server, 80); 
  ThingSpeak.setField(1, arr[0]); 
  ThingSpeak.setField(2, arr[1]); 
  ThingSpeak.writeFields(channelID, APIKey);
  client.stop();
  delay(postDelay);
  */
  delay(1000);
}
