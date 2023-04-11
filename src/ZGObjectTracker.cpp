//
// ZGObjectTracker.cpp
// [TARGET]
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Minimal. All rights reserved.
//


#include <algorithm>
#include "ZGObjectTracker.h"

ZGObjectTracker::ZGObjectTracker() {

}

ZGObjectTracker::~ZGObjectTracker() {

}

void ZGObjectTracker::processBuffer(std::vector<ZGPolarData>& inBuffer)
{
    mClusters.clear();
    segmentPointCloud(inBuffer, mClusters);
    updateTrackedObjects();
    inBuffer.clear();
//    printClusterInfo();
}

void ZGObjectTracker::segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<Point>>& clusters) {
    std::vector<Point> points;
    for (auto point : inBuffer) {
        auto angle = point.angle;
        auto distance = point.distance;
        if (distance <= maxDistance) {
            Point p = polarToCartesian(angle, distance); // Convert polar to Cartesian coordinates
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
            auto distance = std::hypot(points[j].x - points[i].x, points[j].y - points[i].y);
            if (distance <= maxClusterDistance) {
                cluster.push_back(points[j]);
                visited[j] = true;
            }
        }
        if (cluster.size() >= minPointsPerCluster) {
            clusters.push_back(cluster);
        }
    }
}

void ZGObjectTracker::printClusterInfo() {
    char report[80];
    snprintf(report, sizeof(report), "Found %d clusters in specified range", mClusters.size());
    Serial.println(report);
    for(const auto& cluster : mClusters) {
        auto p = findClusterAverage(cluster);
        snprintf(report, sizeof(report), "%.2f, %.2f Cluster Contains %d Points ", p.x, p.y, cluster.size());
        Serial.println(report);
    }
}

Point ZGObjectTracker::findClusterAverage(const std::vector<Point> &inCluster) {
    auto sumX = 0.f;
    auto sumY = 0.f;

    for (const auto& point : inCluster) {
        sumX += point.x;
        sumY += point.y;
    }

    auto avgX = sumX / inCluster.size();
    auto avgY = sumY / inCluster.size();
    Point p {avgX, avgY};

    return  p;

}

const std::vector<std::vector<Point>> &ZGObjectTracker::getClusters() const {
    return mClusters;
}

void ZGObjectTracker::updateTrackedObjects() {
    // Flag existing objects to be removed if they aren't updated
    for (auto& object : mTrackedObjects) {
        object.remove = true;
    }

    // Step through found clusters
    for(auto cluster : mClusters) {
        auto center = findClusterAverage(cluster);
        auto found_match = false;
        // Try to match to an existing object;
        for (auto& object : mTrackedObjects) {
            auto distance = std::hypot(center.x - object.x, center.y - object.y);
            if (distance <= maxClusterDistance) {
                object.x = center.x;
                object.y = center.y;
                object.remove = false;
                found_match = true;
                break;
            }
        }
        // If we don't find a match we add a new tracked object;
        if (!found_match) {
            ZGObject new_object {center.x, center.y};
            mTrackedObjects.push_back(new_object);
        }
    }

    // Send note off before removal
    for (auto object : mTrackedObjects) {
        if (object.remove){
            usbMIDI.sendNoteOff(object.currentMidiNote, 120, object.midiChannel);
        }
    }

    // Remove objects that didn't find a match by flag
    mTrackedObjects.erase(std::remove_if(mTrackedObjects.begin(), mTrackedObjects.end(), [](ZGObject const& e){ return e.remove; }),
              mTrackedObjects.end());

    // Update polar info
    for (auto& object : mTrackedObjects) {
        auto polar = cartesianToPolar(Point{object.x, object.y});
        object.angle = polar.angle;
        object.distance = polar.distance;
        object.calculateMidi();
    }

}

std::vector<ZGObject> &ZGObjectTracker::getObjects() {
    return mTrackedObjects;
}
