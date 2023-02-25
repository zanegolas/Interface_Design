#include <Flicker.h>

// If you hold your finger on the input,
// something will happen.

const uint8_t touchPin = 15; // CHOOSE A TOUCH PIN
const uint8_t ledPin = 13;
TouchSwitch mInput1(touchPin);

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  // WARNING! if you setThreshold with no argument,
  // the threshold is calculated based on a call to
  // touchRead() so DON'T touch the input during setup()
  mInput1.setThreshold();
}

void loop() {
  mInput1.update();

  if (mInput1.rose()){
    Serial.println("rose");
  }
  if (mInput1.fell()){
    Serial.println("fell");
  }

  digitalWrite(ledPin, mInput1.read());
}
