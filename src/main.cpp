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


// Pin Definitions
const int LCD_CS_PIN = 10;
const int LCD_DC_PIN = 9;
const int TOUCH_CS_PIN = 8;


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