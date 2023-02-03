#include <Flicker.h>

/* This determines the interval between the time
   your finger is detected hovering over the input
   and the time it makes contact with the input.
   The LED lights when your finger goes below the
   'hover' threshold to act as a 'ready' light.*/

byte touchPin = 15; // CHOOSE A TOUCH PIN
byte LED = 13;
TouchVelocity myInput1(touchPin);
 
void setup() {
  pinMode(LED, OUTPUT);
  
  // WARNING! setThreshold() uses a call to touchRead()
  // so DON'T touch the input during setup()
  myInput1.setThreshold();
  digitalWrite(LED, HIGH); 
}

void loop(){
  int velocity = myInput1.read();
  if (myInput1.fell()){
    digitalWrite(LED, HIGH);
  }
  if (myInput1.rose()){
    digitalWrite(LED, LOW);
  }
  if (velocity >= 1){
    Serial.println(velocity);
  }
}
