#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "Victor's iPhone X";
const char* pass = "a1b2c3d4";
WiFiClient client;

unsigned long channelID = 1629242; //your TS channal
const char * APIKey = "3MUXL34UG6G33BCM"; //your TS API
const char* server = "api.thingspeak.com";
const int postDelay = 16 * 1000; //post data every 16 seconds

const byte waterSensor = A0;
int waterLevel;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  pinMode(waterSensor,INPUT);

}

void loop() {
  int waterLevel = getLevel();    // Get the percentage of submerged sensor

  ThingSpeak.begin(client);
  client.connect(server, 80);               //connect(URL, Port)
  ThingSpeak.setField(1, waterLevel);       //set data on graph 1
  ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
  client.stop();

  Serial.print("Water level: ");
  Serial.print(waterLevel);       // Print on serial monitor
  Serial.println('%');
  
  delay(1000); //wait and then post again
}

int getLevel() { // Function to get the level of water
  float val = analogRead(waterSensor);                // Read sensor
  float level = pow(10, (val-67.807191)/166.096405);  // Convert value to a percentage
  return level;
}
