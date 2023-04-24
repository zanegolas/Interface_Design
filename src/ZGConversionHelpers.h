//
// ZGConversionHelpers.h
// Teensy 4.1
//
// Created by Zane Golas on 4/12/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <Arduino.h>
#include <cmath>
#include <algorithm>

#pragma once

enum class ClusterID {
    ERROR = -2,
    NOISE,
    UNCLASSIFIED
};

struct ZGPoint {
    float x;
    float y;
    int clusterID = 0;
};

struct ZGPolarData {
    float angle = 0;
    float distance = 0;
};

namespace ZGConversionHelpers {

    inline float getDistance(const ZGPoint& inPointA, const ZGPoint& inPointB) {
        return std::hypot(inPointA.x - inPointB.x, inPointA.y - inPointB.y);
    }

    /**
     * @brief Utility function to convert cartesian data to polar
     * @param inPointXY ZGPoint X/Y data
     * @return ZGPolarData with angles in degrees and distance in centimeters
     */
    inline ZGPolarData cartesianToPolar(ZGPoint inPointXY) {
        float distance = std::hypot(inPointXY.x, inPointXY.y);
        float angle_radians = std::atan2(inPointXY.y, inPointXY.x);
        float angle_degrees = angle_radians * (180.0f / M_PI);
        if (angle_degrees < 0.0f) {
            angle_degrees += 360.0f;
        }

        return ZGPolarData{angle_degrees, distance};
    }

    /**
     * @brief  Utility function to convert cartesian data to polar
     * @param inX X Coordinate
     * @param inY Y Coordinate
     * @return ZGPolarData with angles in degrees and distance in centimeters
     */
    inline ZGPolarData cartesianToPolar(float inX, float inY) {
        float distance = std::hypot(inX, inY);
        float angle_radians = std::atan2(inY, inX);
        float angle_degrees = angle_radians * (180.0f / M_PI);
        if (angle_degrees < 0.0f) {
            angle_degrees += 360.0f;
        }

        return ZGPolarData{angle_degrees, distance};
    }

    /**
     * @brief Utility function to convert polar to cartesian data
     * @param inPolarCoords ZGPolarData pair of distance in cm and angle in degrees
     * @return X/Y point object
     */
    inline ZGPoint polarToCartesian(ZGPolarData inPolarCoords) {
        float angle_radians = std::fmod(inPolarCoords.angle, 360.0f) * (M_PI / 180.0f);
        ZGPoint p {};
        p.x = inPolarCoords.distance * std::cos(angle_radians);
        p.y = inPolarCoords.distance * std::sin(angle_radians);
        return p;
    }

    /**
     * @brief Utility function to convert polar to cartesian data
     * @param inAngleDegrees Angle in degrees
     * @param inDistanceCm Distance in centimeters
     * @return X/Y point object
     */
    inline ZGPoint polarToCartesian(float inAngleDegrees, float inDistanceCm) {
        float angle_radians = std::fmod(inAngleDegrees, 360.0f) * (M_PI / 180.0f);
        ZGPoint p {};
        p.x = inDistanceCm * std::cos(angle_radians);
        p.y = inDistanceCm * std::sin(angle_radians);
        return p;
    }

    inline int convertToMajor(int inDegree) {
        int noteOffset = 0;
        switch (inDegree) {
            case 1: // first degree of the scale
                noteOffset = 0;
                break;
            case 2: // second degree of the scale
                noteOffset = 2;
                break;
            case 3: // third degree of the scale
                noteOffset = 4;
                break;
            case 4: // fourth degree of the scale
                noteOffset = 5;
                break;
            case 5: // fifth degree of the scale
                noteOffset = 7;
                break;
            case 6: // sixth degree of the scale
                noteOffset = 9;
                break;
            case 7: // seventh degree of the scale
                noteOffset = 11;
                break;
            default:
                return 0;
        }
        return noteOffset;
    }

    inline int convertToMinor(int inDegree) {
        int noteOffset = 0;
        switch (inDegree) {
            case 1: // first degree of the scale
                noteOffset = 0;
                break;
            case 2: // second degree of the scale
                noteOffset = 2;
                break;
            case 3: // third degree of the scale
                noteOffset = 3;
                break;
            case 4: // fourth degree of the scale
                noteOffset = 5;
                break;
            case 5: // fifth degree of the scale
                noteOffset = 7;
                break;
            case 6: // sixth degree of the scale
                noteOffset = 8;
                break;
            case 7: // seventh degree of the scale
                noteOffset = 10;
                break;
            default:
                return 0;
        }
        return noteOffset;
    }

    const String noteStrings [12] {
            "C",
            "C#/Db",
            "D",
            "D#/Eb",
            "E",
            "F",
            "F#/Gb",
            "G",
            "G#/Ab",
            "A",
            "A#/Bb",
            "B"
    };

    const String scaleStrings [3] {
            "Chromatic",
            "M",
            "m"
    };


}
