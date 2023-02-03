#include <Flicker.h>

// This reads a touch input and prints a new
// value only when the signal is biased enough.
// The larger the change, the more immediate the response.

byte touchPin = 15; // CHOOSE A TOUCH PIN
byte ledPin = 13;
int preVal;
TouchVariable myInput1(touchPin);

void setup() {
  Serial.begin(9600);

  // WARNING! if you setInputRange with no argument,
  // the input range is calculated based on a call to
  // touchRead() so DON'T touch the input during setup()
  myInput1.setInputRange();
}

void loop(){
  int newVal = myInput1.read();
  if (newVal != preVal){
    Serial.print(myInput1.inLo); Serial.print(" ");
    Serial.print(myInput1.inHi); Serial.print(" ");
    Serial.println(newVal);
    preVal = newVal;
  }
}
