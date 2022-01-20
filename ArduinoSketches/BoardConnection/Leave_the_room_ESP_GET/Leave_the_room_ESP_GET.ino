/*
   For connection with ESP and Arduino boards.
   This should be uploaded to the ESP.
   
   When the comment POST is used it means that this line of code is nessesary to post from the Arduino
   to the ESP
   When the comment GET is used it means that this line of code is nessesary for getting information from the ESP
   to the Arduino

   By Marie Sophie Mudge Woods s194384
   With help from August Valentin
*/
#include "client_lib.h" // connects/includes the header client_lib by August
#include <SoftwareSerial.h> //GET

unsigned long timeSinceGET = 0;
unsigned long GETfreq = 15 * 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600); //GET

  //Sets up the internet
  String wName = "Galaxy A406D53";
  String wPass = "bzux7113";
  String IP = "192.168.43.222";
  setupWIFI(wName, wPass, IP);

}

void loop() {
  //For POST:
//  int forgot_1 = 49; //In the if-statements the ASCII code is used, so this is 1
//  int back_2 = 50; //In the if-statements the ASCII code is used, so this is 2
//  if (Serial.available()) { //POST: This if-statement will happen everytime Serial.print is used in the Arduino code
//    int string_from_Arduino = Serial.read(); //POST
//    if (forgot_1 == string_from_Arduino) { //Will run when the read serial is 1
//      POSTforgotLight(true); // Will post "You forgot to turn off the light" to the website
//    }
//    if (back_2 == string_from_Arduino) { //Will run when the read serial is 2
//      POSTforgotLight(false);
//    }
//  }

  //For GET:
  String strWaitTime = "";
  //  if (millis() - timeSinceGET > GETfreq) {
  //    int waitTime = GETwaitTime();
  //    strWaitTime = String(waitTime);
  //    timeSinceGET = millis();
  //    Serial1.println(strWaitTime); //GET: This sent the information to the Arduino
  //    Serial.println(strWaitTime); //GET: You have to print it for it to be sent to the Arduino probobly
  //
  //  }  
  int waitTime = GETwaitTime();
  strWaitTime = String(waitTime);
  Serial1.println(strWaitTime); //GET: This sent the information to the Arduino
  Serial.println(strWaitTime); //GET: You have to print it for it to be sent to the Arduino probobly

  //  String TEST = "Hi from the ESP";

  //  Serial.println (waitTime);
}
