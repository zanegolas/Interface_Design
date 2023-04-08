//
// ZGObjectTracker.cpp
// [TARGET]
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Minimal. All rights reserved.
//


#include "ZGObjectTracker.h"

ZGObjectTracker::ZGObjectTracker() {

}

ZGObjectTracker::~ZGObjectTracker() {

}

void ZGObjectTracker::processBuffer(std::vector<ZGPolarData>& inBuffer)
{
    mClusters.clear();
    segmentPointCloud(inBuffer, mClusters);
    inBuffer.clear();
    char report[80];
    snprintf(report, sizeof(report), "Found %d clusters in specified range", mClusters.size());
    Serial.println(report);

}

void ZGObjectTracker::segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<Point>>& clusters) {
    std::vector<Point> points;
    for (auto point : inBuffer) {
        double angle = point.angle;
        double distance = point.distance;
        if (distance <= maxDistance) {
            Point p = polarToCartesian(angle * M_PI / 180.0, distance); // Convert polar to Cartesian coordinates
            points.push_back(p);
        }
    }

    std::vector<bool> visited(points.size(), false);
    for (int i = 0; i < points.size(); i++) {
        if (visited[i]) {
            continue;
        }
        std::vector<Point> cluster;
        cluster.push_back(points[i]);
        visited[i] = true;
        for (int j = i + 1; j < points.size(); j++) {
            if (visited[j]) {
                continue;
            }
            double distance = std::hypot(points[j].x - points[i].x, points[j].y - points[i].y);
            if (distance <= maxDistance) {
                cluster.push_back(points[j]);
                visited[j] = true;
            }
        }
        if (cluster.size() >= minPointsPerCluster) {
            clusters.push_back(cluster);
        }
    }
}
