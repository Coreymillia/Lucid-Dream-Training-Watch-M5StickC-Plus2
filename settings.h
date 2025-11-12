#ifndef SETTINGS_H
#define SETTINGS_H

#include <Preferences.h>
#include "config.h"

class Settings {
private:
    Preferences prefs;
    
public:
    int checksPerDay;
    unsigned long lastAlarmTime;
    unsigned long nextAlarmTime;
    bool imuEnabled;
    bool rcEnabled[RC_TYPE_COUNT];
    bool quietHoursEnabled;
    int brightness;
    int checkCount;
    
    Settings();
    void begin();
    void load();
    void save();
    void setChecksPerDay(int checks);
    void setLastAlarmTime(unsigned long time);
    void setNextAlarmTime(unsigned long time);
    void setImuEnabled(bool enabled);
    void setRCEnabled(int type, bool enabled);
    void setQuietHours(bool enabled);
    void setBrightness(int value);
    void incrementCheckCount();
    void reset();
};

#endif
