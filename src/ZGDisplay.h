//
// ZGDisplay.h
// Teensy 4.1
//
// Created by Zane Golas on 4/11/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//

#include <Arduino.h>
#include "ZGObjectTracker.h"
#include "ZGLidar.h"
#include <TeensyUserInterface.h>
#include <font_ArialBold.h>
#include <memory>

#pragma once

namespace {
    const auto width = 320;
    const auto height = 240;
    constexpr auto center_x = width / 2;
    constexpr auto center_y = height / 2;
}


class ZGDisplay {
public:
    /* */
    ZGDisplay(ZGObjectTracker* inObjectTracker, ZGLidar* inLidar);

    /* */
    ~ZGDisplay();

    void initialize(TeensyUserInterface& inUI) const;

    void refresh(TeensyUserInterface& inUI, MENU_ITEM* inMainMenu);

    void printDebugData(TeensyUserInterface& inUI, bool inRedrawAll = false);

    void printDebugValue(int inValue, int inLine, TeensyUserInterface& inUI);

    void plotObjects(TeensyUserInterface& inUI, bool inRedrawAll = false);

private:
    // Pin Definitions
    const int LCD_CS_PIN = 10;
    const int LCD_DC_PIN = 9;
    const int TOUCH_CS_PIN = 8;

    bool showDebugData = false;
    bool mRedraw = true;
    elapsedMillis mRefreshTimer = 0;

    BUTTON mRangeButton {"Range",    75, height - 25,  150 , 50};
    BUTTON mDebugButton {"Debug",    width - 75, height - 25,  150 , 50};

    const uint16_t colorArray [6] {
            LCD_BLUE,
            LCD_GREEN,
            LCD_YELLOW,
            LCD_PURPLE,
            LCD_CYAN,
            LCD_ORANGE
    };

    const String debugCategories [5] {
            "Samples Per Second: ",
            "Buffer Size: ",
            "Total Latency: ",
            "Processing Latency: ",
            "Objects Tracked: "
    };

    ZGObjectTracker* mObjectTracker;
    ZGLidar* mLidar;

    std::vector<ZGPoint> mDisplayedObjects;
    std::vector<std::vector<ZGPoint>> mDisplayedClusters;

    float _getScaleFactor();

};

