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
    float x;
    float y;
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
    static Point polarToCartesian(float angle, float distance) {
        float angle_radians = std::fmod(angle, 360.0f) * (M_PI / 180.0f);
        Point p {};
        p.x = distance * std::cos(angle_radians);
        p.y = distance * std::sin(angle_radians);
        return p;
    }

    void segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<Point>>& clusters);

    static Point findClusterAverage(const std::vector<Point>& inCluster);

    void printClusterInfo();

    float maxDistance = 200.f; //in cm
    float maxClusterDistance = 50.f;
    int minPointsPerCluster = 2;

    std::vector<std::vector<Point>> mClusters;

};
