#include "client_lib.h"


// Pins:
const byte ledPin = D2;
const byte buzzerPin = D5;

// hall sensor used is WPSE313 (from WHADDA).
// Pin layout: When looking at the module's LED, left = GND, middle = +5V (3.3V also works), right = digital signal.
const byte magPin = D6; 

  


void setup() {
  // Set baud-rate of Serial connection(?)
  Serial.begin(115200);
  Serial.println(""); // the ESP8266 prints some gibberish whenever it's reset (probably due to using another baudrate). 
  // To avoid our first printed message being on the same line, we print a newline.

  // Set pin modes:
  pinMode(buzzerPin,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(magPin,INPUT);
  
  String wName = "farligt wifi";
  String wPass = "august1234";
  String IP = "192.168.152.233";
  setupWIFI(wName, wPass, IP);
}


int xDelay = 15 * 1000; // 15 seconds
int yDelay = 250; // main loop is done every 0.25 seconds.


bool checkFridgeOpen(byte magPin) {
  // Return true if the fridge door is open, else false, using the magnet detector.
  bool isFridgeOpen = digitalRead(magPin);
  return isFridgeOpen;
}

void triggerBuzzer(bool trigger) {
  if (trigger) {
    tone(buzzerPin, 1000);
  }
  else {
    noTone(buzzerPin);
  }
}

bool alarmWasTriggered = false;
void fridgeAlarm(bool triggerAlarm) {
  digitalWrite(ledPin, triggerAlarm);
  triggerBuzzer(triggerAlarm);
  Serial.print("was alarm triggerd? "); Serial.println(triggerAlarm); 
  POSTfridge(triggerAlarm);
  alarmWasTriggered = triggerAlarm;

  // Was previously in the (!isFridgeOpen) if-statement block. Needs to be checked if it still works
  fridgeOpenDuration = 0; 
  fridgeOpenStart = mainLoop;
}


unsigned long fridgeOpenDuration = 0; // represents how long the fridge door has been open.
unsigned long fridgeOpenStart = 0; // represents the moment in time when the fridge was opened (same as being last time it was closed).
unsigned long mainLoop = 0; // represents the current time of the system. 
void fridgeWrapper(byte magPin) {
  // Function for wrapping the fridge functionality into a single neat package.
  // The fridge should be installed with the magnet detector inside the frdige, and a magnet on the fridge door.
  // The magnet detector has inverse logic, meaning it sends a HIGH signal when a magnet is NOT detected.
  // Therefore, the fridge door is open when the detector sends HIGH signal.

  /* If the fridge has been continuously open for some specific amount of time,
   *  We want to turn on the buzzer, the LED, and send a POST request to the server.
   *  A drawing of the logicflow can be found in the "Fridge" folder.
  */
  bool isFridgeOpen = checkFridgeOpen(magPin);
  if (!isFridgeOpen) {
    if (alarmWasTriggered) { // this is true only when xDelay time passed before fridge was closed. 
      fridgeAlarm(false);
    }
    
  }
  else {
    Serial.println("Fridge door is open!");
    fridgeOpenDuration = mainLoop - fridgeOpenStart;
  }
  // Pattern for avoiding delays, but only run the function every x seconds. Making it possible to run other parts of the script every y seconds instead:
  if ((fridgeOpenDuration > xDelay) && !(alarmWasTriggered)) { // if xDelay time has passed AND we haven't already sent the alarm signal.
    fridgeAlarm(true);
  }
  
}

void loop() { 
  delay(yDelay); // wait yDelay milliseconds
  mainLoop = millis();
  fridgeWrapper(magPin);
}
