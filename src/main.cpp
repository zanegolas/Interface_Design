//2 button midi CC control

#include <Arduino.h>
#include <usb_midi.h>
#include <LiquidCrystal_I2C.h>

byte MIDIChannel = 5;

// Global Objects
elapsedMillis mElapsedMS = 0;
LiquidCrystal_I2C mLCD(0x27,16,2);

void setup() {
    mLCD.init();
    mLCD.noCursor();
    mLCD.backlight();
    mLCD.print("Starting Up...");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    mLCD.setCursor(0, 1);
    mLCD.print("Complete!");
    delay(5000);
    mLCD.clear();
    mLCD.print("Status: OK");
}

void loop() {


    // This prevents crashes that happen when incoming usbMIDI is ignored.
    while(usbMIDI.read()){}

}