/*
    x_BIMU_Arduino_Example.ino
    Author: Seb Madgwick

    Example usage of x-BIMU C++ library.  Also uses the quaternion library to
    convert the received quaternion to Euler angles.

    Requires two hardware serial modules: one to receive from the x-BIMU and one
    to transmit the decoded data to be displayed on computer.

    Hardware connections:
    x-BIMU Serial Breakout GND  -> Arduino MEGA GND
    x-BIMU Serial Breakout PWR  -> Arduino MEGA 5V
    x-BIMU Serial Breakout TX-O -> Arduino MEGA RX1

    Tested with "arduino-1.0.3" and "Arduino MEGA".
*/

//------------------------------------------------------------------------------
// Includes

#include "Quaternion.h"
#include "XbimuReceiver.h"

//------------------------------------------------------------------------------
// Variables

XbimuReceiver xbimuReceiver;

//------------------------------------------------------------------------------
// Functions

void setup() {
    Serial.begin(115200);   // for sending data to computer
    Serial1.begin(115200);  // for receiving data from x-BIMU
}

void loop() {

    // Process received data
    while(Serial1.available() > 0) {
        xbimuReceiver.processNewChar(Serial1.read());
    }

    // Print quaternion data as Euler angles
    if(xbimuReceiver.isQuaternionGetReady()) {
        QuaternionStruct quaternionStruct = xbimuReceiver.getQuaternion();
        Quaternion quaternion = Quaternion(quaternionStruct.w * 0.0001f, quaternionStruct.x * 0.0001f, quaternionStruct.y * 0.0001f, quaternionStruct.z * 0.0001f);
        EulerAnglesStruct eulerAnglesStruct = quaternion.getEulerAngles();
        Serial.print("roll = ");
        Serial.print(eulerAnglesStruct.roll);
        Serial.print(", pitch = ");
        Serial.print(eulerAnglesStruct.pitch);
        Serial.print(", yaw = ");
        Serial.print(eulerAnglesStruct.yaw);
        Serial.print("\r");
    }

    // Print sensor data
    if(xbimuReceiver.isSensorGetReady()) {
        SensorStruct sensorStruct = xbimuReceiver.getSensor();
        Serial.print("gyrX = ");
        Serial.print(sensorStruct.gyrX);
        Serial.print(", gyrY = ");
        Serial.print(sensorStruct.gyrY);
        Serial.print(", gyrZ = ");
        Serial.print(sensorStruct.gyrZ);
        Serial.print(", accX = ");
        Serial.print(sensorStruct.accX);
        Serial.print(", accY = ");
        Serial.print(sensorStruct.accY);
        Serial.print(", accZ = ");
        Serial.print(sensorStruct.accZ);
        Serial.print(", magX = ");
        Serial.print(sensorStruct.magX);
        Serial.print(", magY = ");
        Serial.print(sensorStruct.magY);
        Serial.print(", magZ = ");
        Serial.print(sensorStruct.magZ);
        Serial.print("\r");
    }

    // Print battery data
    if(xbimuReceiver.isBatteryGetReady()) {
        BatteryStruct batteryStruct = xbimuReceiver.getBattery();
        Serial.print("battery = ");
        Serial.print(batteryStruct.voltage);
        Serial.print("\r");
    }
}

//------------------------------------------------------------------------------
// End of file
