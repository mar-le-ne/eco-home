#include <SoftwareSerial.h>
SoftwareSerial espSerial(5, 6);

const byte pot = A0;

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  pinMode(pot,INPUT);

}

void loop() {
  int sensorValue = getValue();
  // print out the value you read:
  Serial.println(sensorValue);
  espSerial.println(String(sensorValue));
  delay(1000);  
}

int getValue() { // Function to get the level of water
  float val = analogRead(pot);// Read sensor
  return val;
}
