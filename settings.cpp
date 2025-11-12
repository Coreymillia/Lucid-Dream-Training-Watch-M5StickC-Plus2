#include "settings.h"

Settings::Settings() {
    checksPerDay = DEFAULT_CHECKS_PER_DAY;
    lastAlarmTime = 0;
    nextAlarmTime = 0;
    imuEnabled = false;
    quietHoursEnabled = true;
    brightness = DEFAULT_BRIGHTNESS;
    checkCount = 0;
    
    for (int i = 0; i < RC_TYPE_COUNT; i++) {
        rcEnabled[i] = true;
    }
}

void Settings::begin() {
    prefs.begin(NVS_NAMESPACE, false);
    load();
}

void Settings::load() {
    checksPerDay = prefs.getInt(NVS_CHECKS_PER_DAY, DEFAULT_CHECKS_PER_DAY);
    lastAlarmTime = prefs.getULong(NVS_LAST_ALARM, 0);
    nextAlarmTime = prefs.getULong(NVS_NEXT_ALARM, 0);
    imuEnabled = prefs.getBool(NVS_IMU_ENABLED, false);
    quietHoursEnabled = prefs.getBool(NVS_QUIET_HOURS, true);
    brightness = prefs.getInt(NVS_BRIGHTNESS, DEFAULT_BRIGHTNESS);
    checkCount = prefs.getInt(NVS_CHECK_COUNT, 0);
    
    for (int i = 0; i < RC_TYPE_COUNT; i++) {
        String key = String(NVS_RC_ENABLED) + String(i);
        rcEnabled[i] = prefs.getBool(key.c_str(), true);
    }
}

void Settings::save() {
    prefs.putInt(NVS_CHECKS_PER_DAY, checksPerDay);
    prefs.putULong(NVS_LAST_ALARM, lastAlarmTime);
    prefs.putULong(NVS_NEXT_ALARM, nextAlarmTime);
    prefs.putBool(NVS_IMU_ENABLED, imuEnabled);
    prefs.putBool(NVS_QUIET_HOURS, quietHoursEnabled);
    prefs.putInt(NVS_BRIGHTNESS, brightness);
    prefs.putInt(NVS_CHECK_COUNT, checkCount);
    
    for (int i = 0; i < RC_TYPE_COUNT; i++) {
        String key = String(NVS_RC_ENABLED) + String(i);
        prefs.putBool(key.c_str(), rcEnabled[i]);
    }
}

void Settings::setChecksPerDay(int checks) {
    if (checks < MIN_CHECKS_PER_DAY) checks = MIN_CHECKS_PER_DAY;
    if (checks > MAX_CHECKS_PER_DAY) checks = MAX_CHECKS_PER_DAY;
    checksPerDay = checks;
    save();
}

void Settings::setLastAlarmTime(unsigned long time) {
    lastAlarmTime = time;
    save();
}

void Settings::setNextAlarmTime(unsigned long time) {
    nextAlarmTime = time;
    save();
}

void Settings::setImuEnabled(bool enabled) {
    imuEnabled = enabled;
    save();
}

void Settings::setRCEnabled(int type, bool enabled) {
    if (type >= 0 && type < RC_TYPE_COUNT) {
        rcEnabled[type] = enabled;
        save();
    }
}

void Settings::setQuietHours(bool enabled) {
    quietHoursEnabled = enabled;
    save();
}

void Settings::setBrightness(int value) {
    if (value < 1) value = 1;
    if (value > 255) value = 255;
    brightness = value;
    save();
}

void Settings::incrementCheckCount() {
    checkCount++;
    save();
}

void Settings::reset() {
    prefs.clear();
    checksPerDay = DEFAULT_CHECKS_PER_DAY;
    lastAlarmTime = 0;
    nextAlarmTime = 0;
    imuEnabled = false;
    quietHoursEnabled = true;
    brightness = DEFAULT_BRIGHTNESS;
    checkCount = 0;
    
    for (int i = 0; i < RC_TYPE_COUNT; i++) {
        rcEnabled[i] = true;
    }
    save();
}
