//
// ZGObjectTracker.h
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//
#include <Arduino.h>
#include <rplidar_driver_impl.h>
#include <vector>
#include <cmath>

#pragma once

struct ReportLine
{
    char data[80];
};

struct Point {
    float x;
    float y;
};

struct ZGPolarData {
    float angle = 0;
    float distance = 0;
};

struct ZGObject {
    float x;
    float y;
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
        modValue = 127 - (distance * mod_factor);
    }
};

class ZGObjectTracker {
public:
    /* */
    ZGObjectTracker();

    /* */
    ~ZGObjectTracker();

    /* */
    void processBuffer(std::vector<ZGPolarData>& inBuffer);

    const std::vector<std::vector<Point>> &getClusters() const;
    std::vector<ZGObject> &getObjects();

private:
    static Point polarToCartesian(float angle, float distance) {
        float angle_radians = std::fmod(angle, 360.0f) * (M_PI / 180.0f);
        Point p {};
        p.x = distance * std::cos(angle_radians);
        p.y = distance * std::sin(angle_radians);
        return p;
    }

    static ZGPolarData cartesianToPolar(Point inPointXY) {
        float distance = std::hypot(inPointXY.x, inPointXY.y);
        float angle_radians = std::atan2(inPointXY.y, inPointXY.x);
        float angle_degrees = angle_radians * (180.0f / M_PI);
        if (angle_degrees < 0.0f) {
            angle_degrees += 360.0f;
        }

        return ZGPolarData{angle_degrees, distance};
    }


    void segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<Point>>& clusters);

    static Point findClusterAverage(const std::vector<Point>& inCluster);

    void updateTrackedObjects();

    void printClusterInfo();

    float maxDistance = 150.f; //in cm
    float maxClusterDistance = 70.f;
    int minPointsPerCluster = 2;

    std::vector<std::vector<Point>> mClusters;
    std::vector<ZGObject> mTrackedObjects;

};
