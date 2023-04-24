//
// ZGDisplay.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/11/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//


#include "ZGDisplay.h"

ZGDisplay::ZGDisplay(ZGObjectTracker *inObjectTracker, ZGLidar *inLidar)
{
    mObjectTracker = inObjectTracker;
    mLidar = inLidar;
}

ZGDisplay::~ZGDisplay() = default;

void ZGDisplay::initialize(TeensyUserInterface& inUI) const
{
    inUI.begin(LCD_CS_PIN, LCD_DC_PIN, TOUCH_CS_PIN, LCD_ORIENTATION_LANDSCAPE_4PIN_LEFT, Arial_9_Bold);
}

void ZGDisplay::refresh(TeensyUserInterface& inUI, MENU_ITEM* inMainMenu)
{
    inUI.getTouchEvents();
    if(inUI.checkForButtonClicked(mMenuButton)){
        mLidar->pause();
        inUI.displayAndExecuteMenu(inMainMenu);
        mLidar->resume();
        mRedraw = true;
    }
    if (mRefreshTimer >= 100) {
        if (showDebugData) {
            printDebugData(inUI, mRedraw);
        } else {
            plotObjects(inUI, mRedraw);
        }
        mRedraw = false;
        mRefreshTimer = 0;
    } else {
        return;
    }
}

void ZGDisplay::printDebugData(TeensyUserInterface& inUI, bool inRedrawAll)
{
    if (inRedrawAll){
        inUI.lcdClearScreen(LCD_BLACK);
        inUI.drawButton(mMenuButton);
        auto index = 0;
        for (const auto& category : debugCategories) {
            inUI.lcdSetCursorXY(0, 0 + 12 * index);
            inUI.lcdPrint(category.c_str());
            index++;
        }
    }

    printDebugValue(mLidar->getSamplesPerSecond(), 0, inUI);
    printDebugValue(mLidar->getBufferSize(), 1, inUI);
    printDebugValue(mLidar->getTotalLatency(), 2, inUI);
    printDebugValue(mLidar->getProcessingLatency(), 3, inUI);
    printDebugValue(static_cast<int>(mObjectTracker->getObjects().size()), 4, inUI);
}

void ZGDisplay::printDebugValue(int inValue, int inLine, TeensyUserInterface& inUI)
{
    auto cursor_x = inUI.lcdStringWidthInPixels(debugCategories[inLine].c_str());
    auto cursor_y = inLine * 12;
    auto string_cast = static_cast<String>(inValue);
    auto print_val = string_cast.c_str();
    auto str_width = inUI.lcdStringWidthInPixels(print_val);
    auto str_height = 12;

    inUI.lcdSetCursorXY(cursor_x, cursor_y);
    inUI.lcdDrawFilledRectangle(cursor_x, cursor_y, str_width, str_height, LCD_BLACK);
    inUI.lcdPrint(print_val);
}

void ZGDisplay::plotObjects(TeensyUserInterface& inUI, bool inRedrawAll)
{
    auto scale = _getScaleFactor();

    if (inRedrawAll) {
        inUI.lcdClearScreen(LCD_BLACK);
        inUI.drawButton(mMenuButton);

    } else { // Erase previous data
        auto clusters = mDisplayedClusters;
        for (const auto &cluster: clusters) {
            auto object_color = LCD_BLACK;
            for (auto point: cluster) {
                inUI.lcdDrawFilledCircle(center_x + static_cast<int>(point.x * scale), center_y + static_cast<int>(point.y * scale), 1,
                                         object_color);
            }
        }
        auto objects = mDisplayedObjects;
        for (const auto& object: objects) {
            inUI.lcdDrawFilledCircle(center_x + static_cast<int>(object.x * scale), center_y + static_cast<int>(object.y * scale), 4,
                                     LCD_BLACK);
        }
    }

    inUI.lcdDrawFilledCircle(center_x, center_y, 2, aerospace_orange);
    inUI.lcdDrawCircle(center_x, center_y, center_y - 1, LCD_DARKGREY);

    // Paint new data
    auto clusters = mObjectTracker->getClusters();
    auto index = 0;
    for (const auto& cluster : clusters) {
        auto object_color = colorArray[index];
        for (auto point : cluster) {
            inUI.lcdDrawFilledCircle(center_x + static_cast<int>(point.x * scale), center_y + static_cast<int>(point.y * scale), 1, object_color);
        }
        index++;
        if (index > 5) {
            index = 0;
        }
    }
    auto objects = mObjectTracker->getObjects();
    mDisplayedObjects.clear();
    for (const auto& object :objects){
        inUI.lcdDrawFilledCircle(center_x + static_cast<int>(object.getX() * scale), center_y + static_cast<int>(object.getY() * scale), 4, LCD_RED);
        mDisplayedObjects.push_back(object.getPoint());
    }

    // Save for next frame
    mDisplayedClusters = clusters;
}

float ZGDisplay::_getScaleFactor() {
    return 120 / mObjectTracker->getMaxDistance() ;
}



