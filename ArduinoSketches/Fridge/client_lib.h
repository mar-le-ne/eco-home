#include <ESP8266WiFi.h>        // Include library to connect with the WiFi
#include <WiFiClient.h>         // Used as an adapter for the WiFi library, to use with HTTPClient.
#include <ESP8266HTTPClient.h>  // Used to use the HyperText Transfer Protocol (HTTP), the protocol for communication with our server.


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// client_lib.h is written by August Valentin, student ID: S194802                                          !
// If the function isn't credited with a source (such as a stackoverflow link), it's written by me (August) !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Nothing in this file is required to be modified for it to work;!
// Just remember to call setupWIFI() in your setup() function.    !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Global values modified by the setupWIFI() function.
String IP = ""; // the IP
String serverName = "";

// Constants chosen to communicate with the server; we use port 8081, and the HTTP protocol to communicate (port 80 was already reserved for standard HTTP communication)
const String protocol = "http://";
const String port = "8081";
// extension to the path, used to let the server know "what" we generally want to communicate about.
const String api_extension = "/API";

// function for checking if the ESP8266 is connected to the WiFi.
bool isWifiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  else {
    //Serial.println("You are not connected to the WiFi");
    return false;
  }
}

// Call this function in your setup() function.
// Used to set the global-scope variables, and connect to the specified WiFi
void setupWIFI(String WiFiName, String WiFiPassword, String inputIP) {

  // set the global-scoped IP string and serverName:
  IP = inputIP;
  serverName =  protocol + IP + ":" + port;

  // Connect to WiFi network
  WiFi.begin(WiFiName.c_str(), WiFiPassword.c_str());
  Serial.print("Connecting ...");

  const int maxAttempts = 10;
  // Attempt to connect to the WiFI, max 10 attempts.
  for (int attempts = 0; attempts < maxAttempts; attempts++) { // try to connect to the WiFi (maxAttempts) number of times.
    delay(500);
    Serial.print(".");
    if (isWifiConnected) {
      Serial.println("");
      Serial.print("WiFi connected to: "); Serial.println(WiFi.SSID());
      break; // if WiFi connects, break the loop.
    }
  }
  if (!isWifiConnected) { // Send a message to the Serial terminal if connection wasn't possible.
    Serial.print("WiFi could not connect to: "); Serial.println(WiFi.SSID());
  }
}


// Simple function for checking if the String is present in the given String array of size arrSize.
bool isStringInStringArray(String value, const String Array[], int arrSize) {
  for (int i = 0; i < arrSize; i++) {
    if (value == Array[i]) {
      return true; // if the String is found in the array.
    }
  }
  return false; // if the String was not found
}


// Function for cleaning a String and converting it to it lowercase
// returns the new "cleaned" String (function is NOT "in-place").
String cleanString(String input) {
  input.toLowerCase();
  input.trim();

  // the GET response from the server might include \", the actual quotation character. We will 'remove' any instances of them,
  // because they could interfere with how some program interacts with strings.
  const int stringLength = input.length();
  String cleanString = "";
  const char quote = '\"';
  for (int i = 0; i < stringLength; i++) {
    char inpChar = input.charAt(i);
    if (inpChar != quote) {
      cleanString.concat(inpChar);   // we don't actually remove \". Instead we just copy the String's characters, except for any \" character.
      // this way, the indexing isn't messed up (removing characters from the String we're iterating through would do that)
    }
  }
  return cleanString; // return the new String.
}


// function for parsing general String expressions into boolean:
bool parseBool(String input) {
  // source: https://stackoverflow.com/a/1414175
  // the switch-statement in arduino only works on int and char. So use multiple if-statements instead.

  String cleanedInput = cleanString(input);
  // For our case, we will only see "true" and "false" being used, but there's an offchance the program converts it to binary booleans instead of the terms
  // so we will also cover that case:
  const String trueStrings[3] = {"true", "yes", "1"};
  const String falseStrings[3] = {"false", "no", "0"};

  if (isStringInStringArray(cleanedInput, trueStrings, 3)) {
    return true; // case of finding the input in the list of String representations of true
  }
  else if (isStringInStringArray(cleanedInput, falseStrings, 3)) {
    return false; // case of finding the input in the list of String representations of false
  }
  else {
    Serial.println("Warning!! Couldn't parse the String to boolean");
    return false; // return false as fall-back, but prints a warning to let the user know something is wrong.
  }
}


// Going from boolean to a String representation of the boolean
String boolToString(bool input) {
  return input ? "true" : "false"; // automatically casted from C string to c++ String due to the return-type of the function.
}


// Parse and convert a C++ string to int type
int parseIntCpp(String input) {
  return atoi(input.c_str()); // using the C function atoi().
}


