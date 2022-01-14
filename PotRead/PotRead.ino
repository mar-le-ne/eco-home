#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

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
