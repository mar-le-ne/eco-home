#include "client_lib.h"

// LED RGB pins
byte redPin   = D1;
byte greenPin = D2;
byte bluePin  = D3;

// BUTTON pin
byte button = D0;

// init
int on_off = 0;
unsigned long lastTime = 0;

void setup() {

  Serial.begin(115200);

  // set pin modes
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(button, INPUT);

  // set wifi com
  String wName = "Swingerklubben-2";
  String wPass = "benjaminersmuk";
  String IP = "192.168.0.30";
  setupWIFI(wName, wPass, IP);
}

void loop() {

  // GET if in/out of the home
  String presenceAtHome = GEThome();
  String sub_presenceAtHome = presenceAtHome.substring(1, 2);
  Serial.print("presenceAtHome: ");
  Serial.println(sub_presenceAtHome);
  

  if (sub_presenceAtHome == "f") {       // (false) left the house
    Serial.println("left the house ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    // GRB
    setColor(0, 0, 255); // cool down
    Serial.println("Heater is going to be turned off now [blue]");
    delay(5000);
  }
  else if (sub_presenceAtHome == "t") {  // (true) arrived home
    Serial.println("left the house ________________________________________________________");
    setColor(0, 255, 0); // warm up
    Serial.println("Heater is being turned on now [red]");
    delay(5000);
  }
  else{
    Serial.print("backfireeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
  }

  if (digitalRead(button) == HIGH) {    // pressed button
    Serial.println("button clicked");
    switch (on_off) {
      // GRB
      case 0:
        setColor(0, 0, 255); // cool down
        Serial.println("Heater is going to be turned off now [blue]");
        on_off++;
        delay(500);
        break;

      case 1:
        setColor(0, 255, 0); // warm up
        Serial.println("Heater is being turned on now [red]");
        delay(500);
        on_off = 0;
        break;

      default:
        on_off = 0;
    }
  }


  // Pattern for avoiding delays, but only run the function every x seconds.
  // Making it possible to run other parts of the script every y seconds instead:

  //  if ((millis() - lastTime) > 2000) {
  //    POSTheater(!digitalRead(magPin));
  //    lastTime = millis();
  //  }
}

void setColor(int redVal, int greenVal, int blueVal) {
  analogWrite(redPin,   redVal);
  analogWrite(greenPin, greenVal);
  analogWrite(bluePin,  blueVal);
}