// Returns boolean, regarding whether the http status (response) code means the request was successful (return true) or not (return false).
bool isSuccessfulHTTP(int httpStatusCode) {
  return (httpStatusCode > 199) && (httpStatusCode < 300);  // http status codes of 2xx means the request was successful.
  // For simplicity, we will consider all other status codes as "unsuccessful".
}


// Source for POSTreq and GETreq: https://randomnerdtutorials.com/esp8266-nodemcu-http-get-post-arduino/
bool POSTreq(String target, String value) {
  // SIGNATURE:
  // Returns true if POST request was successful, else will return false.
  // 'target' is the name of your value, e.g. "LIGHT" for the light. Type: String
  // 'value' is the value you want to send to the server, e.g. "true" if the light is turned on. Type: String (you CANNOT use int or bool).
  int httpResponseCode; // initialize responseCode variable.
  if ( !isWifiConnected() ) { // if the wifi isn't connected, set the response code to -1.
    Serial.println("WiFi is not connected.");
    httpResponseCode = -1;
  }
  else {
    Serial.println(" --- Beginning POST request --- ");
    WiFiClient client;
    HTTPClient http;

    // Data to send with HTTP POST
    target.toUpperCase(); // our paths use upper case in the server
    value.toLowerCase(); // and their assigned values are lower case

    String serverPath = serverName + api_extension + "/" + target; // add the target to the path, for clarity's sake
    // (not strictly necessary since we use the payload to determine what the target is, server-side).
    Serial.print("The path is: "); Serial.println(serverPath);

    // Begin the HTTP message, with our IP + URL path.
    http.begin(client, serverPath.c_str());
    // Specify content-type header
    http.addHeader("Content-Type", "text/plain");
    // format the request data (payload)
    // For example, the payload is "FRIDGE=true" when the fridge is open.
    String httpRequestData = target + "=" + value;
    Serial.print("Request data is: "); Serial.println(httpRequestData);
    // Send HTTP POST request
    httpResponseCode = http.POST(httpRequestData);

    // Free resources, i.e. end the HTTP process on our side.
    http.end();
    Serial.println(" --- ending POST request --- ");
  }
  // if the responseCode == -1, it indicates that the ESP8266 could not connect to the server (possibly wrong IP, or not connected to WiFi).
  Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
  return isSuccessfulHTTP(httpResponseCode);
}


bool GETreq(String target, String* dest) {
  // SIGNATURE:
  // Returns true if GET request was successful, else will return false.
  // 'target' is the name of your value, e.g. "HOME" for whether the user is home or not. Type: String
  // 'dest' is a pointer of the variable where you want to store the result in. Type: String pointer
  // So to use the function, you create a String variable before calling the function:
  // ('>>' indicates code in your .ino file, not a bit-shift operation)
  // >> String result = "".
  // then call the function like this:
  // >> GETreq("HOME",&result);
  // then the value will be stored in result. Type: String.
  int httpResponseCode = 0;
  bool isSuccessfulGET = isSuccessfulHTTP(httpResponseCode); // initialize the boolean for whether the request is succesful or not, to false
  if ( !isWifiConnected() ) {
    Serial.println("WiFi is not connected.");
  }
  else {
    Serial.println(" --- Beginning GET request --- ");
    WiFiClient client;
    HTTPClient http;

    target.toUpperCase(); // ensure uppercase.
    String serverPath = serverName + api_extension + "/" + target;
    Serial.println(serverPath);
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverPath.c_str());

    // Send HTTP GET request
    httpResponseCode = http.GET();

    // if the WiFi was connected, set the "successful" bool related to the response code:
    isSuccessfulGET = isSuccessfulHTTP(httpResponseCode);
    if (isSuccessfulGET) {
      // if succesful, we can get a valid result.
      String payload = http.getString();
      *dest = payload; // assign the value stored at the destination pointer, equal to the payload value
    }
    else {
      // if the request was unsuccessful, reset the value stored at *dest to empty String.
      *dest = "";
    }
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    // Free resources
    http.end();
    Serial.println(" --- ending GET request --- ");
  }
  return isSuccessfulGET;
}




