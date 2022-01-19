/* Heater.h
 * 
   This script is called by "LeaveHouse.ino" and serves
   as an actuator for the heater uppon the
   exit or entrance in the house.

   An RGB LED is used to represent a heater.
   It will change colors to:
   - red - represent heater on;
   - blue - represent heater off.
  
 
   created 12/01/2022
   by Marlene Antunes
   last modified 19/01/2022
 */

// LED RGB pins
byte redPin   = D1;
byte greenPin = D2;
byte bluePin  = D3;

void setColor(int redVal, int greenVal, int blueVal) {
  /*
     Write color in the LED.
   */
  analogWrite(redPin,   redVal);
  analogWrite(greenPin, greenVal);
  analogWrite(bluePin,  blueVal);
}

void HeaterResponse(bool atHomeStatus) {
  /*
     According to the user's presence at home, given by "atHomeStatus",
      the RGB LED will shine red if the user left the house
      and blue if they returned.

      My RGB LED has an error in its building. It is not "RGB" but "BRG".
   */
  
  if (atHomeStatus == false) {                // left house
    Serial.println("See you soon!");
    Serial.println("Heater is being turned off now.");
    setColor(255, 0, 0);                // BRG - set color to blue
  }
  else if (atHomeStatus == true) {            // arrived house
    Serial.println("Welcome Back!");
    Serial.println("Heater is being turned on now.");
    setColor(0, 255, 0);                // BRG - set color to red
  }
  else {
    Serial.print("GEThome function might be returning a non-boolean value.");
  }
}
