//2 button midi CC control

#include <Arduino.h>
#include <MIDIcontroller.h>

byte MIDIChannel = 5;

const byte button_1 = 29;
const byte button_2 = 30;
const byte green_led = 33;
const byte red_led = 34;

// Sets button 1 and 2 to control MIDI CC 1 and 2
MIDIswitch myInput1(button_1, 1, MOMENTARY);
MIDIswitch myInput2(button_2, 2, MOMENTARY);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(green_led,OUTPUT);
    pinMode(red_led, OUTPUT);
    pinMode(button_1, INPUT_PULLUP);
    pinMode(button_2, INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    myInput1.send();
    myInput2.send();

    // Sets LED to activate when either button is pressed
    byte input_active = myInput1.state || myInput2.state;

    //Green LED for device standby and red led for signal
    digitalWrite(green_led, !input_active);
    digitalWrite(red_led, input_active);

    // This prevents crashes that happen when incoming usbMIDI is ignored.
    while(usbMIDI.read()){}

}