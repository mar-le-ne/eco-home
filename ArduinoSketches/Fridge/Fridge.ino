
#include "client_lib.h" // include the library for communication with our server.

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Fridge.ino is written by August Valentin, student ID: S194802                                            !
// If the function isn't credited with a source (such as a stackoverflow link), it's written by me (August) !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Pins:
const byte ledPin = D2;
const byte buzzerPin = D5;

// Hall sensor used is WPSE313 (from WHADDA).
// Pin layout: When looking at the module's LED, left = GND, middle = +5V (3.3V also works, but possibly a worse detection range), right = digital signal.
const byte magPin = D6;

void setup() {
  // Set baud-rate of Serial connection(?)
  Serial.begin(115200);
  Serial.println(""); // the ESP8266 prints some gibberish whenever it's reset (probably due to using another baudrate for some automatic messages).
  // To avoid our first message being printed on the same line, we print a newline.

  // Set pin modes:
  // we send a HIGH signal to the Buzzer when we want it to make noise
  pinMode(buzzerPin, OUTPUT);
  // we send a HIGH signal to the LED when we want to light it up.
  pinMode(ledPin, OUTPUT);
  // the Hall sensor sends a digital HIGH signal when no magnet is detected, and a digital LOW signal when a magnet is detected.
  // the signal is an INPUT to our microcontroller
  pinMode(magPin, INPUT);

  // set the parameters for connection to WiFi and the server hosted on the local network.
  String wName = "farligt wifi";
  String wPass = "august1234";
  String IP = "192.168.152.233";
  setupWIFI(wName, wPass, IP);
}


int alarmSpan = 15 * 1000; // 15 seconds
int mainDelay = 250; // main loop is done every 0.25 seconds.


bool checkFridgeOpen(byte magPin) {
  // uses the magnet detector.
  // Return true if the magnet is not detected, else false.
  // In our context, it means: true when fridge door is open, else false.
  bool isFridgeOpen = digitalRead(magPin);
  return isFridgeOpen;
}

void triggerBuzzer(bool trigger) {
  if (trigger) {
    tone(buzzerPin, 1000);
  }
  else {
    noTone(buzzerPin);
  }
}

bool alarmWasTriggered = false; // initialize the boolean for whether or not the alarm has been triggered


// Function for handling the 'alarm' process.
void fridgeAlarm(bool triggerAlarm) {
  // SIGNATURE:
  // Returns nothing.
  // uses triggerAlarm (Type: bool)
  // fridgeAlarm == true: turns the devices on, POST fridge is open
  // fridgeAlarm == false: turns the devices off, POST fridge is closed

  
  Serial.print("was alarm triggerd? "); Serial.println(triggerAlarm);
  digitalWrite(ledPin, triggerAlarm); // turn on or off the LED
  triggerBuzzer(triggerAlarm); // turn on or off the buzzer
  alarmWasTriggered = triggerAlarm; // Set the triggerAlarm boolean. For example, fridgeAlarm(true) means the alarm was triggered.
  POSTfridge(triggerAlarm); // Send the POST request to the server, telling the server that the fridge has been opened for too long.
}

unsigned long mainLoop = millis(); // represents the current time of the system.
unsigned long fridgeOpenDuration = 0; // represents how long the fridge door has been open.
unsigned long fridgeOpenStart = 0; // represents the moment in time when the fridge was opened (same as being last time it was closed).
void fridgeWrapper(byte magPin) {
  // Function for wrapping the fridge functionality into a single neat package.
  // The fridge should be installed with the magnet detector inside the frdige, and a magnet on the fridge door.
  // The magnet detector has inverse logic, meaning it sends a HIGH signal when a magnet is NOT detected.
  // Therefore, the fridge door is open when the detector sends HIGH signal.
  // The function is designed to not use delays.
  // Instead only run the timed functions every x seconds. 
  // This makes it possible to run other parts of the script more frequently than the timed region.

  /* If the fridge has been continuously open for some specific amount of time,
      We want to turn on the buzzer, the LED, and send a POST request to the server.
      A drawing of the logicflow can be found in "ArduinoSketches/Fridge/logic_flow.png".
  */
  
  bool isFridgeOpen = checkFridgeOpen(magPin); // checks the magnet sensor. 
  if (!isFridgeOpen) { // If fridge is closed
    if (alarmWasTriggered) { 
      fridgeAlarm(false); // Fridge was closed after having previously triggered the alarm. So now send the opposite signals.
    }
    // here is the main logic of the system
    // If the fridge door is closed, then the duration of having been open is 0.
    // And the most recent time it has been closed is updated
    // Logically, the moment in time when the fridge was opened, is then approximately equal to the time it was last detected as closed (incorrect by approx. mainDelay).
    fridgeOpenDuration = 0;
    fridgeOpenStart = mainLoop;
  }
  else { // if fridge door is open
    Serial.println("Fridge door is open!");
    fridgeOpenDuration = mainLoop - fridgeOpenStart; // update how long the fridge has been open for.
  }

  // check if the fridge has been open long enough to trigger the alarm AND we haven't already sent the alarm signal.
  if ((fridgeOpenDuration > alarmSpan) && !(alarmWasTriggered)) {
    fridgeAlarm(true); // Trigger the alarm
  }
}

// the main loop
void loop() {
  delay(mainDelay); // wait mainDelay milliseconds, ensuring that prints aren't sent too fast.
  mainLoop = millis(); // assign mainLoop to the current time.
  fridgeWrapper(magPin);
}
