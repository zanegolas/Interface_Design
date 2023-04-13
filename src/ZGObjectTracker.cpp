//
// ZGObjectTracker.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include "ZGObjectTracker.h"

ZGObjectTracker::ZGObjectTracker() = default;

ZGObjectTracker::~ZGObjectTracker() = default;

void ZGObjectTracker::processBuffer(std::vector<ZGPolarData>& inBuffer)
{
    mClusters.clear();
    _segmentPointCloud(inBuffer, mClusters);
    _updateTrackedObjects();
    inBuffer.clear();
}

void ZGObjectTracker::_segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<ZGPoint>>& inClusters) const {
    std::vector<ZGPoint> points;
    for (auto point : inBuffer) {
        auto angle = point.angle;
        auto distance = point.distance;
        if (distance <= mMaxDistance) {
            ZGPoint p = ZGConversionHelpers::polarToCartesian(angle, distance); // Convert polar to Cartesian coordinates
            points.push_back(p);
        }
    }

    std::vector<bool> visited(points.size(), false);
    for (int i = 0; i < static_cast<int>(points.size()); i++) {
        if (visited[i]) {
            continue;
        }
        std::vector<ZGPoint> cluster;
        cluster.push_back(points[i]);
        visited[i] = true;
        for (int j = i + 1; j < static_cast<int>(points.size()); j++) {
            if (visited[j]) {
                continue;
            }
            auto distance = std::hypot(points[j].x - points[i].x, points[j].y - points[i].y);
            if (distance <= mMaxClusterDistance) {
                cluster.push_back(points[j]);
                visited[j] = true;
            }
        }
        if (static_cast<int>(cluster.size()) >= mMinPointsPerCluster) {
            inClusters.push_back(cluster);
        }
    }
}

ZGPoint ZGObjectTracker::_findClusterAverage(const std::vector<ZGPoint> &inCluster) {
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

void ZGObjectTracker::_updateTrackedObjects() {
    // Flag existing objects to be removed if they aren't updated
    for (auto& object : mTrackedObjects) {
        object.flagForRemoval(true);
    }

    // Step through found clusters
    for(const auto& cluster : mClusters) {
        auto center = _findClusterAverage(cluster);
        auto found_match = false;
        // Try to match to an existing object;
        for (auto& object : mTrackedObjects) {
            auto distance = std::hypot(center.x - object.getX(), center.y - object.getY());
            if (distance <= mMaxClusterDistance) {
                object.updatePoint(center);
                found_match = true;
                break;
            }
        }
        // If we don't find a match we add a new tracked object;
        if (!found_match) {
            ZGObject new_object(center.x, center.y);
            mTrackedObjects.push_back(new_object);
        }
    }

    // Remove objects that didn't find a match by flag
    mTrackedObjects.erase(std::remove_if(mTrackedObjects.begin(), mTrackedObjects.end(), [](const ZGObject& e){ return e.requestToRemove(); }),
              mTrackedObjects.end());
}

const std::vector<ZGObject> &ZGObjectTracker::getObjects() const {
    return mTrackedObjects;
}
