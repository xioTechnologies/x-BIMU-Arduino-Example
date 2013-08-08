/*
    XbimuReceiver.cpp
    Author: Seb Madgwick

    C++ library for receiving data from the x-BIMU.  Only supports binary
    packet mode, 'binary packet mode' must be enabled in x-BIMU settings
    (BP = 1).

    See x_BIMU_Arduino_Example.ino for example usage.
*/

//------------------------------------------------------------------------------
// Includes

#include "XbimuReceiver.h"

//------------------------------------------------------------------------------
// Definitions

enum {
    QUAT_LENGTH = 11,
    SENS_LENGTH = 21,
    BATT_LENGTH = 5,
    MAX_LENGTH = 21 /* maximum packet length of all packet types */
};

//------------------------------------------------------------------------------
// Variables

// Serial stream decoding
unsigned char buf[256];
unsigned char bufIndex = 0;
unsigned char bufCount = 0;
bool inSync = false;

// Decoded data
QuaternionStruct quaternionStruct = { 0, 0, 0, 0, 0 };
SensorStruct sensorStruct = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BatteryStruct batteryStruct = { 0, 0 };

// Data ready flags
bool quaternionGetReady = false;
bool sensorGetReady = false;
bool batteryGetReady = false;

//------------------------------------------------------------------------------
// Methods

XbimuReceiver::XbimuReceiver() {
}

void XbimuReceiver::processNewChar(unsigned char c) {

    // Add new byte to buffer
    buf[bufIndex++] = c;

    // Check if out of sync
    bufCount++;
    if (bufCount > MAX_LENGTH) {
        bufCount = MAX_LENGTH; // prevent overflow
        inSync = false;
    }

    // Decode quaternion packet
    if (bufIndex >= QUAT_LENGTH) {
        if ((inSync ? (char)buf[0] : (char)buf[bufIndex - QUAT_LENGTH]) == 'Q') {
            if (calcChecksum(QUAT_LENGTH) == 0) {
                quaternionStruct.w = (int)(((int)buf[bufIndex - 10] << 8) | (int)buf[bufIndex - 9]);
                quaternionStruct.x = (int)(((int)buf[bufIndex - 8] << 8) | (int)buf[bufIndex - 7]);
                quaternionStruct.y = (int)(((int)buf[bufIndex - 6] << 8) | (int)buf[bufIndex - 5]);
                quaternionStruct.z = (int)(((int)buf[bufIndex - 4] << 8) | (int)buf[bufIndex - 3]);
                quaternionStruct.counter = buf[bufIndex - 2];
                quaternionGetReady = true;
                bufIndex = 0;
                bufCount = 0;
                inSync = true;
            }
        }
    }

    // Decode sensor packet
    if (bufIndex >= SENS_LENGTH) {
        if ((inSync ? (char)buf[0] : (char)buf[bufIndex - SENS_LENGTH]) == 'S') {
            if (calcChecksum(SENS_LENGTH) == 0) {
                sensorStruct.gyrX = (int)(((int)buf[bufIndex - 20] << 8) | (int)buf[bufIndex - 19]);
                sensorStruct.gyrY = (int)(((int)buf[bufIndex - 18] << 8) | (int)buf[bufIndex - 17]);
                sensorStruct.gyrZ = (int)(((int)buf[bufIndex - 16] << 8) | (int)buf[bufIndex - 15]);
                sensorStruct.accX = (int)(((int)buf[bufIndex - 14] << 8) | (int)buf[bufIndex - 13]);
                sensorStruct.accY = (int)(((int)buf[bufIndex - 12] << 8) | (int)buf[bufIndex - 11]);
                sensorStruct.accZ = (int)(((int)buf[bufIndex - 10] << 8) | (int)buf[bufIndex - 9]);
                sensorStruct.magX = (int)(((int)buf[bufIndex - 8] << 8) | (int)buf[bufIndex - 7]);
                sensorStruct.magY = (int)(((int)buf[bufIndex - 6] << 8) | (int)buf[bufIndex - 5]);
                sensorStruct.magZ = (int)(((int)buf[bufIndex - 4] << 8) | (int)buf[bufIndex - 3]);
                sensorStruct.counter = buf[bufIndex - 2];
                sensorGetReady = true;
                bufIndex = 0;
                bufCount = 0;
                inSync = true;
            }
        }
    }

    // Decode battery packet
    if (bufIndex >= BATT_LENGTH) {
        if ((inSync ? (char)buf[0] : (char)buf[bufIndex - BATT_LENGTH]) == 'B') {
            if (calcChecksum(BATT_LENGTH) == 0) {
                batteryStruct.voltage = (int)(((int)buf[bufIndex - 4] << 8) | (int)buf[bufIndex - 3]);
                batteryStruct.counter = buf[bufIndex - 2];
                batteryGetReady = true;
                bufIndex = 0;
                bufCount = 0;
                inSync = true;
            }
        }
    }
}

unsigned char XbimuReceiver::calcChecksum(unsigned char packetLength) const {
    unsigned char tempRxBufIndex = (unsigned char)(bufIndex - packetLength);
    unsigned char checksum = 0;
    while (tempRxBufIndex != bufIndex) {
        checksum ^= buf[tempRxBufIndex++];
    }
    return checksum;
}

bool XbimuReceiver::isQuaternionGetReady(void) const {
    return quaternionGetReady;
}

bool XbimuReceiver::isSensorGetReady(void) const {
    return sensorGetReady;
}

bool XbimuReceiver::isBatteryGetReady(void) const {
    return batteryGetReady;
}

QuaternionStruct XbimuReceiver::getQuaternion(void) {
    quaternionGetReady = false;
    return quaternionStruct;
}

SensorStruct XbimuReceiver::getSensor(void) {
    sensorGetReady = false;
    return sensorStruct;
}

BatteryStruct XbimuReceiver::getBattery(void) {
    batteryGetReady = false;
    return batteryStruct;
}

//------------------------------------------------------------------------------
// End of file