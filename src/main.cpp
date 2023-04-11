

#include <Arduino.h>
#include <rplidar_driver_impl.h>
#include "ZGObjectTracker.h"
#include <vector>
#include <TeensyUserInterface.h>
#include <font_Arial.h>
#include <font_ArialBold.h>

TeensyUserInterface ui;

const byte LIDAR_MOTOR_PIN = 3;



// Global Objects
RPLidar mLidar;
elapsedMillis mTimer = 0;
elapsedMillis mProcessWait = 0;
elapsedMillis mLatency = 0;
int mSampleCount = 0;
char reportTotal[80];
char reportReady[80];
char reportLatency[80];
bool mReadyToProcess = false;
ZGObjectTracker mObjectTracker;
std::vector<ZGPolarData> mPointBuffer;
bool shouldUpdateDisplay = false;

const uint16_t colorArray [] {
    LCD_BLUE,
    LCD_GREEN,
    LCD_YELLOW,
    LCD_PURPLE,
    LCD_CYAN,
    LCD_ORANGE
};


void setup() {
    //
    // pin numbers used in addition to the default SPI pins
    //
    const int LCD_CS_PIN = 10;
    const int LCD_DC_PIN = 9;
    const int TOUCH_CS_PIN = 8;

    //
    // setup the LCD orientation, the default font and initialize the user interface
    //
    ui.begin(LCD_CS_PIN, LCD_DC_PIN, TOUCH_CS_PIN, LCD_ORIENTATION_LANDSCAPE_4PIN_RIGHT, Arial_9_Bold);

    pinMode(LIDAR_MOTOR_PIN, OUTPUT);
    mLidar.begin();

    delay(1000);
    mLidar.startScanExpress(true, RPLIDAR_CONF_SCAN_COMMAND_EXPRESS);
    digitalWrite(LIDAR_MOTOR_PIN, HIGH); // turn on the motor
    delay(10);

    usbMIDI.begin();

//    Serial.begin(9600);
//    Serial.println("Setup Complete");
    ui.lcdPrint("Setup Complete");
}

void playMidi() {
    for (auto& object : mObjectTracker.getObjects()) {
        if (object.newMidiNote != object.currentMidiNote) {
            usbMIDI.sendNoteOff(object.currentMidiNote, 127, object.midiChannel);
            object.currentMidiNote = object.newMidiNote;
            usbMIDI.sendNoteOn(object.currentMidiNote, 127, object.midiChannel);
        }
        usbMIDI.sendControlChange(1, object.modValue, object.midiChannel);
    }
}

void updateDisplay(){
    ui.lcdClearScreen(LCD_BLACK);
    ui.lcdSetCursorXY(0, 0);
    ui.lcdPrint(reportTotal);
    ui.lcdSetCursorXY(0, 12);
    ui.lcdPrint(reportReady);
    ui.lcdSetCursorXY(0, 24);
    ui.lcdPrint(reportLatency);
    auto clusters = mObjectTracker.getClusters();
    char report[80];
    snprintf(report, sizeof(report), "Found %d clusters in specified range", clusters.size());
    ui.lcdSetCursorXY(0, 36);
    ui.lcdPrint(report);
    snprintf(report, sizeof(report), "Tracking %d Objects", mObjectTracker.getObjects().size());
    ui.lcdSetCursorXY(0, 48);
    ui.lcdPrint(report);
    int index = 1;
    for(const auto& cluster : clusters) {
        snprintf(report, sizeof(report), "Cluster %d Contains %d Points ", index, cluster.size());
        ui.lcdSetCursorXY(0, 48 + 12 * index);
        ui.lcdPrint(report);
        index++;
    }
    shouldUpdateDisplay = false;
}

void plotObjects(){
    auto width = 320;
    auto height = 240;
    auto center_x = width / 2;
    auto center_y = height / 2;
    ui.lcdClearScreen(LCD_BLACK);
    ui.lcdDrawFilledCircle(center_x, center_y, 2, LCD_RED);
    auto clusters = mObjectTracker.getClusters();
    auto index = 0;
    for (auto cluster : clusters) {
        auto object_color = colorArray[index];
        for (auto point : cluster) {
            ui.lcdDrawFilledCircle(center_x + point.x, center_y + point.y, 1, object_color);
        }
        index++;
        if (index > 5) {
            index = 0;
        }
    }
    auto objects = mObjectTracker.getObjects();
    for (auto object :objects){
        ui.lcdDrawFilledCircle(center_x + object.x, center_y + object.y, 4, LCD_RED);
    }
    shouldUpdateDisplay = false;
}

void loop() {

    // loop needs to be send called every loop
    mLidar.loopScanExpressData();

    // create object to hold received data for processing
    rplidar_response_measurement_node_hq_t nodes[512];
    size_t nodeCount = 512; // variable will be set to number of received measurement by reference
    u_result ans = mLidar.grabScanExpressData(nodes, nodeCount);
    if (IS_OK(ans)){
        for (size_t i = 0; i < nodeCount; ++i){

            if (nodes[i].quality == 0) {
                continue;
            } else {
                ZGPolarData p;
                p.distance = nodes[i].dist_mm_q2 / 10.f / (1<<2); //cm
                p.angle = nodes[i].angle_z_q14 * 90.f / (1<<14); //degrees
                mPointBuffer.push_back(p);
                mSampleCount++;
            }

            if (nodes[i].flag == 1) {
                mReadyToProcess = true;
            }
//            snprintf(report, sizeof(report), "Degrees: %.2f Distance: %.2f Quality: %d Flag: %d", angle_in_degrees, distance_in_meters, nodes[i].quality, nodes[i].flag);
//            Serial.println(report);
//            mSampleCount++;
        }
    }

    if (mReadyToProcess) {
        mProcessWait = 0;
        snprintf(reportReady, sizeof(reportReady), "Processing buffer of %d samples", mPointBuffer.size());
//        Serial.println(reportReady);
        mObjectTracker.processBuffer(mPointBuffer);
        snprintf(reportLatency, sizeof(reportLatency), "Processing took %d ms with total latency of %d ms", static_cast<int>(mProcessWait), static_cast<int>(mLatency));
//        Serial.println(reportLatency);
        mLatency = 0;
        mReadyToProcess = false;
    }

    if (mTimer >= 1000) {
        snprintf(reportTotal, sizeof(reportTotal), "Processed %d samples in %d ms", mSampleCount, static_cast<int>(mTimer));
//        Serial.println(" ");
//        Serial.println(reportTotal);
//        Serial.println(" ");
        shouldUpdateDisplay = true;
        mSampleCount = 0;
        mTimer = 0;
    }

    if (shouldUpdateDisplay) {
//        updateDisplay();
        plotObjects();
    }

    playMidi();

    while(usbMIDI.read()){}
}