// specifications of the data tags; described where they should be used.
/*
   "HOME" : POST from webpage, GET from ESP8266
   "FRIDGE": POST from ESP8266
   "FORGOT_IIGHT": POST from ESP8266
   "WAIT_TIME": GET from ESP8266
   "FAUCET": POST from ESP8266
   "SHOWER": POST from ESP8266
   "SHOWER_TIME": GET from webpage

   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   Deprecated:                                                         !
   "LIGHT": POST from ESP8266                                          !
   "LIGHT_TIME": GET from webpage                                      !
   "LIGHT_AUTO": POST from ESP8266 when light shuts down automatically.!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

// Aliases to avoid typos
#define HOME "HOME"
#define FRIDGE "FRIDGE"
#define LIGHT "LIGHT"
#define WAIT_TIME "WAIT_TIME"
#define FORGOT_LIGHT "FORGOT_LIGHT"
#define FAUCET "FAUCET"
#define SHOWER "SHOWER"
#define LIGHT_AUTO "LIGHT_AUTO"


void badRequest(String target) {
  // display message when request was bad.
  Serial.println("request didn't work, so result of " + target + " is meaningless.");
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// functions to wrap specific requests, begins here!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// GEThome wraps GETreq(HOME, &destination) and returns the new value stored in destination;
bool GEThome() {
  // SIGNATURE:
  // Returns the server's data on whether the user is home (return true) or not (return false).
  // Meaningless if request was unsuccessful.
  String target = HOME;
  String destination = "";
  bool requestSuccess = GETreq(target, &destination);
  Serial.print("GEThome successful?: "); Serial.println(requestSuccess);
  if (!requestSuccess) {
    badRequest(target);
  }
  bool result = parseBool(destination);
  return result;
}

// POSTfridge wraps POSTreq(FRIDGE, isFridgeOpen), requiring isFridgeOpen as parameter.
void POSTfridge(bool isFridgeOpen) {// SIGNATURE:
  // SIGNATURE:
  // Returns nothing.
  // uses isFridgeOpen (Type: bool), converts it to a String
  // then sends "FRIDGE=" + the String representation, to the server.
  // Meaningless if request was unsuccessful.
  String target = FRIDGE;
  String value = boolToString(isFridgeOpen);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}


// GETwaitTime wraps GETreq(WAIT_TIME, &destination) and returns the value stored in destination, after parsing it as integer;
int GETwaitTime() {
  // SIGNATURE:
  // Returns the server's data on the current waiting time in minutes.
  // Meaningless if request was unsuccessful.
  // (context: returned result represents the number of minutes to wait, before the server notifies the user that they forgot to turn off the light)
  String target = WAIT_TIME;
  String destination = "";
  bool requestSuccess = GETreq(target, &destination);
  if (!requestSuccess) {
    badRequest(target);
  }
  int result = parseIntCpp(destination); // parse the String as integer
  return result;
}


// POSTforgotLight wraps POSTreq(FORGOT_LIGHT, hasForgotLight), requiring hasForgotLight as parameter.
void POSTforgotLight(bool hasForgotLight) {
  // SIGNATURE:
  // Returns nothing.
  // uses hasForgotLight (Type: bool), converts it to a String
  // then sends "FORGOT_LIGHT=" + the String representation, to the server.
  // Meaningless if request was unsuccessful.
  String target = FORGOT_LIGHT;
  String value = boolToString(hasForgotLight);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}


// POSTforgotLight wraps POSTreq(FAUCET, isFaucetOn), requiring isFaucetOn as parameter.
void POSTfaucet(bool isFaucetOn) {
  // SIGNATURE:
  // Returns nothing.
  // uses isFaucetOn (Type: bool), converts it to a String
  // then sends "FAUCET=" + the String representation, to the server.
  // Meaningless if request was unsuccessful.
  String target = FAUCET;
  String value = boolToString(isFaucetOn);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}


// POSTforgotLight wraps POSTreq(SHOWER, isShowerRunning), requiring isShowerRunning as parameter.
void POSTshower(bool isShowerRunning) {
  // SIGNATURE:
  // Returns nothing.
  // uses isShowerRunning (Type: bool), converts it to a String
  // then sends "SHOWER=" + the String representation, to the server.
  // Meaningless if request was unsuccessful.
  String target = SHOWER;
  String value = boolToString(isShowerRunning);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}





// !!!!!!!!!!!!!!!!!!!!!!
// Deprecated functions!!
// !!!!!!!!!!!!!!!!!!!!!!
// These functions were going to be used, but were cut because of deadlines.

// function called by deprecatedFunctions to warn the developers.
// is deprecated because the developer of the client_lib.h decided it was better to 'comment' the deprecated functions
// terminating compilation if the file uses a deprecated function (forcing developers to not use them).
/*
  void deprecatedFunctionWarning() {
  Serial.println("WARNING: Function is deprecated!!!");
  }

  void POSTautolight() {
    String target = LIGHT_AUTO;
    String value = boolToString(true);
    bool requestSuccess = POSTreq(target, value);
    if (!requestSuccess) { // if request failed:
    badRequest(target);
    }
  deprecatedFunctionWarning();
  }

  void POSTlight(bool isLightOn) {
    String target = LIGHT;
    String value = boolToString(isLightOn);
    bool requestSuccess = POSTreq(target, value);
    if (!requestSuccess) { // if request failed:
    badRequest(target);
    }
  deprecatedFunctionWarning();
  }
*/
