//
// Created by Zane Golas on 4/12/23.
//

#include <Arduino.h>
#include <cmath>
#include <algorithm>

#pragma once

struct ZGPoint {
    float x;
    float y;
};

struct ZGPolarData {
    float angle = 0;
    float distance = 0;
};

namespace ZGConversionHelpers {

    /**
     * @brief Utility function to convert cartesian data to polar
     * @param inPointXY ZGPoint X/Y data
     * @return ZGPolarData with angles in degrees and distance in centimeters
     */
    static inline ZGPolarData cartesianToPolar(ZGPoint inPointXY) {
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
    static inline ZGPolarData cartesianToPolar(float inX, float inY) {
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
    static inline ZGPoint polarToCartesian(ZGPolarData inPolarCoords) {
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
    static inline ZGPoint polarToCartesian(float inAngleDegrees, float inDistanceCm) {
        float angle_radians = std::fmod(inAngleDegrees, 360.0f) * (M_PI / 180.0f);
        ZGPoint p {};
        p.x = inDistanceCm * std::cos(angle_radians);
        p.y = inDistanceCm * std::sin(angle_radians);
        return p;
    }
}
