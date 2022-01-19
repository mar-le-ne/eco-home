/*
   This code is the fourth and final prototyping code for the program.
   It is now fully functionable with one echosensor.
   AND it is connected to the ESP to make POST and GET request.

   When the comment POST is used it means that this line of code is nessesary to post to the ESP
   When the comment GET is used it means that this line of code is nessesary for getting information
   from the ESP.
*/
//Set up to connect to the ESP8266MOD which has a WiFi connection
//#include <SoftwareSerial.h> //POST
//SoftwareSerial espSerial(0, 1); //POST: 0 (RX) and 1 (TX) are the pins for the communication on the Arduino
#include "client_lib.h"


//Change this according to the normal lighting in room
const int daylightInTheRoom = 780;
//Change this for the whished time it's okay to have left the room with the light on
//int waitingTime = 500; // The time that it's waiting is: 50*waitingTime/1000=seconds.
// So waitingTime = 1500 will be 1,25 min. 500 = 25 seconds

// Defines Echo pins (Gnd goes to ground and Vcc goes to 5V (under power))

// Define pins for sensors:
// Ultrasonic sensor
const byte echoPin = D5;
const byte trigPin = D7;
// Photocell sensor
const byte photocellPin = A0;

// Define pins for input(button) and LED:
const byte BUT = D4;
const byte LED = D2;

long duration;
int distance;
int analogValue;
int buttonState;


void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(LED, OUTPUT); // Sets the LED as an Output
  pinMode(BUT, INPUT_PULLUP); // Sets the button as an Input with pullup
  Serial.begin(9600); // Starts the serial communication
  digitalWrite(LED, LOW); //Start with the LED off

  String wName = "farligt wifi";
  String wPass = "august1234";
  String IP = "192.168.152.233";
  setupWIFI(wName, wPass, IP);

}

// Defines functions
void echoSensor () {  //Measures the distance from the echo sensor to where the waves get sent back
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance into centimeters
  distance = duration * 0.034 / 2.0;
}

bool checkPassing() {
  echoSensor();
  return (distance <= 20);
}

void button () {  //When the button change state (is pushed) it will change the state of the LED light (on/off)
  // read the pushbutton input pin:
  buttonState = digitalRead(BUT);
  // Change the state of the light if the button is pushed
  if (!buttonState) { // Read the value of the button. Due to pull-up resistor, the logic is reversed.
    // false = pressed, true = not pressed.
    digitalWrite(LED, !digitalRead(LED)); // Set the value of the led to the opposite of its current state
    while (buttonState == digitalRead(BUT)) { // while the button is pressed, be stuck in a loop until it is released.
      delay(50); // It is necessary to have some small delay in loops
    }
  }
}

//POST: Function that sends signal til ESP when you forget to turn off the light
//void sentMessageToESP_forgot () { //Sends a message to the ESP and also prints it
//  String str = "1"; //Define string
//  espSerial.println(str); //Tell the ESP to print the message
//  Serial.println(str); //Prints the message in the Serial Monitor
//}
//
////POST: Function that sends signal til ESP when you came back to the room after forgetting to turn off the light
//void sentMessageToESP_back () { //Sends a message to the ESP and also prints it
//  String str = "2"; //Define string
//  espSerial.println(str); //Tell the ESP to print the message
//  Serial.println(str); //Prints the message in the Serial Monitor
//}



int minutes2ms(int minutes) { // convert minutes to milliseconds.
  return minutes * 60 * 1000;
}

int GETmsWaitTime() {
  int waitingTime = GETwaitTime();
  if (waitingTime == 0) {
    return minutes2ms(1);
  }
  else {
    return minutes2ms(waitingTime);
  }
}

int readPhotoCell() {
  return analogRead(photocellPin);
}




unsigned long leavingTime = 0;
unsigned long leaveDuration = 0;
unsigned long currentTime = millis();

bool alarmWasTriggered = false;
void leaveAlarm(bool leaving) {
  alarmWasTriggered = leaving;
  POSTforgotLight(leaving);
  leavingTime = 0;
  leaveDuration = 0;
  String msg = leaving ? "You forgot the light" : "you've returned";
  Serial.println(msg);
}

int waitingTime = minutes2ms(1);
int minorDelay = 100;
bool isLeaving = false;
void loop() {
  delay(minorDelay);
  currentTime = millis(); // set the current time
  button (); // For the button that controls the LED
  bool isDetected = checkPassing();
  if (isDetected) { // If the distance, the echo sensor measures, is equal or less that 20 cm
    analogValue = readPhotoCell(); // For the LDR (light sensor). Has to be defore the if statements
    //Check if the light is on
    Serial.print("analog value is:"); Serial.println(analogValue);
    if (analogValue > daylightInTheRoom) { //If the light is on
      isLeaving = !isLeaving;

      if (isLeaving) {
        Serial.println("You left!");
        leavingTime = millis();
        leaveDuration = 0;
        // GET request to the server
        waitingTime = GETmsWaitTime();
        Serial.print("Waiting time is: "); Serial.println(waitingTime);
      }
      else {
        leaveAlarm(false);
      }
      while (checkPassing()) {  // Stay stuck in this loop until the person has passed by
          delay(minorDelay);
        }
    }
  }

  else if (isLeaving) {
    leaveDuration = currentTime - leavingTime;
  }

  if ((leaveDuration > waitingTime) && !(alarmWasTriggered)) {
    leaveAlarm(true);
  }
 

}
