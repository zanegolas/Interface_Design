#include "MIDIpot.h"

// constructors
MIDIpot::MIDIpot(){};

MIDIpot::MIDIpot(int p, byte num){
  pinMode(p, INPUT);
  pin = p;
  number = num;
  value = 0;
  mode = false;
  killSwitch = 0;
  inLo = 0;
  inHi = 1023;
  outLo = 0;
  outHi = 127;
  invert = outLo > outHi;

  // Sets the interval at which alalog signals will actually register.
  divider = !invert ? (inHi-inLo)/(outHi-outLo):(inHi-inLo)/(outLo-outHi);
  divider = divider < 1 ? 1 : divider; // Allows analog range < 127 (NOT GOOD!)
};

MIDIpot::MIDIpot(int p, byte num, byte k){
  pinMode(p, INPUT);
  pin = p;
  number = num;
  value = 0;
  mode = true;
  killSwitch = k;
  inLo = 0;
  inHi = 1023;
  outLo = 0;
  outHi = 127;
  invert = outLo > outHi;

  // Sets the interval at which alalog signals will actually register.
  divider = !invert ? (inHi-inLo)/(outHi-outLo):(inHi-inLo)/(outLo-outHi);
  divider = divider < 1 ? 1 : divider; // Allows analog range < 127 (NOT GOOD!)
};

MIDIpot::MIDIpot(int p, byte num, byte min, byte max){
  pinMode(p, INPUT);
  pin = p;
  number = num;
  value = 0;
  mode = false;
  inLo = 0;
  inHi = 1023;
  outLo = min;
  outHi = max;
  invert = outHi < outLo;

  // Sets the interval at which alalog signals will actually register.
  divider = outHi > outLo ? (inHi-inLo)/(outHi-outLo):(inHi-inLo)/(outLo-outHi);
  divider = divider < 1 ? 1 : divider; // Allows analog range < 127 (NOT GOOD!)
};

MIDIpot::MIDIpot(int p, byte num, byte k, byte min, byte max){
  pinMode(p, INPUT);
  pin = p;
  number = num;
  value = 0;
  mode = true;
  killSwitch = k;
  inLo = 0;
  inHi = 1023;
  outLo = min;
  outHi = max;
  invert = outHi < outLo;

  // Sets the interval at which alalog signals will actually register.
  divider = outHi > outLo ? (inHi-inLo)/(outHi-outLo):(inHi-inLo)/(outLo-outHi);
  divider = divider < 1 ? 1 : divider; // Allows analog range < 127 (NOT GOOD!)
};

// destructor
MIDIpot::~MIDIpot(){
};


// returns new CC if there's enough change in the analog input; -1 otherwise
int MIDIpot::read(){
  int newValue = this->smooth(analogRead(pin), SMOOTHING);
  if (newValue >= inHi && value != outHi){ // Assign hi analog to hi MIDI
    value = outHi;
    newValue = value;
  }
  else if (newValue <= inLo && value != outLo){ // Assign low analog to low MIDI
    value = outLo;
    newValue = value;
  }
  else if (newValue % divider == 0){ // Filter intermittent values
    newValue = map(newValue, inLo, inHi, outLo, outHi);
    newValue = invert ? constrain(newValue, outHi, outLo)
                      : constrain(newValue, outLo, outHi);
    newValue = newValue == value ? -1 : newValue;
  }
  else{newValue = -1;}
  return newValue;
};


int MIDIpot::send(){
  int newValue = read();
  if (killSwitch != 0 && value == outLo && newValue > outLo){//ON before main CC
    usbMIDI.sendControlChange(killSwitch, 127, MIDIChannel);
  }

  if (newValue >= 0){
    usbMIDI.sendControlChange(number, newValue, MIDIChannel); //MAIN CC MESSAGE
    if (killSwitch != 0 && value >= outLo && newValue == outLo){//OFF after main
      usbMIDI.sendControlChange(killSwitch, 0, MIDIChannel);
    }
    value = newValue;
  }
  return newValue;
};


int MIDIpot::send(bool force){
  if (force){
    balancedValue = analogRead(pin);

    int newValue = map(balancedValue, inLo, inHi, outLo, outHi);
    newValue = invert ? constrain(newValue, outHi, outLo)
                      : constrain(newValue, outLo, outHi);

    usbMIDI.sendControlChange(number, newValue, MIDIChannel);
    return newValue;
  }
  else{ return -1; }
}


void MIDIpot::setControlNumber(byte num){ // Set the CC number.
  number = num;
};


// Set upper and lower limits for outgoing MIDI messages.
void MIDIpot::outputRange(byte min, byte max){
  outLo = min;
  outHi = max;
  // Reset the interval at which alalog signals will actually register.
  divider = !invert ? (inHi-inLo)/(outHi-outLo):(inHi-inLo)/(outLo-outHi);
  divider = divider < 1 ? 1 : divider; // Allows analog range < 127 (NOT GOOD!)
  invert = outHi < outLo; // Check again for reverse polarity.
};


// Limit the analog input to the usable range of a sensor.
void MIDIpot::inputRange(uint16_t min, uint16_t max){
  inLo = min;
  inHi = max;
  // Reset the interval at which alalog signals will actually register.
  divider = outHi > outLo ? (inHi-inLo)/(outHi-outLo):(inHi-inLo)/(outLo-outHi);
  divider = divider < 1 ? 1 : divider; // Allows analog range < 127 (NOT GOOD!)
};


void MIDIpot::setKillSwitch(byte k){
  if(k == 0){
    mode = false;
  }
  else{
    mode = true;
    killSwitch = k;
  }
};


int MIDIpot::smooth(int val, int NR){
  difference = val - balancedValue;
  buffer = val == 0 ? -NR : val == balancedValue ? buffer/2 : buffer+difference;

  if (buffer*buffer >= NR*NR){ // This works better than abs(buffer) for me.
    balancedValue = val;
    buffer = 0;
  }
  return balancedValue;
};

