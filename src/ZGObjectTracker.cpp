//
// ZGObjectTracker.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <unordered_map>
#include "ZGObjectTracker.h"

ZGObjectTracker::ZGObjectTracker() = default;

ZGObjectTracker::~ZGObjectTracker() = default;

void ZGObjectTracker::processBuffer(std::vector<ZGPolarData>& inBuffer)
{
    mClusters.clear();
    _segmentPointCloud(inBuffer);
    _updateTrackedObjects();
    inBuffer.clear();
}

void ZGObjectTracker::_segmentPointCloud(std::vector<ZGPolarData>& inBuffer) {
    mPointBuffer.clear();
    for (auto point : inBuffer) {
        auto angle = point.angle;
        auto distance = point.distance;
        if (distance <= mMaxDistance) {
            ZGPoint p = ZGConversionHelpers::polarToCartesian(angle, distance); // Convert polar to Cartesian coordinates
            mPointBuffer.push_back(p);
        }
    }

    if (USE_DBSCAN_METHOD) {
        _dbScan();
    } else {
        _euclideanScan();
    }

}

ZGPoint ZGObjectTracker::_findClusterAverage(const std::vector<ZGPoint> &inCluster)
{
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

const std::vector<std::vector<ZGPoint>> &ZGObjectTracker::getClusters() const
{
    return mClusters;
}

void ZGObjectTracker::_updateTrackedObjects()
{
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
            ZGObject new_object(center.x, center.y, mRootNote, mScaleType, mMaxDistance);
            mTrackedObjects.push_back(new_object);
        }
    }

    // Remove objects that didn't find a match by flag
    mTrackedObjects.erase(std::remove_if(mTrackedObjects.begin(), mTrackedObjects.end(), [](const ZGObject& e){ return e.requestToRemove(); }),
              mTrackedObjects.end());
}

const std::vector<ZGObject> &ZGObjectTracker::getObjects() const
{
    return mTrackedObjects;
}

using namespace std;
vector<int> ZGObjectTracker::_calculateCluster(ZGPoint point)
{
    int index = 0;
    vector<ZGPoint>::iterator iter;
    vector<int> clusterIndex;
    for( iter = mPointBuffer.begin(); iter != mPointBuffer.end(); ++iter)
    {
        if ( ZGConversionHelpers::getDistance(point, *iter) <= mEpsilon )
        {
            clusterIndex.push_back(index);
        }
        index++;
    }
    return clusterIndex;
}

int ZGObjectTracker::_expandCluster(ZGPoint& inPoint, int clusterID)
{
    vector<int> clusterSeeds = _calculateCluster(inPoint);

    if ( clusterSeeds.size() < mMinPointsPerCluster )
    {
        inPoint.clusterID = static_cast<int>(ClusterID::NOISE);
        return static_cast<int>(ClusterID::ERROR);
    }
    else
    {
        int index = 0, indexCorePoint = 0;
        vector<int>::iterator iterSeeds;
        for( iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds)
        {
            mPointBuffer.at(*iterSeeds).clusterID = clusterID;
            if (mPointBuffer.at(*iterSeeds).x == inPoint.x && mPointBuffer.at(*iterSeeds).y == inPoint.y )
            {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.erase(clusterSeeds.begin()+indexCorePoint);

        for( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i )
        {
            vector<int> clusterNeighbors = _calculateCluster(mPointBuffer.at(clusterSeeds[i]));

            if (clusterNeighbors.size() >= mMinPointsPerCluster )
            {
                vector<int>::iterator iterNeighbors;
                for (iterNeighbors = clusterNeighbors.begin(); iterNeighbors != clusterNeighbors.end(); iterNeighbors++ )
                {
                    if (static_cast<ClusterID>(mPointBuffer.at(*iterNeighbors).clusterID) == ClusterID::UNCLASSIFIED || static_cast<ClusterID>(mPointBuffer.at(
                            *iterNeighbors).clusterID) == ClusterID::NOISE )
                    {
                        if (static_cast<ClusterID>(mPointBuffer.at(*iterNeighbors).clusterID) == ClusterID::UNCLASSIFIED )
                        {
                            clusterSeeds.push_back(*iterNeighbors);
                            n = clusterSeeds.size();
                        }
                        mPointBuffer.at(*iterNeighbors).clusterID = clusterID;
                    }
                }
            }
        }

        return 1;
    }
}

int ZGObjectTracker::_dbScan() {
    int clusterID = 1;
    vector<ZGPoint>::iterator iter;
    for(iter = mPointBuffer.begin(); iter != mPointBuffer.end(); ++iter)
    {
        if ( iter->clusterID == static_cast<int>(ClusterID::UNCLASSIFIED))
        {
            if (_expandCluster(*iter, clusterID) != static_cast<int>(ClusterID::ERROR) )
            {
                clusterID += 1;
            }
        }
    }

    for (int i = 1; i < clusterID; i++) {
        std::vector<ZGPoint> cluster;
        for (const auto& point : mPointBuffer) {
            if (point.clusterID == i) {
                cluster.push_back(point);
            }
        }
       mClusters.push_back(cluster);
    }

    return 0;
}

void ZGObjectTracker::_euclideanScan()
{
    std::vector<bool> visited(mPointBuffer.size(), false);
    for (int i = 0; i < static_cast<int>(mPointBuffer.size()); i++) {
        if (visited[i]) {
            continue;
        }
        std::vector<ZGPoint> cluster;
        cluster.push_back(mPointBuffer[i]);
        visited[i] = true;
        for (int j = i + 1; j < static_cast<int>(mPointBuffer.size()); j++) {
            if (visited[j]) {
                continue;
            }
            auto distance = ZGConversionHelpers::getDistance(mPointBuffer[j], mPointBuffer[i]);
            if (distance <= mMaxClusterDistance) {
                cluster.push_back(mPointBuffer[j]);
                visited[j] = true;
            }
        }
        if (static_cast<int>(cluster.size()) >= mMinPointsPerCluster) {
            mClusters.push_back(cluster);
        }
    }
}

const float &ZGObjectTracker::getMaxDistance() const {
    return mMaxDistance;
}

void ZGObjectTracker::setMaxDistance(float inCentimeters) {
    mMaxDistance = inCentimeters;
    for (auto& object : mTrackedObjects) {
        object.updateDistance(mMaxDistance);
    }
}

const bool &ZGObjectTracker::getScanMode() const {
    return USE_DBSCAN_METHOD;
}

void ZGObjectTracker::setScanMode(bool useDBSCAN) {
    USE_DBSCAN_METHOD = useDBSCAN;
}

const int &ZGObjectTracker::getRootNote() const {
    return mRootNote;
}

void ZGObjectTracker::setRootNote(int inNote) {
    mRootNote = inNote;
    for (auto& object : mTrackedObjects) {
        object.updateRootNote(mRootNote);
    }
}

const int &ZGObjectTracker::getScaleType() const {
    return mScaleType;
}

void ZGObjectTracker::setScaleType(int inScaleType) {
    mScaleType = inScaleType;
    for (auto& object : mTrackedObjects) {
        object.updateScaleType(mScaleType);
    }
}

