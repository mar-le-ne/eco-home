/*
 * This code help you to determine the daylightInTheRoom that should be implementet in the main
 * code for "Leave the room". This code is only used for installment.
 * You should test the code with the LED on and off. Add a button to the code.
 * Make sure that the number you choose for daylightInTheRoom is below the number of the LED on
 * but when the LED is off it shouldn't be higher than the final number choosen.
 * e.g. if you but the LDR really close to a red LED the LDR will show something close to 800. 
 * Therefore the final number choosen for daylightInTheRoom will be 780. Also checking that the
 * normal daylight in the room doesn't come over 780
 * 
 * Made by Marie Sophie Mudge Woods s194384 
*/
//The hardware setup:
//The light sencor should be no further away than 1 cm from the LED

//Defines LED and button
const int LED = 13;
const int BUT = 10;
int buttonState;

//Defines function
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
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUT, INPUT_PULLUP);
  digitalWrite(LED, LOW); //Start with the LED off
}

void loop() {
  // reads the input on analog pin A0 (value between 0 and 1023)
  int analogValue = analogRead(A0);
  Serial.print("Your daylightInTheRoom is: ");
  Serial.print(analogValue);   // the raw analog reading

  button (); // For the button that controls the LED


  // We'll have a few threshholds, qualitatively determined
  if (analogValue < 10) {
    Serial.println(" - Dark");
  } else if (analogValue < 200) {
    Serial.println(" - Dim");
  } else if (analogValue < 500) {
    Serial.println(" - Light");
  } else if (analogValue < 800) {
    Serial.println(" - Bright");
  } else {
    Serial.println(" - Very bright");
  }

  delay(500);
}
