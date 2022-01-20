#include "client_lib.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// client_demo.ino is written by August Valentin, student ID: S194802                                            !
// If the function isn't credited with a source (such as a stackoverflow link), it's written by me (August) !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// This sketch is purely for testing the wrapped functions of client_lib.h
// it will run the POSTrequests and the GETrequests, and that way, test all of the functions listed in the header file.

void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  String wName = "farligt wifi";
  String wPass = "august1234";
  String IP = "192.168.152.233";
  setupWIFI(wName, wPass, IP);
}


int mainDelay = 5 * 1000;
int POSTdelay = 25 * 1000;
void testPOSTfunction( void (*POSTfunc)(bool) ) {
  // The post functions all have the same signature: return type of void, parameter of Bool.
  // So it's easy to test them all;
  // we create a function that takes a function with the aforementioned signature,
  // and run it with the 2 possible parameter.
  (*POSTfunc)(true);
  delay(POSTdelay * 3); // Delay between each posting, to give the webpage ample time to automatically update
  (*POSTfunc)(false);  
  delay(POSTdelay); // delay before going to the next test function.
}

void testGEThome() {
  // Unlike the wrapped POST functions, the 2 GET functions have different signatures.
  // Since C/C++ is a bit strict on typing, it's easier to just create hand-made testing functions for them.
  bool homeValue = GEThome();
  String homeText = homeValue ? "User is home" : "User is not home";
  Serial.println(homeText);
  delay(POSTdelay);
}

void testGETwaitTime() {
  // same comment as in testGEThome(), above.
  int waitTime = GETwaitTime();
  Serial.print("Waiting time is: "); Serial.println(waitTime);
  delay(POSTdelay);
}

void testCleanString(String input) {
  // testing the cleanString function, to make sure it was cleaning the input Strings correctly.
  Serial.print("before cleaning: "); Serial.println(input);
  input = cleanString(input);
  Serial.print("after cleaning: "); Serial.println(input);
  delay(POSTdelay);
}

void loop() {
  testCleanString("This is a string \" with quotes \" ");
  
  // Testing the POST functions.
  testPOSTfunction(POSTfridge);
  testPOSTfunction(POSTforgotLight);
  testPOSTfunction(POSTfaucet);
  testPOSTfunction(POSTshower);
  
  // Testing the GET functions.
  testGEThome();
  testGETwaitTime(); 
  
}
