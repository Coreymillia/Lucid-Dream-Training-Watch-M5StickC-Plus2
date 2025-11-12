#ifndef ALARM_H
#define ALARM_H

#include "config.h"
#include "settings.h"
#include <M5StickCPlus2.h>

class Alarm {
private:
    Settings* settings;
    bool isActive;
    bool isDismissed;
    unsigned long alarmStartTime;
    unsigned long lastChirpTime;
    int chirpCount;
    int currentRCType;
    
    void playChirp();
    void stopBuzzer();
    bool isQuietHours();
    int getRandomRCType();
    unsigned long calculateNextInterval();
    
public:
    Alarm(Settings* sett);
    void begin();
    void update(unsigned long currentTime);
    void trigger();
    void dismiss();
    bool isTriggered();
    int getCurrentRCType();
    void scheduleNext(unsigned long currentTime);
};

#endif
