//
// ZGLidar.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/11/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//


#include "ZGLidar.h"

ZGLidar::ZGLidar(ZGObjectTracker* inObjectTracker) {
    mObjectTracker = inObjectTracker;
}

ZGLidar::~ZGLidar() = default;

void ZGLidar::initialize()
{
    pinMode(LIDAR_MOTOR_PIN, OUTPUT);
    mLidar.begin();
    delay(1000);
    mLidar.startScanExpress(true, RPLIDAR_CONF_SCAN_COMMAND_EXPRESS);
    digitalWrite(LIDAR_MOTOR_PIN, HIGH); // turn on the motor
    delay(10);
}

void ZGLidar::run() {
    _readLidarBuffer();
    _processInternalBuffer();
    _updateLogs();
}

void ZGLidar::_readLidarBuffer()
{
    // Call every loop to receive data from the lidar hardware
    mLidar.loopScanExpressData();

    // Create object to hold received data for processing
    rplidar_response_measurement_node_hq_t nodes[512];
    size_t nodeCount = 512; // variable will be set to number of received measurement by reference
    u_result ans = mLidar.grabScanExpressData(nodes, nodeCount);
    if (IS_OK(ans)){
        // If the data is valid, write all samples with a quality greater than 0 to processing buffer
        for (size_t i = 0; i < nodeCount; ++i){
            // Trigger processing when new scan flag is received
            if (nodes[i].flag == 1 || mPointBuffer.size() >= 512) {
                mReadyToProcess = true;
            }

            if (nodes[i].quality == 0) {
                continue;
            } else {
                ZGPolarData p;
                p.distance = nodes[i].dist_mm_q2 / 10.f / (1<<2); //cm
                p.angle = nodes[i].angle_z_q14 * 90.f / (1<<14); //degrees
                mPointBuffer.push_back(p);
                mSampleCount++;
            }
        }
    }
}

void ZGLidar::_processInternalBuffer() {
    // Process sample buffer when a single scan is complete and generate latency report strings
    if (mReadyToProcess) {
        mProcessWait = 0;
        mBufferSize = static_cast<int>(mPointBuffer.size());
        mObjectTracker->processBuffer(mPointBuffer);
        mProcessingLatency = static_cast<int>(mProcessWait);
        mTotalLatency =static_cast<int>(mLatency);
        mLatency = 0;
        mReadyToProcess = false;
    }
}

void ZGLidar::_updateLogs() {
    // Measure total samples processed every second and generate report string
    if (mTimer >= 1000) {
        mSamplesPerSecond = mSampleCount;
        mSampleCount = 0;
        mTimer = 0;
    }
}

const int& ZGLidar::getSamplesPerSecond() const {
    return mSamplesPerSecond;
}

const int &ZGLidar::getTotalLatency() const {
    return mTotalLatency;
}

const int &ZGLidar::getProcessingLatency() const {
    return mProcessingLatency;
}

const int &ZGLidar::getBufferSize() const {
    return mBufferSize;
}
