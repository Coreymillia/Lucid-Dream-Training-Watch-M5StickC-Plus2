#ifndef DISPLAY_H
#define DISPLAY_H

#include <M5StickCPlus2.h>
#include "config.h"
#include "settings.h"

class Display {
private:
    Settings* settings;
    int currentBrightness;
    unsigned long lastActivityTime;
    bool screenOn;
    int currentMenu;
    int currentSetting;
    
public:
    Display(Settings* sett);
    void begin();
    void update(unsigned long currentTime);
    void showClock();
    void showRealityCheck(int type);
    void showMenu();
    void showSettings();
    void showStats();
    void showTestBuzzer();
    void showSetTime(int hour, int minute, bool editingHour);
    void wake();
    void sleep();
    bool isAwake();
    void nextMenu();
    void previousMenu();
    void selectMenu();
    void updateActivity();
    int getCurrentMenu() { return currentMenu; }  // Getter for menu index
    int getCurrentSetting() { return currentSetting; }  // Getter for setting index
    void nextSetting() { currentSetting = (currentSetting + 1) % (SETTING_COUNT - 1); }  // Skip RC_TYPES for now
    
private:
    void showRCFingerPalm();
    void showRCDigitalClock();
    void showRCTextStability();
    void showRCNoseBreathing();
    void showRCIMUPhysics();
    void drawBattery();
    void drawTime();
};

#endif
