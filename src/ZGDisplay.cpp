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

void ZGDisplay::initialize()
{
    ui.begin(LCD_CS_PIN, LCD_DC_PIN, TOUCH_CS_PIN, LCD_ORIENTATION_LANDSCAPE_4PIN_RIGHT, Arial_9_Bold);
}

void ZGDisplay::refresh()
{
    ui.getTouchEvents();
    if (ui.checkForTouchEventInRect(TOUCH_PUSHED_EVENT, 0, 0, ui.displaySpaceWidth, ui.displaySpaceHeight)){
        showDebugData = !showDebugData;
        mRedraw = true;
    }
    if (mRefreshTimer >= 100) {
        if (showDebugData) {
            printDebugData(mRedraw);
        } else {
            plotObjects(mRedraw);
        }
        mRedraw = false;
        mRefreshTimer = 0;
    } else {
        return;
    }
}

void ZGDisplay::printDebugData(bool inRedrawAll)
{
    if (inRedrawAll){
        ui.lcdClearScreen(LCD_BLACK);
        auto index = 0;
        for (const auto& category : debugCategories) {
            ui.lcdSetCursorXY(0, 0 + 12 * index);
            ui.lcdPrint(category.c_str());
            index++;
        }
    }

    printDebugValue(mLidar->getSamplesPerSecond(), 0);
    printDebugValue(mLidar->getBufferSize(), 1);
    printDebugValue(mLidar->getTotalLatency(), 2);
    printDebugValue(mLidar->getProcessingLatency(), 3);
    printDebugValue(static_cast<int>(mObjectTracker->getObjects().size()), 4);
}

void ZGDisplay::printDebugValue(int inValue, int inLine)
{
    auto cursor_x = ui.lcdStringWidthInPixels(debugCategories[inLine].c_str());
    auto cursor_y = inLine * 12;
    auto string_cast = static_cast<String>(inValue);
    auto print_val = string_cast.c_str();
    auto width = ui.lcdStringWidthInPixels(print_val);
    auto height = 12;

    ui.lcdSetCursorXY(cursor_x, cursor_y);
    ui.lcdDrawFilledRectangle(cursor_x, cursor_y, width, height, LCD_BLACK);
    ui.lcdPrint(print_val);
}

void ZGDisplay::plotObjects(bool inRedrawAll)
{
    auto width = 320;
    auto height = 240;
    auto center_x = width / 2;
    auto center_y = height / 2;

    if (inRedrawAll) {
        ui.lcdClearScreen(LCD_BLACK);

    } else { // Erase previous data
        auto clusters = mDisplayedClusters;
        for (const auto &cluster: clusters) {
            auto object_color = LCD_BLACK;
            for (auto point: cluster) {
                ui.lcdDrawFilledCircle(center_x + static_cast<int>(point.x), center_y + static_cast<int>(point.y), 1,
                                       object_color);
            }
        }
        auto objects = mDisplayedObjects;
        for (auto object: objects) {
            ui.lcdDrawFilledCircle(center_x + static_cast<int>(object.x), center_y + static_cast<int>(object.y), 4,
                                   LCD_BLACK);
        }
    }

    ui.lcdDrawFilledCircle(center_x, center_y, 2, LCD_RED);

    // Paint new data
    auto clusters = mObjectTracker->getClusters();
    auto index = 0;
    for (const auto& cluster : clusters) {
        auto object_color = colorArray[index];
        for (auto point : cluster) {
            ui.lcdDrawFilledCircle(center_x + static_cast<int>(point.x), center_y + static_cast<int>(point.y), 1, object_color);
        }
        index++;
        if (index > 5) {
            index = 0;
        }
    }
    auto objects = mObjectTracker->getObjects();
    for (auto object :objects){
        ui.lcdDrawFilledCircle(center_x + static_cast<int>(object.x), center_y + static_cast<int>(object.y), 4, LCD_RED);
    }

    // Save for next frame
    mDisplayedClusters = clusters;
    mDisplayedObjects = objects;
}
