//2 button midi CC control

#include <Arduino.h>
#include <RPLidar.h>

const byte LIDAR_MOTOR_PIN = 2;

// Global Objects
RPLidar mLidar;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(LIDAR_MOTOR_PIN, OUTPUT);
    mLidar.begin(Serial1);
}

void loop() {
    if (IS_OK(mLidar.waitPoint())) {
        float distance = mLidar.getCurrentPoint().distance; //distance value in mm unit
        float angle    = mLidar.getCurrentPoint().angle; //anglue value in degree
        bool  startBit = mLidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
        byte  quality  = mLidar.getCurrentPoint().quality; //quality of the current measurement

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