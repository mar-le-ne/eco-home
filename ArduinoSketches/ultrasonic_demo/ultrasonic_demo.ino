

/* #define echoPin D5 // attach pin D2 Arduino to pin Echo of HC-SR04
  #define trigPin D6 //attach pin D3 Arduino to pin Trig of HC-SR04
*/


const byte echoPin = D5;
const byte trigPin = D7;

const byte buttonPin = D4;
const byte ledPin = D2;
const byte photocellPin = A0;


/*
  const byte echoPin = 7;
  const byte trigPin = 5;
*/

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int photocellReading = 0;

void latchButton() {
  if (!digitalRead(buttonPin)) { // Read the value of the button. Due to pull-up resistor, the logic is reversed.
    // false = pressed, true = not pressed.
    digitalWrite(ledPin, !digitalRead(ledPin)); // Set the value of the led to the opposite of its current state
    Serial.print("Button pressed. ");
    Serial.print("LED is: " );
    Serial.println(digitalRead(ledPin)); // Print the current state of the LED.

    while (!digitalRead(buttonPin)) { // while the button is pressed, be stuck in a loop until it is released.
      delay(50); // It is necessary to have some small delay in loops, or the ESP8266 might complain.
    }
    Serial.println("Button released");
  }
}

void echoSensor() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2.0; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void photoSensor() {
  photocellReading = analogRead(photocellPin);
  Serial.print("Analog reading = ");
  Serial.println(photocellReading);     // the raw analog reading
}


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(photocellPin, INPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
}


unsigned long mainTime = millis();
unsigned long timePreviousMeasurement = 0;
unsigned long timePerMeasurement = 5 * 1000;
int mainDelay = 50;
void loop() {
  latchButton();
  mainTime = millis();
  if (mainTime - timePreviousMeasurement > timePerMeasurement) {
    timePreviousMeasurement = millis();
    echoSensor();
    photoSensor();
  }
  delay(mainDelay);
}
