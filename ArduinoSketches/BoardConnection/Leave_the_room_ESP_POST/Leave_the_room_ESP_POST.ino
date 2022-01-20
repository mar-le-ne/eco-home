/*
 * For connection with ESP and Arduino boards.
   This should be uploaded to the ESP.
   
   This is for POST requests.

   By Marie Sophie Mudge Woods s194384 and August Valentin
 */
// #include <ESP8266WiFi.h>
#include "client_lib.h" // connects/includes the header client_lib by August



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

//Sets up the internet
  String wName = "Galaxy A406D53";
  String wPass = "bzux7113";
  String IP = "192.168.43.222";
  setupWIFI(wName, wPass, IP);

  //  while (!Serial){
  //    ;
  //  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int forgot_1 = 49; //In the if-statements the ASCII code is used, so this is 1
  int back_2 = 50; //In the if-statements the ASCII code is used, so this is 2
  if (Serial.available()) { //This if-statement will happen everytime Serial.print is used in the Arduino code
    int string_from_Arduino = Serial.read();
    if (forgot_1 == string_from_Arduino) { //Will run when the read serial is 1
      POSTforgotLight(true); // Will post "You forgot to turn off the light" to the website
    }
    if (back_2 == string_from_Arduino) { //Will run when the read serial is 2
      POSTforgotLight(false); 
    }
    //    Serial.write(Serial.read());
    //    POSTforgotLight();
  }
}
