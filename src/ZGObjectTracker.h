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
#include "ZGObject.h"

#pragma once

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
     * @brief Segments buffer of polar data into clusters using euclidean distance
     * @param inBuffer Buffer will be cleared at the end of the function
     * @param inClusters Vector will be cleared at the beginning of the function and new data will be written
     * @see processBuffer()
     */
    void _segmentPointCloud(std::vector<ZGPolarData>& inBuffer, std::vector<std::vector<ZGPoint>>& inClusters) const;

    /**
     * @brief Utility function to find the center point given a cluster of points
     * @param inCluster A vector of points comprising a cluster
     * @return ZGPoint representing the center of the cluster
     */
    static ZGPoint _findClusterAverage(const std::vector<ZGPoint>& inCluster);

    /**
     * @brief Called after point cloud has been segmented in order to update tracked objects and send midi data
     * @see _segmentPointCloud()
     * @see processBuffer()
     */
    void _updateTrackedObjects();

    const float mMaxDistance = 150.f; //in cm
    const float mMaxClusterDistance = 70.f;
    const int mMinPointsPerCluster = 2;

    std::vector<std::vector<ZGPoint>> mClusters {};
    std::vector<ZGObject> mTrackedObjects {};

};
