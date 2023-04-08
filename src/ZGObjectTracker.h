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

struct Point {
    double x;
    double y;
};

struct ZGPolarData {
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
    void processBuffer(std::vector<ZGPolarData>& inBuffer);

private:
    int mTrackingCount = 0;
    ZGObject mTrackedObjects[8];

    Point polarToCartesian(double angle, double distance) {
        Point p;
        p.x = distance * std::cos(angle);
        p.y = distance * std::sin(angle);
        return p;
    }

    void segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<Point>>& clusters);

    double maxDistance = 30.0;
    int minPointsPerCluster = 2;

    std::vector<std::vector<Point>> mClusters;

};
