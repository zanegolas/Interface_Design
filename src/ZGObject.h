//
// Created by Zane Golas on 4/12/23.
//

#pragma once
#include <Arduino.h>
#include "ZGConversionHelpers.h"


class ZGObject {
public:

    ZGObject(float inX, float inY);

    ~ZGObject();

    void updatePoint(ZGPoint inPoint);

    const float& getX() const;

    const float& getY() const;

    void flagForRemoval(bool inShouldRemove);

    const bool& getShouldRemove() const;

private:

    float mX = 0;
    float mY = 0;
    float mSpeed = 0;
    float mAngle = 0;
    float mAngleSpeed = 0;
    float mDistance = 0;
    float mDistanceSpeed = 0;
    bool mRemoveFlag = false;

    elapsedMillis mSpeedTracker = 0;

    static constexpr double midi_factor = 12. / 360.;
    static constexpr float mod_factor = 127.f / 150.f;

    //MIDI
    int currentMidiNote = 0;
    int newMidiNote = 0;
    int mMidiChannel = 1;
    int modValue = 0;

    static int _assignMidiChannel();

    void _calculateMidi();

    void _playMidi();

};

