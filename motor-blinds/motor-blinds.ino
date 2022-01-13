byte stepPin1 = 9;
byte stepPin2 = 10;
byte stepPin3 = 11;
byte stepPin4 = 12;
int stepNumber = 1;
int counter = 0;

void setup() {
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(stepPin4, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  for (int a = 0; a < 1000; a++) {
    OneStep(1);
    delay(2);       // minimum is 2
  }

  for (int a = 0; a < 1000; a++) {
    OneStep(0);
    delay(2);       // minimum is 2
  }
}

void OneStep(int dir) {    // dir declares rotation direction
  Serial.println(dir);
  if (dir == 1) {         // clockwise
    switch (stepNumber) {
      case 1:
        digitalWrite(stepPin1, HIGH); // 1
        digitalWrite(stepPin2, LOW);  // 0
        digitalWrite(stepPin3, LOW);  // 0
        digitalWrite(stepPin4, LOW);  // 0
        break;
      case 2:
        digitalWrite(stepPin1, LOW);  // 0
        digitalWrite(stepPin2, HIGH); // 1
        digitalWrite(stepPin3, LOW);  // 0
        digitalWrite(stepPin4, LOW);  // 0
        break;
      case 3:
        digitalWrite(stepPin1, LOW);  // 0
        digitalWrite(stepPin2, LOW);  // 0
        digitalWrite(stepPin3, HIGH); // 1
        digitalWrite(stepPin4, LOW);  // 0
        break;
      case 4:
        digitalWrite(stepPin1, LOW);  // 0
        digitalWrite(stepPin2, LOW);  // 0
        digitalWrite(stepPin3, LOW);  // 0
        digitalWrite(stepPin4, HIGH); // 1
        break;
    }
  } else {
    switch (stepNumber) {
      case 1:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, HIGH);
        break;
      case 2:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, HIGH);
        digitalWrite(stepPin4, LOW);
        break;
      case 3:
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, HIGH);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
        break;
      case 4:
        digitalWrite(stepPin1, HIGH);
        digitalWrite(stepPin2, LOW);
        digitalWrite(stepPin3, LOW);
        digitalWrite(stepPin4, LOW);
    }
  }
  stepNumber++;            // 1, 2, 3, 4
  counter++;

  if (stepNumber > 4) {    // switch
    stepNumber = 1;
  }
  if (counter > 10000) {
    delay(10000000);
  }
}
