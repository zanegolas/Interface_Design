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
const byte JOYSTICK_1_X = 36;
const byte JOYSTICK_1_Y = 37;
const byte JOYSTICK_2_X = 38;
const byte JOYSTICK_2_Y = 39;

// Sets button 1 and 2 to control MIDI CC 1 and 2
Bounce mButton1(BUTTON_1, 5);
Bounce mButton2(BUTTON_2, 5);

int mCurrentJoystick1X = 0;
int mCurrentJoystick2X = 0;

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
    pinMode(JOYSTICK_1_X, INPUT);
    pinMode(JOYSTICK_1_Y, INPUT);
    pinMode(JOYSTICK_2_X, INPUT);
    pinMode(JOYSTICK_2_Y, INPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    mButton1.update();
    mButton2.update();

    // Note On messages when each button is pressed
    if (mButton1.fallingEdge()) {
        usbMIDI.sendNoteOn(60, 99, 2);  // 60 = C4
        digitalWrite(LED_RED, HIGH);
    }
    if (mButton2.fallingEdge()) {
        usbMIDI.sendNoteOn(67, 99, 3);
        digitalWrite(LED_GREEN, HIGH);
    }
    // Note Off messages when each button is released
    if (mButton1.risingEdge()) {
        usbMIDI.sendNoteOff(60, 0, 2);  // 60 = C4
        digitalWrite(LED_RED, LOW);
    }
    if (mButton2.risingEdge()) {
        usbMIDI.sendNoteOff(67, 0, 3);
        digitalWrite(LED_GREEN, LOW);
    }


    int joystick_1_x = analogRead(JOYSTICK_1_X) - 512; // 0 at mid point
    if (abs(joystick_1_x - mCurrentJoystick1X) > 1)  { // have we moved enough to avoid analog jitter?
        if (abs(joystick_1_x) > 4) { // are we out of the central dead zone?
            usbMIDI.sendPitchBend(8 * joystick_1_x, 2); // or -8 depending which way you want to go up and down
            mCurrentJoystick1X = joystick_1_x;
        }
    }

    int joystick_2_x = analogRead(JOYSTICK_2_X) - 512; // 0 at mid point
    if (abs(joystick_2_x - mCurrentJoystick2X) > 1)  { // have we moved enough to avoid analog jitter?
        if (abs(joystick_2_x) > 4) { // are we out of the central dead zone?
            usbMIDI.sendPitchBend(8 * joystick_2_x, 3); // or -8 depending which way you want to go up and down
            mCurrentJoystick1X = joystick_2_x;
        }
    }


    // This prevents crashes that happen when incoming usbMIDI is ignored.
    while(usbMIDI.read()){}

}