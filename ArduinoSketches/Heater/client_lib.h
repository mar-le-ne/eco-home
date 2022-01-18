#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti wifiMulti;


// WifiName:
// CHANGE THESE!! To your personal WiFi. Make sure the password is not
// something used elsewhere, such as for facebook, netflix, other personal accounts etc.!!!
//char *ap_name = "Swingerklubben-2";
//char *ap_pass = "benjaminersmuk";

String IP = "";// The IP address of the server. Should be changed using the method from documentation.
String serverName = "";
const String protocol = "http://";
const String port = "8081";

const String api_extension = "/API";


void setupWIFI(String WiFiName, String WiFiPassword, String inputIP) {
  // set the global-scoped IP string and serverName:
#define WIFISETUP 1
  IP = inputIP;
  serverName =  protocol + IP + ":" + port;

  // Connect to WiFi network
  wifiMulti.addAP(WiFiName.c_str(), WiFiPassword.c_str());
  Serial.print("Connecting ...");

  const int maxAttempts = 10;
  // Attempt to connect to the WiFI, max 10 attempts.
  for (int attempts = 0; attempts < maxAttempts; attempts++) {
    delay(500);
    Serial.print(".");
    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.print("WiFi connected to: "); Serial.println(WiFi.SSID());
      break;
    }
  }
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.print("WiFi could not connect to: "); Serial.println(WiFi.SSID());
  }

}

bool isStringInStringArray(String value, const String Array[], int Size) {
  for (int i = 0; i < Size; i++) {
    if (value == Array[i]) {
      return true;
    }
  }
  return false;
}

bool isWifiConnected() {
#ifndef WIFISETUP
  Serial.println("You didn't run the setupWIFI(.,.,.) function yet");
  return false;
#endif
  return (WiFi.status() == WL_CONNECTED);
}


// function for parsing general String expressions into boolean:
bool parseBool(String input) {
  // source: https://stackoverflow.com/a/1414175
  // the switch-statement in arduino only works on int and char. So use multiple if-statements instead.
  input.toLowerCase();
  input.trim();
  const String trueStrings[3] = {"true", "yes", "1"};
  const String falseStrings[3] = {"false", "no", "0"};

  if (isStringInStringArray(input, trueStrings, 3)) {
    return true;
  }
  else if (isStringInStringArray(input, falseStrings, 3)) {
    return false;
  }
  else {
    return bool(input.toInt());
  }
}

String boolToString(bool input) {
  return input ? "true" : "false";
}

// Source for POSTreq and GETreq: https://randomnerdtutorials.com/esp8266-nodemcu-http-get-post-arduino/
bool POSTreq(String target, String value) {
  // SIGNATURE:
  // Returns true if POST request was succesful, else will return false.
  // 'target' is the name of your value, e.g. "LIGHT" for the light. Type: String
  // 'value' is the value you want to send to the server, e.g. "true" if the light is turned on. Type: String (not int or bool).
  int httpResponseCode = 0;
  if ( !isWifiConnected() ) {
    Serial.println("WiFi is not connected.");
  }
  else {
    Serial.println(" --- Beginning POST request --- ");
    WiFiClient client;
    HTTPClient http;

    String serverPath = serverName + api_extension;
    Serial.println(serverPath);

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverPath.c_str());

    // Specify content-type header
    http.addHeader("Content-Type", "text/plain");

    // Data to send with HTTP POST
    target.toUpperCase();
    value.toLowerCase();
    String httpRequestData = target + "=" + value;
    Serial.print("Request data is: "); Serial.println(httpRequestData);
    // Send HTTP POST request
    httpResponseCode = http.POST(httpRequestData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);  // -1 indicates that the IP might be wrong.

    // Free resources
    http.end();
    Serial.println(" --- ending POST request --- ");
  }
  return (httpResponseCode >= 200 && httpResponseCode < 300); // http Response codes of 2xx means the request was successful.
}

bool GETreq(String target, String* dest) { // * dest = value of destination
  // SIGNATURE:
  // Returns true if GET request was succesful, else will return false.
  // 'target' is the name of your value, e.g. "HOME" for whether the user is home or not. Type: String
  // 'dest' is a pointer of the variable where you want to store the result in. e.g. outside the function you create a String variable:
  // String result = "".
  // then call the function like GETreq("HOME",&result);
  // the value will then be stored in result. Type: String pointer.
  int httpResponseCode = 0;
  if ( !isWifiConnected() ) {
    Serial.println("WiFi is not connected.");
  }
  else {
    Serial.println(" --- Beginning GET request --- ");
    WiFiClient client;
    HTTPClient http;

    target.toUpperCase(); // ensure uppercase.
    String serverPath = serverName + api_extension + "/" + target;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverPath.c_str());

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      * dest = payload;
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    Serial.println(" --- ending GET request --- ");
  }
  return (httpResponseCode >= 200 && httpResponseCode < 300); // http Response codes of 2xx means the request was successful.
}



// functions to wrap specific requests:

/*
   "HOME" : POST from webpage, GET from arduino
   "FRIDGE": POST from arduino
   "LIGHT": POST from arduino
   "LIGHT_TIME": GET from webpage
   "FAUCET": POST from arduino
   "SHOWER": POST from arduino
   "SHOWER_TIME": GET from webpage
   "LIGHT_AUTO": POST from arduino when light shuts down automatically.
*/

#define HOME "HOME"
#define FRIDGE "FRIDGE"
#define LIGHT "LIGHT"
#define WAIT_TIME "WAIT_TIME"
#define FORGOT_LIGHT "FOROT_LIGHT"
#define FAUCET "FAUCET"
#define SHOWER "SHOWER"
#define LIGHT_AUTO "LIGHT_AUTO"

void badRequest(String target) {
  // display message when request was bad.
  Serial.println("request didn't work, so result of " + target + " is meaningless.");
}

String GEThome() {
  // SIGNATURE:
  // Returns true if user is home or not. Meaningless if request was unsuccesful.
  String target = HOME;
  String destination = "";
  bool requestSuccess = GETreq(target, &destination); // &destination is address of value destination
  if (!requestSuccess) {
    badRequest(target);
  }
  //bool result = parseBool(destination);
  String result = destination;
  return result;
}

void POSTfridge(bool isFridgeOpen) {
  String target = FRIDGE;
  String value = boolToString(isFridgeOpen);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}

void POSTheater(bool isHeaterOn) {
  String target = FRIDGE;
  String value = boolToString(isHeaterOn);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}

void POSTlight(bool isLightOn) {
  String target = LIGHT;
  String value = boolToString(isLightOn);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}

bool GETwaitTime() {
  // SIGNATURE:
  // Returns an integer corresponding to number of minutes to wait before turning off light. Meaningless if request was unsuccesful.
  String target = WAIT_TIME;
  String destination = "";
  bool requestSuccess = GETreq(target, &destination);
  if (!requestSuccess) {
    badRequest(target);
  }
  int result = atoi(destination.c_str()); // convert the string to integer
  return result;
}

void POSTforgotlight() {
  String target = FORGOT_LIGHT;
  String value = boolToString(true);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}

void POSTautolight() {
  String target = LIGHT_AUTO;
  String value = boolToString(true);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}


void POSTfaucet(bool isFaucetOn) {
  String target = FAUCET;
  String value = boolToString(isFaucetOn);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}

void POSTshower(bool isShowerRunning) {
  String target = SHOWER;
  String value = boolToString(isShowerRunning);
  bool requestSuccess = POSTreq(target, value);
  if (!requestSuccess) { // if request failed:
    badRequest(target);
  }
}
