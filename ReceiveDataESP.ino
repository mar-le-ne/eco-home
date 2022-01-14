
int arr[]
void setup() {

  Serial.begin(115200);
  //while (!Serial) { // wait for serial port to connect. 
  //  ;
  // }
}


void loop() { 
  //if (Serial.available()) {
  //  Serial.println(Serial.read());   
  //} 

  if (Serial.available() > 3) {

    Serial.println(Serial.read());
  }
}
