//
// ZGObject.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/12/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include "ZGObject.h"

namespace {
    bool isChannelAssigned[15] {
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false
    };
}

ZGObject::ZGObject(float inX, float inY, int inRoot, int inScaleType, float inDistance) {
    mX = inX;
    mY = inY;
    updateRootNote(inRoot);
    updateScaleType(inScaleType);
    updateDistance(inDistance);
    if (_assignMidiChannel()) {
        auto polar = ZGConversionHelpers::cartesianToPolar(inX, inY);
        mAngle = polar.angle;
        mDistance = polar.distance;

        mSpeedTracker = 0;

        _playMidi();
    }
}

ZGObject::~ZGObject() = default;

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
        auto time = 100.f / static_cast<float>(mSpeedTracker);
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

const bool & ZGObject::requestToRemove() const {
    if (mRemoveFlag) {
        usbMIDI.sendNoteOff(currentMidiNote, 127, mMidiChannel);
        _releaseMidiChannel();
    }
    return mRemoveFlag;
}

bool ZGObject::_assignMidiChannel() {
    for (int i = 0; i < 15; i++){
        if (!isChannelAssigned[i]) {
            isChannelAssigned[i] = true;
            mMidiChannel = i + 2;
            return true;
        }
    }
    flagForRemoval(true);
    return false;

}

void ZGObject::_releaseMidiChannel() const {
    if (mMidiChannel < 2){
        return;
    } else {
        isChannelAssigned[mMidiChannel - 2] = false;
    }
}

void ZGObject::_calculateMidi(){
    auto degree = static_cast<int>(mAngle * midi_factor);
    if (mScaleType == 1) {
        degree = ZGConversionHelpers::convertToMajor(degree);
    } else if (mScaleType == 2) {
        degree = ZGConversionHelpers::convertToMinor(degree);
    }
    newMidiNote = degree + mRootNote;
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

ZGPoint ZGObject::getPoint() const {
    return ZGPoint{mX, mY};
}

void ZGObject::updateRootNote(int inNewRoot) {
    mRootNote = inNewRoot + 48;
}

void ZGObject::updateScaleType(int inNewType) {
    if (inNewType == 0){
        midi_factor = 12. / 360.;
    } else {
        midi_factor = 7. / 360.;
    }
    mScaleType = inNewType;
}

void ZGObject::updateDistance(float inNewDistance) {
    mod_factor = 127.f / inNewDistance;
}
