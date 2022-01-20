/* WaterSensor.ino
 * 
 * Program used to read the water sensor
 * 
 * Connect S pin of the sensor to an analog pin
 * Connect + pin of the sensor to a digital pin
 * Connect both boards to GND
 * 
 * created 12/01/2022
 * by Victor Leweke
 * last modified 18/01/2022
 */

#include "client_lib.h"

const byte powerPin = D1;     // Define a power pin so that the sensor is not always powered up
const byte waterSensor = A0;  // Define the analog pin
int waterLevel;
bool infoSent = true;         // Boolean for the information sent

// waitingTime inspired by Sophie Woods
const int waitingTime = 300; // The time that it's waiting is: 50*waitingTime/1000=seconds.
// So waitingTime = 1500 will be 1,25 min. 500 = 25 seconds
int timeCount = 0;           // Time counter

void setup() {
  Serial.begin(115200);
  String wName = "Wifi";
  String wPass = "Pass";
  String IP = "172.16.129.160";
  setupWIFI(wName, wPass, IP);
  
  pinMode(powerPin, OUTPUT);
  pinMode(waterSensor, INPUT);
}

void loop() {
  int waterLevel = getLevel();    // Get the percentage of submerged sensor
  Serial.print("Water level: ");
  Serial.println(waterLevel);     // Print on serial monitor  
  delay(1000);
  
  while (waterLevel > 120) {      // If the water level is high for too long, the user will be notified
    delay(50);
    timeCount++;
    waterLevel = getLevel();      // Update the water level
    if (timeCount >= waitingTime) {
      infoSent = true;
      Serial.println("1");
      POSTfaucet(true);           // Posting on the server
      timeCount = 0;
    } 
  }
  timeCount = 0;
  if (infoSent == true) {         // Checks whether a different information was sent
    infoSent = false;             // so that it does not sent repetitive requests
    Serial.println("0");
    POSTfaucet(false);            // Posting on the server
  }
}

int getLevel() {  // Function to get the level of water
  digitalWrite(powerPin, HIGH);
  delay(10);
  float level = analogRead(waterSensor);   // Read sensor
  delay(10);
  digitalWrite(powerPin, LOW);
  return level;
}
