#include <Flicker.h>

// The input goes on when touched
// and off when touched again.

byte touchPin = 15; // CHOOSE A TOUCH PIN
byte ledPin = 13;

// MOMENTARY (the default) or LATCH can be specified
TouchSwitch mInput1(touchPin, LATCH);

void setup() {
  pinMode(ledPin, OUTPUT);

  // WARNING! if you setThreshold with no argument,
  // the threshold is calculated based on a call to
  // touchRead() so DON'T touch the input during setup()
  mInput1.setThreshold();
}

void loop(){
  mInput1.update();
  digitalWrite(ledPin, mInput1.read());
}
