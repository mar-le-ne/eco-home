/*
  The function waterOn is made by Sophie Woods
*/

const int waitingTime = 500; // The time that it's waiting is: 50*waitingTime/1000=seconds.
// So waitingTime = 1500 will be 1,25 min. 500 = 25 seconds


void waterOn () {
  if (level >= 40) {
    // Wait some time to see if the person turns off the water again
    for (int i = 0; i <= waitingTime; i++) {
      getLevel ();
      delay(50);
      if (level <= 40) { //If the person turns off the water again the loop will end
        Serial.println("Water has been turned off within the waiting time");
        delay(1000); // important to wait a second so the first signal doesn't get resieved again
        return;
      }
    }
    Serial.println("You forgot to turn off the water");
    // Maybe have to add something here...
  }
}

}
