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
#include "TeensyUserInterface.h"

// Global Objects
TeensyUserInterface ui;
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
    mDisplay->initialize(ui);
    usbMIDI.begin();
}

//
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];

void commandAbout();


//
// main menu, NOTE HOW 4TH COLUMN IN FIRST LINE IS SET TO "NULL", THIS DISPLAYS THE "MENU" BUTTON
//
MENU_ITEM mainMenu[] = {
        {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "Settings",         MENU_COLUMNS_1,            NULL},
        {MENU_ITEM_TYPE_COMMAND,           "About",                      commandAbout,              NULL},
        {MENU_ITEM_TYPE_END_OF_MENU,       "",                           NULL,                      NULL}
};

/**
 * MAIN PROGRAM
 */

void loop() {
   mLidar->run();
   mDisplay->refresh(ui, mainMenu);

    // Prevent errors when incoming usb midi buffer is ignored
    while(usbMIDI.read()){}
}

void commandAbout() {
//
    // clear the screen and draw title bar showing with the "Back" button
    //
    ui.drawTitleBarWithBackButton("About This Program");
    ui.clearDisplaySpace();

    //
    // show some info in the display space
    //
    int y = 70;
    int ySpacing = 17;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("Test");

    y += ySpacing * 2;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("This is a Test");


    //
    // wait for the user to press the "Back" button, then return to the main menu
    //
    while(true)
    {
        ui.getTouchEvents();

        if (ui.checkForBackButtonClicked())
            return;
    }
}
