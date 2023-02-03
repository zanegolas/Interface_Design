#include <Flicker.h>

// If you hold your finger on the input
// for long enough, something will happen.

const uint8_t touchPin = 15; // CHOOSE A TOUCH PIN
const uint8_t ledPin = 13;
TouchSwitch myInput1(touchPin);

void setup() {
  pinMode(ledPin, OUTPUT);

  // WARNING! if you setThreshold with no argument,
  // the threshold is calculated based on a call to
  // touchRead() so DON'T touch the input during setup()
  myInput1.setThreshold();
  myInput1.interval(300); // How long to hold?
}

void loop() {
  myInput1.update();
  digitalWrite(ledPin, myInput1.read());
}
