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
    const uint16_t aerospace_orange = 0xFA80;
    const uint16_t ghost_white = 0xFFDF;
    const uint16_t ubus_green = 0x0744;
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

    BUTTON_EXTENDED mMenuButton {"Menu", width - 34, height - 20, 68 , 40, LCD_BLACK, aerospace_orange, aerospace_orange, ghost_white, Arial_10_Bold};

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



