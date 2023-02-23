//2 button midi CC control

#include <Arduino.h>
#include <MIDIcontroller.h>

byte MIDIChannel = 5;

const byte button_1 = 29;
const byte button_2 = 30;
const byte green_led = 33;
const byte red_led = 34;
const byte blue_led = 35;

// Sets button 1 and 2 to control MIDI CC 1 and 2
MIDIswitch myInput1(button_1, 1, MOMENTARY);
MIDIswitch myInput2(button_2, 2, MOMENTARY);

// Global led variables
elapsedMillis sinceChange = 0;
int blueVal = 0;
int increment = 1;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(green_led,OUTPUT);
    pinMode(red_led, OUTPUT);
    pinMode(blue_led, OUTPUT);
    pinMode(button_1, INPUT_PULLUP);
    pinMode(button_2, INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    myInput1.send();
    myInput2.send();

    //Green LED for device standby and red led for signal
    digitalWrite(green_led, myInput1.state);
    digitalWrite(red_led, myInput2.state);


    if (sinceChange > 3){
        sinceChange = 0;
        blueVal += increment;
        if (blueVal >= 255 || blueVal <= 0){
            increment *= -1;
        }
        analogWrite(blue_led, blueVal);
    }

    // This prevents crashes that happen when incoming usbMIDI is ignored.
    while(usbMIDI.read()){}

}