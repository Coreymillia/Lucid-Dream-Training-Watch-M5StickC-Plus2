#include "imu.h"
#include <cmath>

IMU::IMU() {
    dataValid = false;
    lastUpdateTime = 0;
}

void IMU::begin() {
    auto imu_update = M5.Imu.update();
    if (imu_update) {
        currentData = M5.Imu.getImuData();
        previousData = currentData;
        dataValid = true;
    }
}

void IMU::update() {
    auto imu_update = M5.Imu.update();
    if (imu_update) {
        previousData = currentData;
        currentData = M5.Imu.getImuData();
        dataValid = true;
        lastUpdateTime = millis();
    }
}

float IMU::getAccelMagnitude() {
    if (!dataValid) return 0.0f;
    
    float x = currentData.accel.x;
    float y = currentData.accel.y;
    float z = currentData.accel.z;
    
    return sqrt(x*x + y*y + z*z);
}

float IMU::getGyroMagnitude() {
    if (!dataValid) return 0.0f;
    
    float x = currentData.gyro.x;
    float y = currentData.gyro.y;
    float z = currentData.gyro.z;
    
    return sqrt(x*x + y*y + z*z);
}

bool IMU::detectFall() {
    if (!dataValid) return false;
    
    float accelMag = getAccelMagnitude();
    
    if (accelMag < 2.0f) {
        return true;
    }
    
    return false;
}

bool IMU::detectWeirdPhysics() {
    if (!dataValid) return false;
    
    float accelMag = getAccelMagnitude();
    
    if (accelMag > IMU_WEIRD_PHYSICS_THRESHOLD || accelMag < 5.0f) {
        return true;
    }
    
    return false;
}

bool IMU::detectRotation() {
    if (!dataValid) return false;
    
    float gyroMag = getGyroMagnitude();
    
    if (gyroMag > IMU_ROTATION_THRESHOLD) {
        return true;
    }
    
    return false;
}

bool IMU::shouldTriggerRC() {
    if (!dataValid) return false;
    
    return detectFall() || detectWeirdPhysics() || detectRotation();
}

float IMU::getAccelX() {
    return dataValid ? currentData.accel.x : 0.0f;
}

float IMU::getAccelY() {
    return dataValid ? currentData.accel.y : 0.0f;
}

float IMU::getAccelZ() {
    return dataValid ? currentData.accel.z : 0.0f;
}
