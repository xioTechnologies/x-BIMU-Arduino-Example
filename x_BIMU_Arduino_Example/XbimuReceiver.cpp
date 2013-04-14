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
    QUAT_LENGTH = 10,
    SENS_LENGTH = 20,
    BATT_LENGTH = 4,
    MAX_LENGTH = 20 /* maximum packet length of all packet types */
};

//------------------------------------------------------------------------------
// Variables

// Serial stream decoding
unsigned char buf[256];
unsigned char bufIndex = 0;
unsigned char bufCount = 0;
bool inSync = false;

// Decoded data
QuaternionStruct quaternionStruct = { 0, 0, 0, 0 };
SensorStruct sensorStruct = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int battery = 0;

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
                quaternionStruct.w = (int)(((int)buf[bufIndex - 9] << 8) | (int)buf[bufIndex - 8]);
                quaternionStruct.x = (int)(((int)buf[bufIndex - 7] << 8) | (int)buf[bufIndex - 6]);
                quaternionStruct.y = (int)(((int)buf[bufIndex - 5] << 8) | (int)buf[bufIndex - 3]);
                quaternionStruct.z = (int)(((int)buf[bufIndex - 3] << 8) | (int)buf[bufIndex - 2]);
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
                sensorStruct.gyrX = (int)(((int)buf[bufIndex - 19] << 8) | (int)buf[bufIndex - 18]);
                sensorStruct.gyrY = (int)(((int)buf[bufIndex - 17] << 8) | (int)buf[bufIndex - 16]);
                sensorStruct.gyrZ = (int)(((int)buf[bufIndex - 15] << 8) | (int)buf[bufIndex - 14]);
                sensorStruct.accX = (int)(((int)buf[bufIndex - 13] << 8) | (int)buf[bufIndex - 12]);
                sensorStruct.accY = (int)(((int)buf[bufIndex - 11] << 8) | (int)buf[bufIndex - 10]);
                sensorStruct.accZ = (int)(((int)buf[bufIndex - 9] << 8) | (int)buf[bufIndex - 8]);
                sensorStruct.magX = (int)(((int)buf[bufIndex - 7] << 8) | (int)buf[bufIndex - 6]);
                sensorStruct.magY = (int)(((int)buf[bufIndex - 5] << 8) | (int)buf[bufIndex - 4]);
                sensorStruct.magZ = (int)(((int)buf[bufIndex - 3] << 8) | (int)buf[bufIndex - 2]);
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
                battery = (int)(((int)buf[bufIndex - 3] << 8) | (int)buf[bufIndex - 2]);
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

int XbimuReceiver::getBattery(void) {
    batteryGetReady = false;
    return battery;
}

//------------------------------------------------------------------------------
// End of file