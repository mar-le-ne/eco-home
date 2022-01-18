#include "client_lib.h"

void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  String wName = "farligt wifi";
  String wPass = "august1234";
  String IP = "192.168.152.233";
  setupWIFI(wName, wPass, IP);

}


int mainDelay = 5 * 1000;
int minorDelay = 15 * 1000;
void testPOSTfunction( void (*POSTfunc)(bool) ) {
  (*POSTfunc)(true);
  delay(minorDelay);
  (*POSTfunc)(false);  
  delay(minorDelay);
}

void testGEThome() {
  bool homeValue = GEThome();
  String homeText = homeValue ? "User is home" : "User is not home";
  Serial.println(homeText);
  delay(minorDelay);
}

void testGETwaitTime() {
  int waitTime = GETwaitTime();
  Serial.print("Waiting time is: "); Serial.println(waitTime);
  delay(minorDelay);
}

void testCleanString(String input) {
  Serial.print("before cleaning: "); Serial.println(input);
  input = cleanString(input);
  Serial.print("after cleaning: "); Serial.println(input);
}

void loop() {
  delay(mainDelay);

  // testCleanString("This is a string \" with quotes \" ");
  
  // Testing the POST functions.
  testPOSTfunction(POSTfridge);
  testPOSTfunction(POSTforgotLight);
  testPOSTfunction(POSTfaucet);
  testPOSTfunction(POSTshower);
  
  // Testing the GET functions.
  testGEThome();
  testGETwaitTime();

  
}
