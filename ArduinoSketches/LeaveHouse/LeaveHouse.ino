/* LeaveHouse.ino
 * 
 * This script manages the actuations in the house uppon the 
 * user's exit or entrance in it.
 * 
 * The user updates the webpage "Your EcoHome" according 
 * to their presence at home.
 * A message is then given to the server and then received 
 *(get request) via wifi by the ESP.
 * The ESP also performs the respective actuations:
 * - Close/open blinds;
 * - Turn on/off the heater.
 * 
*/

#include "client_lib.h"
#include "Blinds.h"
#include "Heater.h"

int oldServerMessage = 0;

void setup() {

  Serial.begin(115200);

  // set pin modes:  heater
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // set pin modes: blinds
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(stepPin4, OUTPUT);

  // set wifi communication: connect to server
  String wName = "Swingerklubben-2";
  String wPass = "benjaminersmuk";
  String IP = "192.168.0.30";
  setupWIFI(wName, wPass, IP);
}

void loop() {
  delay(5000);   // check the server every 5s

  bool presenceAtHome = GEThome();            // GET from server if user is in/out of the house

  if (presenceAtHome != oldServerMessage) {   // avoid printing repetitive messages
    HeaterResponse(presenceAtHome);           // perform Heater On/Off
    BlindsResponse(presenceAtHome);           // perform Blinds Open/Close

    oldServerMessage = presenceAtHome;        // avoid printing repetitive messages
  }
}
