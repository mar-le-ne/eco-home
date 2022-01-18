#include "client_lib.h"


// Pins:
const byte ledPin = D2;

// hall sensor used is WPSE313 (from WHADDA).
// Pin layout: When looking at the module's LED, left = GND, middle = +5V (3.3V also works), right = digital signal.
const byte magPin = D6; 

  


void setup() {
  // Set baud-rate of Serial connection(?)
  Serial.begin(115200);

  // Set pin modes:
  pinMode(ledPin,OUTPUT);
  pinMode(magPin,INPUT);
  
  String wName = "farligt wifi";
  String wPass = "august1234";
  String IP = "192.168.152.233";
  setupWIFI(wName, wPass, IP);
}


int xDelay = 15 * 1000;
int yDelay = 150;
unsigned long lastTime = 0;
void loop() { 
  delay(yDelay); // wait 0.15 seconds
  //test();
  if (!digitalRead(magPin)) { // magnet has inverse logic. i.e. True when no magnet is detected.
    digitalWrite(ledPin,HIGH);
    Serial.println("Magnet detected!");
  }
  else {
    digitalWrite(ledPin,LOW);
    Serial.println("No magnet detected...");
  }
  
  // Pattern for avoiding delays, but only run the function every x seconds. Making it possible to run other parts of the script every y seconds instead:
  if ((millis() - lastTime) > xDelay) {
    POSTfridge(!digitalRead(magPin));
    lastTime = millis();
  }
}
