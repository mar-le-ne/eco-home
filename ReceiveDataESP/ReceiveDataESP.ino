/* ReceiveDataESP.ino
 * 
 * Program used in order to receive data over serial communication 
 * from another Arduino board.
 * 
 * Connect Rx pin of the ESP to Tx pin of the Arduino board
 * Connect Tx pin of the ESP to Rx pin of the Arduino board
 * Connect both boards to GND
 * 
 * created 13/01/2022
 * by Victor Leweke
 * last modified 14/01/2022
 */

int const numVariables = 2;        // Define number of variables to be received
int arr[numVariables];             // Create an empty array with size equal to number of variables

void setup() {

  Serial.begin(115200);
  while (!Serial) { // Make sure the connection is established
    ;
  }
}


void loop() {

  if (Serial.available() > numVariables+1) {
    for (int i = 0; i < numVariables; i++) {
      arr[i] = (int)Serial.parseInt();      // Each variable is assigned to an index in the array
      Serial.println(arr[i]);               // For testing
    }
  }
  delay(1000);
}
