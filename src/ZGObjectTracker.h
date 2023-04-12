//
// ZGObjectTracker.h
// Teensy 4.1
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <Arduino.h>
#include <rplidar_driver_impl.h>
#include <vector>
#include <cmath>

#pragma once

/*
 * Data Structures
 */

struct ZGPoint {
    float x;
    float y;
};

struct ZGPolarData {
    float angle = 0;
    float distance = 0;
};

struct ZGObject {
    float x = 0;
    float y = 0;
    float angle = 0;
    float distance = 0;
    bool remove = false;

    static constexpr double midi_factor = 12. / 360.;
    static constexpr float mod_factor = 127.f / 150.f;

    //MIDI
    int currentMidiNote = 0;
    int newMidiNote = 0;
    int midiChannel = 1;
    int modValue = 0;

    void calculateMidi(){
        newMidiNote = static_cast<int>(angle * midi_factor) + 50;
        modValue = 127 - static_cast<int>(distance * mod_factor);
    }

    void playMidi(){
        if (remove) {
            usbMIDI.sendNoteOff(currentMidiNote, 127, midiChannel);
            return;
        }

        calculateMidi();

        if (newMidiNote != currentMidiNote) {
            usbMIDI.sendNoteOff(currentMidiNote, 127, midiChannel);
            currentMidiNote = newMidiNote;
            usbMIDI.sendNoteOn(currentMidiNote, 127, midiChannel);
        }
        usbMIDI.sendControlChange(1, modValue, midiChannel);
    }

};

/**
 * @brief Used for processing point cloud buffers to create clusters which are then matched to internally tracked objects or
 * used to create a new object if no match is found. This class also updates and sends Midi data derived from positional
 * information of the tracked object.
 */

class ZGObjectTracker {
public:
    /* */
    ZGObjectTracker();

    /* */
    ~ZGObjectTracker();

    /**
     * @brief Called once per 360 degree scan to find objects in the point cloud buffer and send midi data
     * @param inBuffer Polar point data from the lidar. This function will clear the buffer when processing is finished
     * */
    void processBuffer(std::vector<ZGPolarData>& inBuffer);

    /**
     * @return A const reference to the clusters found by the object tracker. Intended for plotting on LCD.
     */
    const std::vector<std::vector<ZGPoint>> &getClusters() const;

    /**
     * @return A const reference to the objects currently tracked. Intended for plotting on LCD.
     */
    const std::vector<ZGObject> &getObjects() const;

private:

    /**
     * @brief Utility function to convert polar to cartesian data
     * @param inAngleDegrees Angle in degrees
     * @param inDistanceCm Distance in centimeters
     * @return X/Y point object
     */
    static ZGPoint polarToCartesian(float inAngleDegrees, float inDistanceCm);

    /**
     * @brief Utility function to convert cartesian data to polar
     * @param inPointXY ZGPoint X/Y data
     * @return ZGPolarData with angles in degrees and distance in centimeters
     */
    static ZGPolarData cartesianToPolar(ZGPoint inPointXY);

    /**
     * @brief Segments buffer of polar data into clusters using euclidean distance
     * @param inBuffer Buffer will be cleared at the end of the function
     * @param inClusters Vector will be cleared at the beginning of the function and new data will be written
     * @see processBuffer()
     */
    void segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<ZGPoint>>& inClusters) const;

    /**
     * @brief Utility function to find the center point given a cluster of points
     * @param inCluster A vector of points comprising a cluster
     * @return ZGPoint representing the center of the cluster
     */
    static ZGPoint findClusterAverage(const std::vector<ZGPoint>& inCluster);

    /**
     * @brief Called after point cloud has been segmented in order to update tracked objects and send midi data
     * @see segmentPointCloud()
     * @see processBuffer()
     */
    void updateTrackedObjects();

    /**
     * @brief Utility class for printing debug data to the usb serial. Use if you need faster data and logging.
     * @note Should be called at the end of the process buffer function.
     * @see processBuffer()
     */
    __attribute__((unused)) void printClusterInfo();


    int _assignMidiChannel();

    const float mMaxDistance = 150.f; //in cm
    const float mMaxClusterDistance = 70.f;
    const int mMinPointsPerCluster = 2;

    int mMidiIndex = 0;

    std::vector<std::vector<ZGPoint>> mClusters;
    std::vector<ZGObject> mTrackedObjects;

};
