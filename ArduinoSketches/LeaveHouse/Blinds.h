/* Blinds.h

   This script is called by "LeaveHouse.ino" and serves
   as an actuator for the blinds uppon the
   exit or entrance in the house.

   To open/close the blinds, a step motor is used to pull a
   string attached to the blinds cloth.
   Depending on the direction of rotation, it will:
   - close the blinds if the user left the house;
   - open the blinds if the user arrived to the house.


   created 12/01/2022
   by Marlene Antunes
   last modified 19/01/2022
*/

// rotation pins
byte stepPin1 = D5;
byte stepPin2 = D6;
byte stepPin3 = D7;
byte stepPin4 = D8;
//
// initialization
int stepNumber = 1;
int counter = 0;

void AdvanceStepMotor(String dir) {        // dir declares rotation direction
  /*  Advance the motor in a low level manner, interacting directly
   *   with the 4 unipolar stepper coils.
   */
  if (dir == "clockwise") {
    switch (stepNumber) {                  // moving in order 1-2-3-4
      case 1:
        digitalWrite(stepPin1, HIGH); // 1
        digitalWrite(stepPin2, LOW);  // 0
        digitalWrite(stepPin3, LOW);  // 0
        digitalWrite(stepPin4, LOW);  // 0
        break;
      case 2:
        digitalWrite(stepPin1, LOW);  // 0
        digitalWrite(stepPin2, HIGH); // 1
        digitalWrite(stepPin3, LOW);  // 0
        digitalWrite(stepPin4, LOW);  // 0
        break;
      case 3:
        digitalWrite(stepPin1, LOW);  // 0
        digitalWrite(stepPin2, LOW);  // 0
        digitalWrite(stepPin3, HIGH); // 1
        digitalWrite(stepPin4, LOW);  // 0
        break;
      case 4:
        digitalWrite(stepPin1, LOW);  // 0
        digitalWrite(stepPin2, LOW);  // 0
        digitalWrite(stepPin3, LOW);  // 0
        digitalWrite(stepPin4, HIGH); // 1
        break;
    }
  } else if (dir == "counterclockwise") {
    switch (stepNumber) {
      case 1:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, HIGH);
        break;
      case 2:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, HIGH);
        digitalWrite(stepPin4, LOW);
        break;
      case 3:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, HIGH);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
        break;
      case 4:
        digitalWrite(stepPin1, HIGH);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
    }
  }
  else{
    Serial.println("Error in given direction of Step Motor.");
  }
  stepNumber++;            // advance step
  counter++;

  if (stepNumber > 4) {    // create rotation
    stepNumber = 1;
  }
}

void BlindsResponse(bool atHomeStatus) {
  /*  According to the user's presence at home, given by "atHomeStatus",
      the motor will advance a step clockwise "1" or counter-clockwise "0".

      The command to the motor will be given 7000 times 
      with a 2 ms interval between commands.
   */

  if (atHomeStatus == false) {                // left house
    for (int a = 0; a < 7000; a++) {
      AdvanceStepMotor("clockwise");           // perform motor step in clockwise direction
      delay(2);                                // 2ms between motor steps (speed)
    }
    Serial.println("See you soon!");
    Serial.println("Blinds are being closed now");
  }
  else if (atHomeStatus == true) {             // arrived house
    for (int a = 0; a < 7000; a++) {
      AdvanceStepMotor("counterclockwise");    // perform motor step in counter-clockwise direction
      delay(2);
    }
    Serial.println("Welcome Back!");
    Serial.println("Blinds are being opened now");
  }
  else {
    Serial.print("GEThome function might be returning a non-boolean value.");
  }
}
