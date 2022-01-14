byte redPin   = 7;
byte greenPin = 6;
byte bluePin  = 5;
const byte BUTTON = 2;

int on_off = 0;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(BUTTON, INPUT);
  Serial.begin(115200);
}

void loop() {
  if (digitalRead(BUTTON) == HIGH) {    // pressed button
    switch (on_off) {
      // BRG
      case 0:
        setColor(255, 0, 0); // cool down
        Serial.println("Heater is going to be turned off now [blue]");
        on_off++;
        delay(500);
        break;
        
      case 1:
        setColor(0, 255, 0); // warm up
        Serial.println("Heater is being turned on now [red]");
        delay(500);
        on_off = 0;
        break;
        
      default:
        on_off = 0;
    }
  }
}

void setColor(int redVal, int greenVal, int blueVal) {
  analogWrite(redPin,   redVal);
  analogWrite(greenPin, greenVal);
  analogWrite(bluePin,  blueVal);
}
