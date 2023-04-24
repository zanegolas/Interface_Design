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

    mDisplay->initialize(ui);
    mLidar->initialize();
    usbMIDI.begin();
}

//
// for each menu, create a forward declaration with "extern"
//
extern MENU_ITEM mainMenu[];

void commandAbout();
void commandSetRange();


//
// main menu, NOTE HOW 4TH COLUMN IN FIRST LINE IS SET TO "NULL", THIS DISPLAYS THE "MENU" BUTTON
//
MENU_ITEM mainMenu[] = {
        {MENU_ITEM_TYPE_MAIN_MENU_HEADER,  "Settings",         MENU_COLUMNS_1,            NULL},
        {MENU_ITEM_TYPE_COMMAND,           "Set Range",                      commandSetRange,              NULL},
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
    ui.drawTitleBarWithBackButton("About");
    ui.clearDisplaySpace();

    //
    // show some info in the display space
    //
    int y = 70;
    int ySpacing = 20;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("MPE LiDAR MIDI Interface");

    y += ySpacing * 2;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("This project was developed for the");

    y += ySpacing * 1;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("interface design class at CalArts in");

    y += ySpacing * 1;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("Spring 2023 by Zane Golas.");

    y += ySpacing * 2;
    ui.lcdSetCursorXY(ui.displaySpaceCenterX, y);
    ui.lcdPrintCentered("Version 1.0");


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

//
// menu command to "Set Range"
//
void commandSetRange()
{
    char sBuffer[25];

    ui.lcdClearScreen(LCD_BLACK);
    //
    // draw the title bar and clear the screen
    //
    ui.drawTitleBar("Set Max Detection Distance In CM");
    ui.clearDisplaySpace();

    //
    // set the size and initial value of the number box
    //
    const int numberBoxWidth = 200;
    const int numberBoxAndButtonsHeight = 35;

    //
    // define a Number Box so the user can select a numeric value, specify the initial value,
    // max and min values, and step up/down amount
    //
    NUMBER_BOX my_NumberBox;
    my_NumberBox.labelText     = "Range";
    my_NumberBox.value         = static_cast<int>(mObjectTracker->getMaxDistance());
    my_NumberBox.minimumValue  = 50;
    my_NumberBox.maximumValue  = 1000;
    my_NumberBox.stepAmount    = 50;
    my_NumberBox.centerX       = ui.displaySpaceCenterX;
    my_NumberBox.centerY       = ui.displaySpaceCenterY - 20;
    my_NumberBox.width         = numberBoxWidth;
    my_NumberBox.height        = numberBoxAndButtonsHeight;
    ui.drawNumberBox(my_NumberBox);


    //
    // define and display "OK" and "Cancel" buttons
    //
    BUTTON okButton        = {"OK",      ui.displaySpaceCenterX-70, ui.displaySpaceBottomY-35,  120 , numberBoxAndButtonsHeight};
    ui.drawButton(okButton);

    BUTTON cancelButton    = {"Cancel",  ui.displaySpaceCenterX+70, ui.displaySpaceBottomY-35,  120 , numberBoxAndButtonsHeight};
    ui.drawButton(cancelButton);


    //
    // process touch events
    //
    while(true)
    {
        ui.getTouchEvents();

        //
        // process touch events on the Number Box
        //
        ui.checkForNumberBoxTouched(my_NumberBox);

        //
        // check for touch events on the "OK" button
        //
        if (ui.checkForButtonClicked(okButton))
        {
            //
            // user OK pressed, get the value from the Number Box and display it
            //
            mObjectTracker->setMaxDistance(static_cast<float>(my_NumberBox.value));
            sprintf(sBuffer, "Max Range = %d cm", (int)mObjectTracker->getMaxDistance());

            ui.clearDisplaySpace();
            ui.lcdSetCursorXY(ui.displaySpaceCenterX, ui.displaySpaceCenterY-10);
            ui.lcdPrintCentered(sBuffer);
            delay(1500);
            return;
        }

        //
        // check for touch events on the "Cancel" button
        //
        if (ui.checkForButtonClicked(cancelButton))
            return;
    }
}
