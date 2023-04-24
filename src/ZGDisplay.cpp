//
// ZGDisplay.cpp
// Teensy 4.1
//
// Created by Zane Golas on 4/11/23.
// Copyright (c) 2023 Zane Golas. All rights reserved.
//


#include "ZGDisplay.h"
#include "assets/LaunchScreen.c"

ZGDisplay::ZGDisplay(ZGObjectTracker *inObjectTracker, ZGLidar *inLidar)
{
    mObjectTracker = inObjectTracker;
    mLidar = inLidar;
}

ZGDisplay::~ZGDisplay() = default;

void ZGDisplay::initialize()
{
    mUI.begin(LCD_CS_PIN, LCD_DC_PIN, TOUCH_CS_PIN, LCD_ORIENTATION_LANDSCAPE_4PIN_LEFT, Inter_12);
    mUI.lcdDrawImage(0, 0, width, height, LaunchScreen);
    mUI.setTitleBarColors(aerospace_orange, ghost_white, LCD_BLACK, aerospace_orange);
    mUI.setMenuColors(LCD_BLACK, aerospace_orange, LCD_BLACK, dim_gray, ghost_white);
}

void ZGDisplay::refresh()
{
    mUI.getTouchEvents();
    if(mUI.checkForButtonClicked(mMenuButton)){
        mLidar->pause();
        showMainMenu(mUI);
        mLidar->resume();
        mRedraw = true;
    }
    if (mRefreshTimer >= 100) {
        if (showDebugData) {
            printDebugData(mUI, mRedraw);
        } else {
            plotObjects(mUI, mRedraw);
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
        mUI.lcdDrawImage(0, 0, width, height, SettingsBackplate);
        mUI.lcdSetCursorXY(247, 6);
        mUI.lcdSetFont(ChakraPetchSemiBold_16);
        mUI.lcdPrintCentered("DEBUG MODE");
        mUI.lcdSetFont(Inter_12);
        inUI.drawButton(mMenuButton);
        auto index = 0;
        for (const auto& category : debugCategories) {
            inUI.lcdSetCursorXY(0, 40 + 18 * index);
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
    auto cursor_y = inLine * 18 + 40;
    auto string_cast = static_cast<String>(inValue);
    auto print_val = string_cast.c_str();
    auto str_width = inUI.lcdStringWidthInPixels(print_val);
    auto str_height = 18;

    inUI.lcdSetCursorXY(cursor_x, cursor_y);
    inUI.lcdDrawFilledRectangle(cursor_x, cursor_y, str_width + 20, str_height, LCD_BLACK);
    inUI.lcdPrint(print_val);
}

void ZGDisplay::plotObjects(TeensyUserInterface& inUI, bool inRedrawAll)
{
    auto scale = _getScaleFactor();

    if (inRedrawAll) {
        inUI.lcdClearScreen(LCD_BLACK);
        inUI.drawButton(mMenuButton);

        inUI.lcdSetFont(Inter_9);
        inUI.lcdSetFontColor(cadet_gray);

        inUI.lcdSetCursorXY(71, 7);
        inUI.lcdPrintRightJustified("Scale");

        inUI.lcdSetCursorXY(249, 7);
        inUI.lcdPrint("Max Range");

        inUI.lcdSetCursorXY(67, 199);
        inUI.lcdPrintRightJustified("Mode");

        inUI.lcdSetFont(Inter_11);
        inUI.lcdSetFontColor(ghost_white);

        inUI.lcdSetCursorXY(71, 23);
        inUI.lcdPrintRightJustified("Chromatic");

        inUI.lcdSetCursorXY(249, 23);
        inUI.lcdPrint((int)mObjectTracker->getMaxDistance());
        inUI.lcdPrint("cm");

        inUI.lcdSetCursorXY(67, 215);
        inUI.lcdPrintRightJustified("DBSCAN");

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
    inUI.lcdDrawCircle(center_x, center_y, center_y - 1, dim_gray);

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

void ZGDisplay::showMainMenu(TeensyUserInterface& ui) {
    auto redraw = true;
    BUTTON_IMAGE mScanButton {"SCAN", SelectButtonDefault, SelectButtonPressed, 82, 80, 144 , 50, ghost_white, ChakraPetchSemiBold_12};
    BUTTON_IMAGE mMidiButton {"MIDI",SelectButtonDefault, SelectButtonPressed, 237, 80, 144 , 50, ghost_white, ChakraPetchSemiBold_12};
    BUTTON_IMAGE mDisplayButton {"DISPLAY",SelectButtonDefault, SelectButtonPressed, 82, 162, 144 , 50, ghost_white, ChakraPetchSemiBold_12};
    BUTTON_IMAGE mAboutButton {"ABOUT",SelectButtonDefault, SelectButtonPressed, 237, 162, 144 , 50, ghost_white, ChakraPetchSemiBold_12};

    while(true)
    {
        if (redraw){
            ui.lcdDrawImage(0, 0, width, height, SettingsBackplate);
            ui.lcdSetCursorXY(247, 6);
            ui.lcdSetFont(ChakraPetchSemiBold_16);
            ui.lcdPrintCentered("SETTINGS");
            ui.drawButton(mBackButton);
            ui.drawButton(mScanButton);
            ui.drawButton(mMidiButton);
            ui.drawButton(mDisplayButton);
            ui.drawButton(mAboutButton);
            redraw = false;
        }
        ui.getTouchEvents();

        if (ui.checkForButtonClicked(mBackButton)){
            return;
        }

        if (ui.checkForButtonClicked(mScanButton)) {
            showScan();
            redraw = true;
        }

        if (ui.checkForButtonClicked(mMidiButton)) {
            showMidi();
            redraw = true;
        }

        if (ui.checkForButtonClicked(mDisplayButton)) {
            showDisplay();
            redraw = true;
        }

        if (ui.checkForButtonClicked(mAboutButton)) {
            showAbout(ui);
            redraw = true;
        }
    }
}

void ZGDisplay::showAbout(TeensyUserInterface &ui) {
    ui.lcdDrawImage(0, 0, width, height, SettingsBackplate);
    ui.lcdSetCursorXY(247, 6);
    ui.lcdSetFont(ChakraPetchSemiBold_16);
    ui.lcdPrintCentered("ABOUT");
    ui.drawButton(mBackButton);

    ui.lcdSetFont(Inter_12);

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

        if (ui.checkForButtonClicked(mBackButton))
            return;
    }

}

void ZGDisplay::showScan()
{
    mUI.lcdDrawImage(0, 0, width, height, SettingsBackplate);
    mUI.lcdSetCursorXY(247, 6);
    mUI.lcdSetFont(ChakraPetchSemiBold_16);
    mUI.lcdPrintCentered("SCAN");

    //
    // set the size and initial value of the number box
    //
    const int numberBoxWidth = 200;
    const int numberBoxAndButtonsHeight = 30;

    //
    // define a Number Box so the user can select a numeric value, specify the initial value,
    // max and min values, and step up/down amount
    //
    NUMBER_BOX range_box;
    range_box.labelText     = "Range In CM";
    range_box.value         = static_cast<int>(mObjectTracker->getMaxDistance());
    range_box.minimumValue  = 50;
    range_box.maximumValue  = 1000;
    range_box.stepAmount    = 50;
    range_box.centerX       = width / 2;
    range_box.centerY       = height / 2 - 40;
    range_box.width         = numberBoxWidth;
    range_box.height        = numberBoxAndButtonsHeight;
    mUI.drawNumberBox(range_box);

    SELECTION_BOX mode_box;
    mode_box.labelText = "Clustering Algorithm Mode";
    mode_box.value = 1;	 // set default value, 0 is 1st choice
    mode_box.choice0Text = "Distance";
    mode_box.choice1Text = "DBSCAN";
    mode_box.choice2Text = "";
    mode_box.choice3Text = "";		// set unused choices to: ""
    mode_box.centerX = width/2;
    mode_box.centerY = height / 2 + 30;
    mode_box.width = 250;
    mode_box.height = 30;
    mUI.drawSelectionBox(mode_box);		       // display the Selection Box


    mUI.drawButton(mOkButton);

    mUI.drawButton(mCancelButton);


    //
    // process touch events
    //
    while(true)
    {
        mUI.getTouchEvents();

        mUI.checkForNumberBoxTouched(range_box);
        mUI.checkForSelectionBoxTouched(mode_box);

        //
        // check for touch events on the "OK" button
        //
        if (mUI.checkForButtonClicked(mOkButton))
        {
            //
            // user OK pressed, get the value from the Number Box and display it
            //
            mObjectTracker->setMaxDistance(static_cast<float>(range_box.value));
            return;
        }

        //
        // check for touch events on the "Cancel" button
        //
        if (mUI.checkForButtonClicked(mCancelButton))
            return;
    }
}

void ZGDisplay::showDisplay()
{
    mUI.lcdDrawImage(0, 0, width, height, SettingsBackplate);
    mUI.lcdSetCursorXY(247, 6);
    mUI.lcdSetFont(ChakraPetchSemiBold_16);
    mUI.lcdPrintCentered("DISPLAY");

    SELECTION_BOX mode_box;
    mode_box.labelText = "Main View";
    mode_box.value = showDebugData;	 // set default value, 0 is 1st choice
    mode_box.choice0Text = "Plot";
    mode_box.choice1Text = "Debug";
    mode_box.choice2Text = "";
    mode_box.choice3Text = "";		// set unused choices to: ""
    mode_box.centerX = width/2;
    mode_box.centerY = height / 2;
    mode_box.width = 250;
    mode_box.height = 30;
    mUI.drawSelectionBox(mode_box);		       // display the Selection Box


    mUI.drawButton(mOkButton);

    mUI.drawButton(mCancelButton);


    //
    // process touch events
    //
    while(true)
    {
        mUI.getTouchEvents();

        mUI.checkForSelectionBoxTouched(mode_box);

        //
        // check for touch events on the "OK" button
        //
        if (mUI.checkForButtonClicked(mOkButton))
        {
            //
            // user OK pressed, get the value from the Number Box and display it
            //
            showDebugData = mode_box.value;
            return;
        }

        //
        // check for touch events on the "Cancel" button
        //
        if (mUI.checkForButtonClicked(mCancelButton))
            return;
    }
}

void ZGDisplay::showMidi()
{
    mUI.lcdDrawImage(0, 0, width, height, SettingsBackplate);
    mUI.lcdSetCursorXY(247, 6);
    mUI.lcdSetFont(ChakraPetchSemiBold_16);
    mUI.lcdPrintCentered("MIDI");

    //
    // set the size and initial value of the number box
    //
    const int numberBoxWidth = 200;
    const int numberBoxAndButtonsHeight = 30;

    //
    // define a Number Box so the user can select a numeric value, specify the initial value,
    // max and min values, and step up/down amount
    //
    NUMBER_BOX note_box;
    note_box.labelText     = "Root Note";
    note_box.value         = 0;
    note_box.minimumValue  = 0;
    note_box.maximumValue  = 11;
    note_box.stepAmount    = 1;
    note_box.centerX       = width / 2;
    note_box.centerY       = height / 2 - 40;
    note_box.width         = numberBoxWidth;
    note_box.height        = numberBoxAndButtonsHeight;
    note_box.isNoteBox     = true;
    mUI.drawNumberBox(note_box);

    NUMBER_BOX scale_box;
    scale_box.labelText     = "Scale Type";
    scale_box.value         = 0;
    scale_box.minimumValue  = 0;
    scale_box.maximumValue  = 2;
    scale_box.stepAmount    = 1;
    scale_box.centerX       = width / 2;
    scale_box.centerY       = height / 2 + 30;
    scale_box.width         = numberBoxWidth;
    scale_box.height        = numberBoxAndButtonsHeight;
    scale_box.isScaleBox     = true;
    mUI.drawNumberBox(scale_box);


    mUI.drawButton(mOkButton);

    mUI.drawButton(mCancelButton);


    //
    // process touch events
    //
    while(true)
    {
        mUI.getTouchEvents();

        mUI.checkForNumberBoxTouched(note_box);
        mUI.checkForNumberBoxTouched(scale_box);

        //
        // check for touch events on the "OK" button
        //
        if (mUI.checkForButtonClicked(mOkButton))
        {
            //
            // user OK pressed, get the value from the Number Box and display it
            //
            return;
        }

        //
        // check for touch events on the "Cancel" button
        //
        if (mUI.checkForButtonClicked(mCancelButton))
            return;
    }
}


