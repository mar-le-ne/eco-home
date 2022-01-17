/* SendDataUno.ino
 * 
 * Program used in order to send data over serial communication 
 * to an ESP board.
 * 
 * Connect Rx pin of the Arduino to Tx pin of the ESP
 * Connect Tx pin of the Arduino to Rx pin of the ESP
 * NOTE: These wires should not be plugged in while uploading the code
 * Connect both boards to GND
 * 
 * created 13/01/2022
 * by Victor Leweke
 * last modified 14/01/2022
 */

#include <SoftwareSerial.h>

/* As an example, we will send data from 2 analog sensors:
 * A temperature sensor (LM35DZ) and a potentiometer
 */
const byte potSensor = A0;  
const byte tempSensor = A1;

SoftwareSerial espSerial(0, 1); // Select port for communication, by default Rx=0, Tx=1

void setup() {
  
  Serial.begin(115200);
  espSerial.begin(115200);      // Begin serial communication with ESP
}

void loop() {

  String temp = getTemp();  // For more convenient communication, only one string should be sent
  String potVal = getPot(); // So all functions should return a String 
  
  String str = temp+String("; ")+potVal; // Concatenate all strings into one
  espSerial.println(str);                // Send the string via serial
  delay(1000);
}

String getTemp() {                         // Get value from sensor and convert to temperature (in Celcius)
  float analogVal = analogRead(tempSensor);
  float milliV = (analogVal/1024.0)*5000;  // Convert to millivolts
  int temp = milliV/10;                    // Convert to temperature
  return String(temp);    
}

String getPot() {                          // Get value from potentiometer
  int potVal = analogRead(potSensor);
  return String(potVal);
}
