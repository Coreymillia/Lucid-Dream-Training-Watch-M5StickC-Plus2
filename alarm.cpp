#include "alarm.h"

Alarm::Alarm(Settings* sett) {
    settings = sett;
    isActive = false;
    isDismissed = false;
    alarmStartTime = 0;
    lastChirpTime = 0;
    chirpCount = 0;
    currentRCType = 0;
}

void Alarm::begin() {
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWriteTone(BUZZER_CHANNEL, 0);
    
    if (settings->nextAlarmTime == 0) {
        scheduleNext(millis());
    }
}

void Alarm::update(unsigned long currentTime) {
    if (isActive && !isDismissed) {
        if (currentTime - lastChirpTime >= BUZZER_CHIRP_INTERVAL) {
            if (chirpCount < BUZZER_CHIRP_COUNT) {
                playChirp();
                chirpCount++;
                lastChirpTime = currentTime;
            } else {
                chirpCount = 0;
                lastChirpTime = currentTime + 500;
            }
        }
    } else {
        stopBuzzer();
    }
    
    if (!isActive && currentTime >= settings->nextAlarmTime) {
        if (!isQuietHours()) {
            trigger();
        } else {
            scheduleNext(currentTime);
        }
    }
}

void Alarm::trigger() {
    isActive = true;
    isDismissed = false;
    alarmStartTime = millis();
    chirpCount = 0;
    lastChirpTime = 0;
    currentRCType = getRandomRCType();
    settings->setLastAlarmTime(millis());
}

void Alarm::dismiss() {
    isDismissed = true;
    isActive = false;
    stopBuzzer();
    settings->incrementCheckCount();
    scheduleNext(millis());
}

bool Alarm::isTriggered() {
    return isActive && !isDismissed;
}

int Alarm::getCurrentRCType() {
    return currentRCType;
}

void Alarm::playChirp() {
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY);
    delay(BUZZER_CHIRP_DURATION);
    ledcWriteTone(BUZZER_CHANNEL, 0);
}

void Alarm::stopBuzzer() {
    ledcWriteTone(BUZZER_CHANNEL, 0);
}

bool Alarm::isQuietHours() {
    if (!settings->quietHoursEnabled) return false;
    
    auto dt = M5.Rtc.getDateTime();
    int hour = (dt.time.hours + TIMEZONE) % 24;
    
    if (QUIET_START_HOUR < QUIET_END_HOUR) {
        return hour >= QUIET_START_HOUR && hour < QUIET_END_HOUR;
    } else {
        return hour >= QUIET_START_HOUR || hour < QUIET_END_HOUR;
    }
}

int Alarm::getRandomRCType() {
    int enabledTypes[RC_TYPE_COUNT];
    int enabledCount = 0;
    
    for (int i = 0; i < RC_TYPE_COUNT; i++) {
        if (settings->rcEnabled[i]) {
            enabledTypes[enabledCount++] = i;
        }
    }
    
    if (enabledCount == 0) return 0;
    
    int randomIndex = random(enabledCount);
    return enabledTypes[randomIndex];
}

unsigned long Alarm::calculateNextInterval() {
    unsigned long intervalMinutes = random(MIN_INTERVAL_MINUTES, MAX_INTERVAL_MINUTES + 1);
    return intervalMinutes * 60 * 1000;
}

void Alarm::scheduleNext(unsigned long currentTime) {
    unsigned long interval = calculateNextInterval();
    settings->setNextAlarmTime(currentTime + interval);
}
