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
#include "assets/font_Inter.h"
#include "assets/font_ChakraPetch-SemiBold.h"
#include <memory>
#include "assets/SettingsButtonDefault.c"
#include "assets/SettingsButtonPressed.c"
#include "assets/BackButtonDefault.c"
#include "assets/BackButtonPressed.c"
#include "assets/SelectButtonDefault.c"
#include "assets/SelectButtonPressed.c"
#include "assets/OkButtonDefault.c"
#include "assets/SettingsBackplate.c"

#pragma once

namespace {
    const auto width = 320;
    const auto height = 240;
    constexpr auto center_x = width / 2;
    constexpr auto center_y = height / 2;
    const uint16_t aerospace_orange = 0xFA80;
    const uint16_t ghost_white = 0xFFDF;
    const uint16_t ubus_green = 0x0744;
    const uint16_t cadet_gray = 0x9576;
    const uint16_t dim_gray = 0x6B4C;
}


class ZGDisplay {
public:
    /* */
    ZGDisplay(ZGObjectTracker* inObjectTracker, ZGLidar* inLidar);

    /* */
    ~ZGDisplay();

    void initialize();

    void refresh();

    void printDebugData(TeensyUserInterface& inUI, bool inRedrawAll = false);

    void printDebugValue(int inValue, int inLine, TeensyUserInterface& inUI);

    void plotObjects(TeensyUserInterface& inUI, bool inRedrawAll = false);

private:
    // Pin Definitions
    const int LCD_CS_PIN = 10;
    const int LCD_DC_PIN = 9;
    const int TOUCH_CS_PIN = 8;

    TeensyUserInterface mUI;

    bool showDebugData = false;
    bool mRedraw = true;
    elapsedMillis mRefreshTimer = 0;

    BUTTON_IMAGE mMenuButton {"",SettingsButtonDefault, SettingsButtonPressed, 287, 213, 40 , 40, ghost_white, Inter_12};
    BUTTON_IMAGE mBackButton {"",BackButtonDefault, BackButtonPressed, 44, height - 14, 88 , 29, ghost_white, Inter_12};
    BUTTON_IMAGE mOkButton {"OK", OkButtonDefault, SelectButtonPressed, 82, 203, 144 , 50, ghost_white, ChakraPetchSemiBold_12};
    BUTTON_IMAGE mCancelButton {"CANCEL", SelectButtonDefault, SelectButtonPressed, 237, 203, 144 , 50, ghost_white, ChakraPetchSemiBold_12};

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

    void showMainMenu(TeensyUserInterface& ui);
    void showAbout(TeensyUserInterface& ui);
    void showScan();
    void showDisplay();
    void showMidi();



};



