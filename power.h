#ifndef POWER_H
#define POWER_H

#include <M5StickCPlus2.h>
#include "config.h"

class Power {
private:
    int batteryPercent;
    int batteryVoltage;
    int updateCounter;
    
public:
    Power();
    void begin();
    void update();
    int getBatteryPercent();
    int getBatteryVoltage();
    void sleep();
    void wake();
};

#endif
