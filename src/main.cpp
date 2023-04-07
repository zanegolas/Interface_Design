//2 button midi CC control

#include <Arduino.h>
#include <usb_midi.h>
#include <LiquidCrystal_I2C.h>
#include <rplidar_driver_impl.h>

const byte LIDAR_MOTOR_PIN = 25; // Lidar TX/RX on 26/27
char report[80];

// Global Objects
LiquidCrystal_I2C mLCD(0x27,16,2);
RPLidar mLidar;

void setup() {
    mLCD.init();
    mLCD.noCursor();
    mLCD.backlight();
    mLCD.print("Starting Up...");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(LIDAR_MOTOR_PIN, OUTPUT);
    mLidar.begin();

    mLCD.setCursor(0, 1);
    mLCD.print("Complete!");
}

void printInfo(){
    rplidar_response_device_info_t info;
    mLidar.getDeviceInfo(info);

    snprintf(report, sizeof(report), "model: %d firmware: %d.%d", info.model, info.firmware_version/256, info.firmware_version%256);
    mLCD.print(report);
    delay(1000);
}

void loop() {

    if (!mLidar.isScanning()){
        mLidar.startScanNormal(false);
        digitalWrite(LIDAR_MOTOR_PIN, HIGH); // turn on the motor
        mLCD.clear();
        mLCD.print("Start Scan...");
        delay(1000);
    } else {
        mLCD.clear();
        mLCD.print("Reading...");
        delay(1000);
        mLidar.loopScanData();

        rplidar_response_measurement_node_hq_t nodes[512];
        size_t nodeCount = 512; // variable will be set to number of received measurement by reference
        u_result ans = mLidar.grabScanData(nodes, nodeCount);

        if (IS_OK(ans)) {
            for (size_t i = 0; i < nodeCount; ++i) {
                // convert to standard units
                float angle_in_degrees = static_cast<float>(nodes[i].angle_z_q14) * 90.f / (1 << 14);
                float distance_in_meters = static_cast<float>(nodes[i].dist_mm_q2) / 1000.f / (1 << 2);
                mLCD.setCursor(0, 0);
                mLCD.print(angle_in_degrees);
                mLCD.setCursor(0, 1);
                mLCD.print(distance_in_meters);
            }
        }
    }
    if (!mLidar.isConnected()) {
        mLCD.clear();
        mLCD.print("LIDAR Error");
    } else {
        mLCD.clear();
        printInfo();
    }
    // This prevents crashes that happen when incoming usbMIDI is ignored.
    while(usbMIDI.read()){}

}