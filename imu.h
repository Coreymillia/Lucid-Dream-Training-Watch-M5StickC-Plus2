#ifndef IMU_H
#define IMU_H

#include <M5StickCPlus2.h>
#include "config.h"

class IMU {
private:
    m5::imu_data_t currentData;
    m5::imu_data_t previousData;
    bool dataValid;
    unsigned long lastUpdateTime;
    
    float getAccelMagnitude();
    float getGyroMagnitude();
    
public:
    IMU();
    void begin();
    void update();
    bool detectFall();
    bool detectWeirdPhysics();
    bool detectRotation();
    bool shouldTriggerRC();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
};

#endif
