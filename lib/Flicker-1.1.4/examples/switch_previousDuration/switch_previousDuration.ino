#include <Flicker.h>

// This will tell you how long the input stayed in its PREVIOUS state.
// It may be useful to call this within rose() or fell().

const uint8_t touchPin = 15; // CHOOSE A TOUCH PIN
const uint8_t ledPin = 13;
TouchSwitch mInput1(touchPin);

void setup() {
  pinMode(ledPin, OUTPUT);

  // WARNING! if you setThreshold with no argument,
  // the threshold is calculated based on a call to
  // touchRead() so DON'T touch the input during setup()
  mInput1.setThreshold();
}

void loop() {
  mInput1.update();

  if (mInput1.fell()){
    // After the input was released, print how long it was held.
    Serial.println(mInput1.previousDuration() );
  }

  digitalWrite(ledPin, mInput1.read());
}
