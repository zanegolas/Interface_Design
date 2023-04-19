//
// ZGLidar.h
// Teensy 4.1
//
// Created by Zane Golas on 4/11/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <Arduino.h>
#include <rplidar_driver_impl.h>
#include <vector>
#include "ZGObjectTracker.h"

#pragma once


class ZGLidar {
public:
    /* */
    explicit ZGLidar(ZGObjectTracker* inObjectTracker);

    /* */
    ~ZGLidar();

    void initialize();

    void run();

    const int& getSamplesPerSecond() const;

    const int& getTotalLatency() const;

    const int& getProcessingLatency() const;

    const int& getBufferSize() const;

private:

    void _readLidarBuffer();

    void _processInternalBuffer();

    void _updateLogs();

    const int LIDAR_MOTOR_PIN = 2;
    RPLidar mLidar {};

    elapsedMillis mTimer = 0;
    elapsedMillis mProcessWait = 0;
    elapsedMillis mLatency = 0;

    int mSampleCount = 0;
    int mSamplesPerSecond = 0;
    int mBufferSize = 0;
    int mTotalLatency = 0;
    int mProcessingLatency = 0;
    bool mReadyToProcess = false;

    std::vector<ZGPolarData> mPointBuffer;
    ZGObjectTracker* mObjectTracker;

};
