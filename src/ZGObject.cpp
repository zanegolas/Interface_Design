//
// Created by Zane Golas on 4/12/23.
//

#include "ZGObject.h"

namespace {
    int MIDI_INDEX = 1;
}

ZGObject::ZGObject(float inX, float inY) {
    mX = inX;
    mY = inY;
    mMidiChannel = _assignMidiChannel();
    auto polar = ZGConversionHelpers::cartesianToPolar(inX, inY);
    mAngle = polar.angle;
    mDistance = polar.distance;

    mSpeedTracker = 0;

    _playMidi();
}

ZGObject::~ZGObject() {
    usbMIDI.sendNoteOff(currentMidiNote, 127, mMidiChannel);
}

void ZGObject::updatePoint(ZGPoint inPoint) {
    auto polar = ZGConversionHelpers::cartesianToPolar(inPoint);
    auto cart_distance = std::hypot(inPoint.x - mX, inPoint.y - mY);
    auto angle_distance = polar.angle - mAngle;
    auto distance = polar.distance - mDistance;

    mX = inPoint.x;
    mY = inPoint.y;
    mAngle = polar.angle;
    mDistance = polar.distance;
    mRemoveFlag = false;

    if (mSpeedTracker != 0) {
        auto time = static_cast<float>(1000 / mSpeedTracker);
        mSpeedTracker = 0;
        mSpeed = time * cart_distance;
        mAngleSpeed = time * angle_distance;
        mDistanceSpeed = time * distance;
    }

    _playMidi();
}

void ZGObject::flagForRemoval(bool inShouldRemove) {
    mRemoveFlag = inShouldRemove;
}

const bool & ZGObject::getShouldRemove() const {
    return mRemoveFlag;
}

int ZGObject::_assignMidiChannel() {
    MIDI_INDEX += 1;
    if (MIDI_INDEX > 16) {
        MIDI_INDEX = 2;
    }
    return MIDI_INDEX;
}

void ZGObject::_calculateMidi(){
    newMidiNote = static_cast<int>(mAngle * midi_factor) + 50;
    modValue = 127 - static_cast<int>(mDistance * mod_factor);
}

void ZGObject::_playMidi(){
    _calculateMidi();

    if (newMidiNote != currentMidiNote) {
        usbMIDI.sendNoteOff(currentMidiNote, 127, mMidiChannel);
        currentMidiNote = newMidiNote;
        usbMIDI.sendNoteOn(currentMidiNote, 127, mMidiChannel);
    }
    usbMIDI.sendControlChange( 1, modValue, mMidiChannel);
}

const float &ZGObject::getX() const {
    return mX;
}

const float &ZGObject::getY() const {
    return mY;
}
