//2 button midi CC control

#include <Arduino.h>
#include <RPLidar.h>

const byte LIDAR_MOTOR_PIN = 2;

// Global Objects
RPLidar mLidar;
elapsedMillis mLatency = 0;
int mSampleCount = 0;
int mQualitySamples = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(LIDAR_MOTOR_PIN, OUTPUT);
    mLidar.begin(Serial1);

    Serial.begin(9600);
    Serial.println("Setup Complete");
}

void loop() {
    if (IS_OK(mLidar.waitPoint())) {
        float distance = mLidar.getCurrentPoint().distance; //distance value in mm unit
        float angle    = mLidar.getCurrentPoint().angle; //angle value in degree
        bool  startBit = mLidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
        byte  quality  = mLidar.getCurrentPoint().quality; //quality of the current measurement

        mSampleCount++;

        if (quality != 0) {
            mQualitySamples++;
        }
        /* Diagnostic Prints
        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.print("Angle: ");
        Serial.println(angle);
        Serial.print("Start Bit: ");
        Serial.println(startBit);
        Serial.print("Quality: ");
        Serial.println(quality); */

        if (startBit) {
            Serial.print("Start Bit Received After: ");
            Serial.print(mLatency);
            Serial.println("ms");
            Serial.print("Number Of Samples: ");
            Serial.println(mSampleCount);
            Serial.print("Quality Samples: ");
            Serial.println(mQualitySamples);
            auto samples_per_second = mSampleCount * (1000 / mLatency);
            Serial.print("Samples Per Second: ");
            Serial.println(samples_per_second);

            mSampleCount = 0;
            mQualitySamples = 0;
            mLatency = 0;

        }

        digitalWrite(LED_BUILTIN, HIGH);


    } else {
        analogWrite(LIDAR_MOTOR_PIN, 0); //stop the rplidar motor

        // try to detect RPLIDAR...
        rplidar_response_device_info_t info;
        if (IS_OK(mLidar.getDeviceInfo(info, 100))) {
            // detected...
            mLidar.startScan();

            // start motor rotating at max allowed speed
            analogWrite(LIDAR_MOTOR_PIN, 255);
            delay(1000);
        }
        digitalWrite(LED_BUILTIN, LOW);
    }

}