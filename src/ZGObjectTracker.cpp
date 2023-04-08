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

void ZGObjectTracker::processBuffer(ZGPointData (&inResponseBuffer)[1024], int &inBufferSize)
{
    bool is_tracking = false;
    ZGObject found_objects[8];
    int found_count = 0;
    int error_buffer = 5;

    for (int i = 0; i < inBufferSize; i++){
        if (!is_tracking && inResponseBuffer[i].distance > 0) {
            found_objects[found_count].startAngle = inResponseBuffer[i].angle;
            found_objects[found_count].startDistance = inResponseBuffer[i].distance;
            is_tracking = true;
            error_buffer = 5;
        } else if(error_buffer <= 0 && is_tracking) {
            found_objects[found_count].startAngle = inResponseBuffer[i - 5].angle;
            found_objects[found_count].endDistance = inResponseBuffer[i - 5].distance;
            found_objects[found_count].updated = true;
            found_objects[found_count].ttl = 2;
            is_tracking = false;
            found_count++;
        } else if (is_tracking && inResponseBuffer[i].distance == 0) {
            error_buffer--;
        }

        if (found_count >= 8) {
            Serial.println("Found objects exceeded maximum. Stopping search");
            break;
        }
    }

    Serial.print("Found Objects: ");
    Serial.println(found_count);

    _processAverages(found_objects, found_count);

    auto merged_count = 0;
    Serial.print("Merge Count: ");
    Serial.println(merged_count);

    // Update Tracked Attributes
    for (int i = 0; i < mTrackingCount; i++){
        mTrackedObjects[i].updated = false;
        mTrackedObjects[i].ttl -= 1;
    }

    auto match_count = 0;
    // Match Close Objects
    for (int i = 0; i < mTrackingCount; i++){
        for (int j = 0; j < found_count; j++) {
            float angle_change = mTrackedObjects[i].angle - found_objects[j].angle;
            if (angle_change < 0) {
                angle_change *= -1;
            }
            if (angle_change <= 20){
                mTrackedObjects[i] = found_objects[j];
                found_objects[j].updated = false;
                match_count++;
            }
        }
    }

    Serial.print("Match Count: ");
    Serial.println(match_count);
    ZGObject updated_array[8];
    int update_count = 0;
    for (auto & tracked_object : mTrackedObjects){
        if (update_count >= 8) {
            Serial.println("Updated objects exceeded maximum. Stopping update");
            break;
        }
        if (tracked_object.updated || tracked_object.ttl > 0) {
            updated_array[update_count] = tracked_object;
            update_count++;
        }
    }
    for (auto & found_object : found_objects){
        if (update_count >= 8) {
            Serial.println("Updated objects exceeded maximum. Stopping update");
            break;
        }
        if (found_object.updated) {
            updated_array[update_count] = found_object;
            update_count++;
        }
    }
    Serial.print("Tracking Updated To ");
    Serial.print(update_count);
    Serial.println(" objects");

    for (int i = 0; i < 8; i++) {
        mTrackedObjects[i] = updated_array[i];
    }
    mTrackingCount = update_count;

    char report[80];
    for (int i = 0; i < mTrackingCount; i++) {
        snprintf(report, sizeof(report), "Object %d at angle %.2f with distance of %.2f", i, mTrackedObjects[i].angle, mTrackedObjects[i].distance);
        Serial.println(report);
    }


    memset(inResponseBuffer, 0, sizeof(inResponseBuffer));
    inBufferSize = 0;


}

void ZGObjectTracker::_processAverages(ZGObject (&inObjectArray)[8], int inSize)
{
    for (int i = 0; i < inSize; i++){
        inObjectArray[i].angle = (inObjectArray[i].startAngle + inObjectArray[i].endAngle) / 2;
        inObjectArray[i].distance = (inObjectArray[i].startDistance + inObjectArray[i].endDistance) / 2;
    }
}
