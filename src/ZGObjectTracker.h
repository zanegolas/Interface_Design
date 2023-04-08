//
// ZGObjectTracker.h
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//
#include <Arduino.h>
#include <rplidar_driver_impl.h>

#pragma once

struct ZGPointData {
    float angle = 0;
    float distance = 0;
};

struct ZGObject {
    float startAngle = 0;
    float startDistance = 0;
    float endAngle = 0;
    float endDistance = 0;
    float angle = 0;
    float distance = 0;
    bool updated = false;
    int ttl = 0;
};

class ZGObjectTracker {
public:
    /* */
    ZGObjectTracker();

    /* */
    ~ZGObjectTracker();

    /* */
    void processBuffer(ZGPointData (&inResponseBuffer)[1024], int &inBufferSize);

private:
    int mTrackingCount = 0;
    ZGObject mTrackedObjects[8];

    static void _processAverages(ZGObject (&inObjectArray)[8], int inSize);

};
