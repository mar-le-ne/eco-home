#include <SoftwareSerial.h>

const byte tempSensor = A0;
const byte potSensor = A1;

SoftwareSerial espSerial(5, 6);


void setup() {
  
  Serial.begin(115200);
  espSerial.begin(115200);

}

void loop() {

  String temp = getTemp();
  String potVal = getPot();
  String str = temp+String("; ")+potVal;
  espSerial.println(analogRead(potSensor));
  delay(1000);

  
}

String getTemp() {
  float analogVal = analogRead(tempSensor);
  float milliVolts = (analogVal / 1024.0) * 5000; // Convert to millivolts
  float temp = milliVolts/10; // Convert to temperature
  return String(temp);
}

String getPot() {
  float potVal = analogRead(potSensor);
  return String(potVal);
}
