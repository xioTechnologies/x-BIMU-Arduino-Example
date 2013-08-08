/*
    XbimuReceiver.h
    Author: Seb Madgwick

    C++ library for receiving data from the x-BIMU.  Only supports binary
    packet mode, 'binary packet mode' must be enabled in x-BIMU settings
    (BP = 1).

    See x_BIMU_Arduino_Example.ino for example usage.
*/

#ifndef XbimuReceiver_h
#define XbimuReceiver_h

//------------------------------------------------------------------------------
// Definitions

typedef struct {
    int w;
    int x;
    int y;
    int z;
    unsigned char counter;
} QuaternionStruct;

typedef struct {
    int gyrX;   /* gyroscope x axis in 0.1 dps  */
    int gyrY;   /* gyroscope y axis in 0.1 dps  */
    int gyrZ;   /* gyroscope z axis in 0.1 dps  */
    int accX;   /* accelerometer x axis in mg   */
    int accY;   /* accelerometer y axis in mg   */
    int accZ;   /* accelerometer z axis in mg   */
    int magX;   /* magnetometer x axis in mGa   */
    int magY;   /* magnetometer y axis in mGa   */
    int magZ;   /* magnetometer z axis in mGa   */
    unsigned char counter;
} SensorStruct;

typedef struct {
    int voltage;    /* battery voltage in mV */
    unsigned char counter;
} BatteryStruct;

//------------------------------------------------------------------------------
// Class declaration

class XbimuReceiver {
    public:
        XbimuReceiver(void);
        void processNewChar(unsigned char c);
        bool isQuaternionGetReady(void) const;
        bool isSensorGetReady(void) const;
        bool isBatteryGetReady(void) const;
        QuaternionStruct getQuaternion(void);
        SensorStruct getSensor(void);
        BatteryStruct getBattery(void);

    private:
        unsigned char buf[256];
        unsigned char bufIndex;
        unsigned char bufCount;
        bool inSync;
        QuaternionStruct quaternionStruct;
        SensorStruct sensorStruct;
        BatteryStruct batteryStruct;
        bool quaternionGetReady;
        bool sensorGetReady;
        bool batteryGetReady;
        unsigned char calcChecksum(unsigned char packetLength) const;
};

#endif

//------------------------------------------------------------------------------
// End of file
