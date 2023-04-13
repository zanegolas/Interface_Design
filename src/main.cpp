//
// LIDAR MIDI Interface
// Teensy 4.1
//
// Created by Zane Golas on 4/7/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <Arduino.h>
#include "ZGObjectTracker.h"
#include "ZGLidar.h"
#include "ZGDisplay.h"
#include <memory>

// Global Objects
std::unique_ptr<ZGObjectTracker> mObjectTracker;
std::unique_ptr<ZGLidar> mLidar;
std::unique_ptr<ZGDisplay> mDisplay;

/**
 * SETUP
 */

void setup() {
    mObjectTracker = std::make_unique<ZGObjectTracker>();
    mLidar = std::make_unique<ZGLidar>(mObjectTracker.get());
    mDisplay = std::make_unique<ZGDisplay>(mObjectTracker.get(), mLidar.get());

    mLidar->initialize();
    mDisplay->initialize();
    usbMIDI.begin();
}

/**
 * MAIN PROGRAM
 */

void loop() {
   mLidar->run();
   mDisplay->refresh();

    // Prevent errors when incoming usb midi buffer is ignored
    while(usbMIDI.read()){}
}
