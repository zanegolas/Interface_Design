//
// ZGObjectTracker.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <algorithm>
#include "ZGObjectTracker.h"

ZGObjectTracker::ZGObjectTracker() = default;

ZGObjectTracker::~ZGObjectTracker() = default;

void ZGObjectTracker::processBuffer(std::vector<ZGPolarData>& inBuffer)
{
    mClusters.clear();
    segmentPointCloud(inBuffer, mClusters);
    updateTrackedObjects();
    inBuffer.clear();
}

void ZGObjectTracker::segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<ZGPoint>>& inClusters) const {
    std::vector<ZGPoint> points;
    for (auto point : inBuffer) {
        auto angle = point.angle;
        auto distance = point.distance;
        if (distance <= mMaxDistance) {
            ZGPoint p = polarToCartesian(angle, distance); // Convert polar to Cartesian coordinates
            points.push_back(p);
        }
    }

    std::vector<bool> visited(points.size(), false);
    for (int i = 0; i < points.size(); i++) {
        if (visited[i]) {
            continue;
        }
        std::vector<ZGPoint> cluster;
        cluster.push_back(points[i]);
        visited[i] = true;
        for (int j = i + 1; j < points.size(); j++) {
            if (visited[j]) {
                continue;
            }
            auto distance = std::hypot(points[j].x - points[i].x, points[j].y - points[i].y);
            if (distance <= mMaxClusterDistance) {
                cluster.push_back(points[j]);
                visited[j] = true;
            }
        }
        if (cluster.size() >= mMinPointsPerCluster) {
            inClusters.push_back(cluster);
        }
    }
}

__attribute__((unused)) void ZGObjectTracker::printClusterInfo() {
    char report[80];
    snprintf(report, sizeof(report), "Found %d clusters in specified range", mClusters.size());
    Serial.println(report);
    for(const auto& cluster : mClusters) {
        auto p = findClusterAverage(cluster);
        snprintf(report, sizeof(report), "%.2f, %.2f Cluster Contains %d Points ", p.x, p.y, cluster.size());
        Serial.println(report);
    }
}

ZGPoint ZGObjectTracker::findClusterAverage(const std::vector<ZGPoint> &inCluster) {
    auto sumX = 0.f;
    auto sumY = 0.f;

    for (const auto& point : inCluster) {
        sumX += point.x;
        sumY += point.y;
    }

    auto avgX = sumX / static_cast<float>(inCluster.size());
    auto avgY = sumY / static_cast<float>(inCluster.size());
    ZGPoint p {avgX, avgY};

    return  p;

}

const std::vector<std::vector<ZGPoint>> &ZGObjectTracker::getClusters() const {
    return mClusters;
}

void ZGObjectTracker::updateTrackedObjects() {
    // Flag existing objects to be removed if they aren't updated
    for (auto& object : mTrackedObjects) {
        object.remove = true;
    }

    // Step through found clusters
    for(const auto& cluster : mClusters) {
        auto center = findClusterAverage(cluster);
        auto found_match = false;
        // Try to match to an existing object;
        for (auto& object : mTrackedObjects) {
            auto distance = std::hypot(center.x - object.x, center.y - object.y);
            if (distance <= mMaxClusterDistance) {
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
            new_object.midiChannel = _assignMidiChannel();
            mTrackedObjects.push_back(new_object);
        }
    }

    // Update polar info
    for (auto& object : mTrackedObjects) {
        auto polar = cartesianToPolar(ZGPoint{object.x, object.y});
        object.angle = polar.angle;
        object.distance = polar.distance;
    }

    // Update and send midi signal;
    for (auto& object : mTrackedObjects) {
        object.playMidi();
    }

    // Remove objects that didn't find a match by flag
    mTrackedObjects.erase(std::remove_if(mTrackedObjects.begin(), mTrackedObjects.end(), [](ZGObject const& e){ return e.remove; }),
              mTrackedObjects.end());
}

const std::vector<ZGObject> &ZGObjectTracker::getObjects() const {
    return mTrackedObjects;
}

ZGPoint ZGObjectTracker::polarToCartesian(float inAngleDegrees, float inDistanceCm) {
    float angle_radians = std::fmod(inAngleDegrees, 360.0f) * (M_PI / 180.0f);
    ZGPoint p {};
    p.x = inDistanceCm * std::cos(angle_radians);
    p.y = inDistanceCm * std::sin(angle_radians);
    return p;
}

ZGPolarData ZGObjectTracker::cartesianToPolar(ZGPoint inPointXY) {
    float distance = std::hypot(inPointXY.x, inPointXY.y);
    float angle_radians = std::atan2(inPointXY.y, inPointXY.x);
    float angle_degrees = angle_radians * (180.0f / M_PI);
    if (angle_degrees < 0.0f) {
        angle_degrees += 360.0f;
    }

    return ZGPolarData{angle_degrees, distance};
}

int ZGObjectTracker::_assignMidiChannel() {
    mMidiIndex += 1;
    if (mMidiIndex > 16) {
        mMidiIndex = 2;
    }
    return mMidiIndex;
}
