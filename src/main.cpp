//2 button midi CC control

#include <Arduino.h>
#include <MIDIcontroller.h>

byte MIDIChannel = 5;

// Macros
const byte BUTTON_1 = 29;
const byte BUTTON_2 = 30;
const byte LED_GREEN = 33;
const byte LED_RED = 34;
const byte LED_BLUE = 35;

// Sets button 1 and 2 to control MIDI CC 1 and 2
MIDIswitch mInput1(BUTTON_1, 1, MOMENTARY);
MIDIswitch mInput2(BUTTON_2, 2, MOMENTARY);

// Global led variables
elapsedMillis mElapsedMS = 0;
int mBlueValue = 0;
int mIncrement = 1;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    mInput1.send();
    mInput2.send();

    //Green LED for input 1, Red LED for input 2, Pulsing Blue LED for standby
    digitalWrite(LED_GREEN, mInput1.state);
    digitalWrite(LED_RED, mInput2.state);

    auto is_writing = (mInput1.state || mInput2.state);
    if (mElapsedMS > 3 && !is_writing){
        mElapsedMS = 0;
        if (mBlueValue >= 255 || mBlueValue <= 0){
            mIncrement *= -1;
        }
        mBlueValue += mIncrement;
    } else {
        mBlueValue = 0;
    }

    analogWrite(LED_BLUE, mBlueValue);

    // This prevents crashes that happen when incoming usbMIDI is ignored.
    while(usbMIDI.read()){}

}