#include <Flicker.h>

/* This detects how hard/fast an input is touched and
   sends a MIDI note with corresponding velocity.
   Be sure to choose MIDI from the "Tools/USB Type" menu.*/

byte touchPin = 15; // CHOOSE A TOUCH PIN
int noteNumber = 38; // CHOOSE A NOTE TO PLAY

// Parameters are: pin, minimum velocity, maximum velocity
TouchVelocity myInput1(touchPin, 1, 127);
 
void setup() {
  // WARNING! setThreshold() uses a call to touchRead()
  // so DON'T touch the input during setup()
  myInput1.setThreshold();
}

void loop(){
  int velocity = myInput1.responsiveRead();
  if (velocity >= 1){
    usbMIDI.sendNoteOn(noteNumber, velocity, 0);
  }
  if (myInput1.fell()){
    usbMIDI.sendNoteOff(noteNumber, 0, 0);
  }
}
