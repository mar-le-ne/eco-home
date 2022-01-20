// This is just a test file in order to test the connection
// Made by Victor Leweke

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  ; 
  }
}

void loop() { // run over and over
  if (Serial.available()) {
    Serial.println(Serial.read());
  }
}
