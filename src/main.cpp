//2 button midi CC control

#include <Arduino.h>
#include <rplidar_driver_impl.h>
#include "ZGObjectTracker.h"

const byte LIDAR_MOTOR_PIN = 2;

// Global Objects
RPLidar mLidar;
elapsedMillis mTimer = 0;
int mSampleCount = 0;
char report[80];
bool mReadyToProcess = false;
ZGObjectTracker mObjectTracker;
ZGPointData mPointBuffer[1024];
int mBufferSize = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(LIDAR_MOTOR_PIN, OUTPUT);
    mLidar.begin();

    delay(1000);
    mLidar.startScanExpress(true, RPLIDAR_CONF_SCAN_COMMAND_EXPRESS);
    digitalWrite(LIDAR_MOTOR_PIN, HIGH); // turn on the motor
    delay(10);

    Serial.begin(9600);
    Serial.println("Setup Complete");
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
            float distance_in_meters = nodes[i].dist_mm_q2 / 1000.f / (1<<2);
            if (nodes[i].quality == 0 || distance_in_meters == 0.f) {
                continue;
            } else {
                mPointBuffer[i].angle = nodes[i].angle_z_q14 * 90.f / (1<<14);
                if (distance_in_meters >= 1.5f) {
                    distance_in_meters = 0.f;
                }
                mPointBuffer[i].distance = distance_in_meters;
                mBufferSize++;
                mSampleCount++;
            }

            if (nodes[i].flag == 1)
            {
                mReadyToProcess = true;
                snprintf(report, sizeof(report), "Ready to process with: %d samples", mBufferSize);
                Serial.println(report);
            }

            if (mBufferSize >= 1024) {
                Serial.println("ERROR: BUFFER OVERFLOW");
                mBufferSize = 0;
            }
//            snprintf(report, sizeof(report), "Degrees: %.2f Distance: %.2f Quality: %d Flag: %d", angle_in_degrees, distance_in_meters, nodes[i].quality, nodes[i].flag);
//            Serial.println(report);
//            mSampleCount++;
        }
    }

    if (mReadyToProcess) {
        mObjectTracker.processBuffer(mPointBuffer, mBufferSize);
        mReadyToProcess = false;
    }
    if (mTimer >= 1000) {
        snprintf(report, sizeof(report), "Processed %d samples in %d ms", mSampleCount, static_cast<int>(mTimer));
        Serial.println(report);

        mSampleCount = 0;
        mTimer = 0;
    }
}
