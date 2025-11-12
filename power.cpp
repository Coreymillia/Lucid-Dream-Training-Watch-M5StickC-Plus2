#include "power.h"

Power::Power() {
    batteryPercent = 0;
    batteryVoltage = 0;
    updateCounter = 0;
}

void Power::begin() {
    update();
}

void Power::update() {
    updateCounter++;
    if (updateCounter < 10) return;
    
    updateCounter = 0;
    
    batteryVoltage = M5.Power.getBatteryVoltage();
    
    float normalized_max = (MAX_VOLTAGE - MIN_VOLTAGE) * 1.0f;
    int normalized_vol = batteryVoltage - MIN_VOLTAGE;
    batteryPercent = (int)((normalized_vol * 1.0f) / normalized_max * 100.0f);
    
    if (batteryPercent < 0) batteryPercent = 0;
    if (batteryPercent > 100) batteryPercent = 100;
}

int Power::getBatteryPercent() {
    return batteryPercent;
}

int Power::getBatteryVoltage() {
    return batteryVoltage;
}

void Power::sleep() {
    M5.Display.setBrightness(0);
}

void Power::wake() {
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
}
