/*
   For connection with ESP and Arduino boards.
   This should be uploaded to the Arduino.
   
   This is for POST requests.

   By Marie Sophie Mudge Woods s194384 
   The POST requests addings are made my Marie Sophie Mudge Woods and August Valentin
*/
//Set up to connect to the ESP8266MOD with has a WiFi connection
#include <SoftwareSerial.h>
SoftwareSerial espSerial(0, 1); //0 and 1 are the pins for the communication on the Arduino

//Change this according to the normal lighting in room
const int daylightInTheRoom = 780;
//Change this for the whished time it's okay to have left the room with the light on
const int waitingTime = 500; // The time that it's waiting is: 50*waitingTime/1000=seconds.
// So waitingTime = 1500 will be 1,25 min. 500 = 25 seconds

// Defines Echo pins (Gnd goes to ground and Vcc goes to 5V (under power))
const int trigPin = 11;
const int echoPin = 12;
//Defines LED and button
const int LED = 13;
const int BUT = 10;
// Defines variables
long duration;
int distance;
int analogValue;
int buttonState;

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
  distance = duration * 0.034 / 2;
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

//Function that sends signal til ESP when you forget to turn off the light
void sentMessageToESP_forgot () { //Sends a message to the ESP and also prints it
  String str = "1"; //Define string
  espSerial.println(str); //Tell the ESP to print the message
  Serial.println(str); //Prints the message in the Serial Monitor
}

//Function that sends signal til ESP when you came back to the room after forgetting to turn off the light
void sentMessageToESP_back () { //Sends a message to the ESP and also prints it
  String str = "2"; //Define string
  espSerial.println(str); //Tell the ESP to print the message
  Serial.println(str); //Prints the message in the Serial Monitor
}

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(LED, OUTPUT); // Sets the LED as an Output
  pinMode(BUT, INPUT_PULLUP); // Sets the button as an Input with pullup
  Serial.begin(9600); // Starts the serial communication
  espSerial.begin(9600); //Starts the communication with the ESP
  digitalWrite(LED, LOW); //Start with the LED off

}

void loop() {

  echoSensor (); // For the echo sensor

  button (); // For the button that controls the LED

  analogValue = analogRead(A0); // For the LDR (light sensor). Has to be defore the if statements
  //  but after the defined functions (I think)


  if (distance <= 20) { // If the distance, the echo sensor measures, is equal of less that 20 cm
    //Check if the light is on
    if (analogValue > daylightInTheRoom) { //If the light is on
      Serial.println("You left");
      delay(1000); // important to wait a second so the first signal doesn't get resieved again

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
      Serial.println("You forgot to turn off the light"); // TEST, LATER THE MASSAGE IS SENT VIA WIFI
      sentMessageToESP_forgot ();

      // Waiting till the person comes back into the room
      echoSensor ();
      while (distance > 20) {
        echoSensor ();
      }
      Serial.println("You came back");
      sentMessageToESP_back ();
      delay(1000); // important to wait a second so the first signal doesn't get resieved again
    }
  }

}
