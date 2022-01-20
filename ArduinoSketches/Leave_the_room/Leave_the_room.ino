/*
   Upload this program to the ESP8288MOD.
   When you run this program you can press a button that will turn a LED light
   on or off. If you pass the echo-sensor (makes the distance from the sensor to
   an object less than 20 cm) and if the LED is on then the waiting time begins.
   The waiting time will be collected from the website (GET request).
   If you then pass the echo-sensor again nothing will happen, but if you don't
   and the waiting time is over then a message will be send to the serial
   monitor and to the website saying "You forogt to turn off the light".
   Then you have to pass by the echo-sensors again to turn off the light.

   This program is ment for you to remember to turn off the lighting in the room
   before you leave for a long period of time.

   Made by: Marie Sophie Mudge Woods s194384
   The client_lib.h file is made by August Valentin
*/
#include "client_lib.h" // connects/includes the header client_lib by August Valentin

//Change this according to the normal lighting in room
const int daylightInTheRoom = 780;

// Defines Echo pins (Gnd goes to ground and Vcc goes to 5V (under power))
const int trigPin = D3;
const int echoPin = D4;
//Defines LED and button
const int LED = D5;
const int BUT = D2;
// Defines variables
long duration;
int distance;
int analogValue;
int buttonState;
int waitingTime; //The wished time it's okay to have left the room with the light on

// Defines functions
void echoSensor () {
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
  distance = duration * 0.034 / 2;
}

void button () {
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


void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(LED, OUTPUT); // Sets the LED as an Output
  pinMode(BUT, INPUT_PULLUP); // Sets the button as an Input with pullup
  Serial.begin(9600); // Starts the serial communication
  digitalWrite(LED, LOW); //Start with the LED off

  //Sets up the internet
  String wName = "Galaxy A406D53";
  String wPass = "bzux7113";
  String IP = "192.168.43.222";
  setupWIFI(wName, wPass, IP);

}

void loop() {

  echoSensor (); // For the echo sensor

  button (); // For the button that controls the LED

  analogValue = analogRead(A0); // For the LDR (light sensor). Has to be before the if statements
  //  but after the defined functions


  if (distance <= 20) { // If the distance, the echo sensor measures, is equal of less than 20 cm
    //Check if the light is on
    if (analogValue > daylightInTheRoom) { //If the light is on
      Serial.println("You left while the light is on");
      delay(1000); // important to wait a second so the first signal doesn't get resieved again

      waitingTime = GETwaitTime() * 1200; //Makes a GET request to the server (function from header)
      //This equation follows the waitingTime: 
      // 50*waitingTime/1000/60 = min. => waitingTime = min. * 1200

      // Wait some time to see if the person comes back
      for (int i = 0; i <= waitingTime; i++) {
        echoSensor ();
        delay(50);
        if (distance <= 20) { //If the person enters the room again the loop will end/break
          Serial.println("You came back");
          delay(1000); // important to wait a second so the first signal doesn't get resieved again
          return;
        }
      }
      Serial.println("You forgot to turn off the light");
      POSTforgotLight(true); // Will post "You forgot to turn off the light" to the website
      
      // Waiting till the person comes back into the room
      echoSensor ();
      while (distance > 20) {
        echoSensor ();
      }
      Serial.println("You came back");
      POSTforgotLight(false); //Will delete the "You forgot to turn off the light" on the website
      delay(1000); // important to wait a second so the first signal doesn't get resieved again
    }
  }

}
