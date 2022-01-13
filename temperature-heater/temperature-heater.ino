byte redPin   = 7;
byte greenPin = 6;
byte bluePin  = 5;
int analogPin = A0;

int digVal    = 0;
float voltage = 0;
float temp    = 0;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(9600);

}

void loop() {

  // read the input from the pin
  digVal = analogRead(analogPin);
  voltage = digVal * (5.0 / 1023.0);
  temp = (voltage * 1000) / 10;
  Serial.print("Temp: ");
  Serial.print(temp, 1);
  Serial.println("ºC");

  if (temp < 22) {
    setColor(0, 0, 255); // blue color
  }
  else if (temp > 24) {
    setColor(255, 0, 0); // red color
  }
  else{
   setColor(0, 255, 0); // green color
  }
  delay(1000);
  
}

void setColor(int redVal, int greenVal, int blueVal) {
  analogWrite(redPin,   redVal);
  analogWrite(greenPin, greenVal);
  analogWrite(bluePin,  blueVal);
}
