#include "display.h"

Display::Display(Settings* sett) {
    settings = sett;
    currentBrightness = DEFAULT_BRIGHTNESS;
    lastActivityTime = 0;
    screenOn = true;
    currentMenu = MENU_CLOCK;
    currentSetting = 0;
}

void Display::begin() {
    M5.Display.setRotation(3);
    M5.Display.setTextDatum(top_left);
    M5.Display.setBrightness(settings->brightness);
    currentBrightness = settings->brightness;
}

void Display::update(unsigned long currentTime) {
    if (screenOn && currentTime - lastActivityTime > SCREEN_TIMEOUT_MS) {
        sleep();
    }
    
    if (currentBrightness != settings->brightness) {
        M5.Display.setBrightness(settings->brightness);
        currentBrightness = settings->brightness;
    }
}

void Display::showClock() {
    M5.Display.clear();
    
    auto dt = M5.Rtc.getDateTime();
    int hour = (dt.time.hours + TIMEZONE) % 24;
    int minute = dt.time.minutes;
    
    M5.Display.setTextColor(COLOR_YELLOW);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(40, 50);
    M5.Display.printf("%02d:%02d", hour, minute);
    
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 20);
    M5.Display.setTextColor(COLOR_GREEN);
    M5.Display.printf("Lucid Watch");
    
    drawBattery();
    
    M5.Display.setCursor(10, 100);
    M5.Display.setTextColor(COLOR_CYAN);
    M5.Display.setTextSize(1);
    M5.Display.printf("Checks: %d", settings->checkCount);
}

void Display::showRealityCheck(int type) {
    M5.Display.clear();
    
    M5.Display.setTextColor(COLOR_RED);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 10);
    M5.Display.printf("ARE YOU");
    M5.Display.setCursor(10, 35);
    M5.Display.printf("DREAMING?");
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_WHITE);
    
    switch(type) {
        case RC_FINGER_PALM:
            showRCFingerPalm();
            break;
        case RC_DIGITAL_CLOCK:
            showRCDigitalClock();
            break;
        case RC_TEXT_STABILITY:
            showRCTextStability();
            break;
        case RC_NOSE_BREATHING:
            showRCNoseBreathing();
            break;
        case RC_IMU_PHYSICS:
            showRCIMUPhysics();
            break;
    }
    
    M5.Display.setCursor(10, 115);
    M5.Display.setTextColor(COLOR_GREEN);
    M5.Display.printf("Press B when done");
}

void Display::showRCFingerPalm() {
    M5.Display.setCursor(10, 70);
    M5.Display.printf("Push finger");
    M5.Display.setCursor(10, 85);
    M5.Display.printf("through palm");
}

void Display::showRCDigitalClock() {
    M5.Display.setCursor(10, 70);
    M5.Display.printf("Look at clock");
    M5.Display.setCursor(10, 85);
    M5.Display.printf("Look away, back");
    M5.Display.setCursor(10, 100);
    M5.Display.printf("Did it change?");
}

void Display::showRCTextStability() {
    M5.Display.setCursor(10, 70);
    M5.Display.setTextColor(COLOR_CYAN);
    M5.Display.printf("REALITY CHECK");
    M5.Display.setCursor(10, 85);
    M5.Display.setTextColor(COLOR_WHITE);
    M5.Display.printf("Look away");
    M5.Display.setCursor(10, 100);
    M5.Display.printf("Text stable?");
}

void Display::showRCNoseBreathing() {
    M5.Display.setCursor(10, 70);
    M5.Display.printf("Pinch your nose");
    M5.Display.setCursor(10, 85);
    M5.Display.printf("Can you still");
    M5.Display.setCursor(10, 100);
    M5.Display.printf("breathe?");
}

void Display::showRCIMUPhysics() {
    M5.Display.setCursor(10, 70);
    M5.Display.printf("Jump lightly");
    M5.Display.setCursor(10, 85);
    M5.Display.printf("Does gravity");
    M5.Display.setCursor(10, 100);
    M5.Display.printf("feel normal?");
}

void Display::showMenu() {
    M5.Display.clear();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_YELLOW);
    M5.Display.setCursor(10, 10);
    M5.Display.printf("MENU");
    
    const char* menus[] = {"Clock", "Set Time", "Settings", "Stats", "Test Buzzer"};
    
    for (int i = 0; i < MENU_COUNT; i++) {
        M5.Display.setCursor(10, 40 + (i * 20));
        if (i == currentMenu) {
            M5.Display.setTextColor(COLOR_GREEN);
            M5.Display.printf("> %s", menus[i]);
        } else {
            M5.Display.setTextColor(COLOR_WHITE);
            M5.Display.printf("  %s", menus[i]);
        }
    }
}

void Display::showSettings() {
    M5.Display.clear();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_YELLOW);
    M5.Display.setCursor(10, 10);
    M5.Display.printf("SETTINGS");
    
    const char* labels[] = {
        "Checks/day",
        "IMU Trigger",
        "Quiet Hours",
        "Brightness"
    };
    
    for (int i = 0; i < SETTING_COUNT - 1; i++) {  // -1 to skip RC_TYPES for now
        int yPos = 35 + (i * 20);
        M5.Display.setCursor(10, yPos);
        
        // Highlight current setting
        if (i == currentSetting) {
            M5.Display.setTextColor(COLOR_GREEN);
            M5.Display.printf(">");
        } else {
            M5.Display.setTextColor(COLOR_WHITE);
            M5.Display.printf(" ");
        }
        
        M5.Display.setCursor(20, yPos);
        M5.Display.setTextColor(i == currentSetting ? COLOR_GREEN : COLOR_WHITE);
        
        // Display setting value
        switch(i) {
            case SETTING_CHECKS_PER_DAY:
                M5.Display.printf("%s: %d", labels[i], settings->checksPerDay);
                break;
            case SETTING_IMU_TRIGGER:
                M5.Display.printf("%s: %s", labels[i], settings->imuEnabled ? "ON" : "OFF");
                break;
            case SETTING_QUIET_HOURS:
                M5.Display.printf("%s: %s", labels[i], settings->quietHoursEnabled ? "ON" : "OFF");
                break;
            case SETTING_BRIGHTNESS:
                M5.Display.printf("%s: %d", labels[i], settings->brightness);
                break;
        }
    }
    
    M5.Display.setCursor(10, 115);
    M5.Display.setTextColor(COLOR_CYAN);
    M5.Display.printf("A=Next B=Change");
}

void Display::showStats() {
    M5.Display.clear();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_YELLOW);
    M5.Display.setCursor(10, 10);
    M5.Display.printf("STATISTICS");
    
    M5.Display.setTextColor(COLOR_WHITE);
    M5.Display.setCursor(10, 40);
    M5.Display.printf("Total Checks:");
    M5.Display.setCursor(10, 60);
    M5.Display.setTextColor(COLOR_GREEN);
    M5.Display.setTextSize(2);
    M5.Display.printf("%d", settings->checkCount);
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_WHITE);
    M5.Display.setCursor(10, 95);
    
    unsigned long nextIn = (settings->nextAlarmTime - millis()) / 60000;
    if (settings->nextAlarmTime > millis()) {
        M5.Display.printf("Next: %lu min", nextIn);
    } else {
        M5.Display.printf("Next: Soon");
    }
}

void Display::showTestBuzzer() {
    M5.Display.clear();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_YELLOW);
    M5.Display.setCursor(10, 50);
    M5.Display.printf("Press B to test");
    M5.Display.setCursor(10, 70);
    M5.Display.printf("buzzer sound");
}

void Display::wake() {
    screenOn = true;
    M5.Display.setBrightness(settings->brightness);
    lastActivityTime = millis();
}

void Display::sleep() {
    screenOn = false;
    M5.Display.setBrightness(IDLE_BRIGHTNESS);
}

bool Display::isAwake() {
    return screenOn;
}

void Display::nextMenu() {
    currentMenu = (currentMenu + 1) % MENU_COUNT;
}

void Display::previousMenu() {
    currentMenu = (currentMenu - 1 + MENU_COUNT) % MENU_COUNT;
}

void Display::selectMenu() {
    // Selection logic handled in main.cpp
}

void Display::updateActivity() {
    lastActivityTime = millis();
    if (!screenOn) {
        wake();
    }
}

void Display::drawBattery() {
    // Will be updated by power module
}

void Display::drawTime() {
    auto dt = M5.Rtc.getDateTime();
    int hour = (dt.time.hours + TIMEZONE) % 24;
    int minute = dt.time.minutes;
    
    M5.Display.setTextColor(COLOR_WHITE);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(170, 10);
    M5.Display.printf("%02d:%02d", hour, minute);
}

void Display::showSetTime(int hour, int minute, bool editingHour) {
    M5.Display.clear();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_YELLOW);
    M5.Display.setCursor(10, 10);
    M5.Display.printf("SET TIME");
    
    M5.Display.setTextSize(3);
    M5.Display.setCursor(20, 50);
    
    // Highlight hour or minute
    if (editingHour) {
        M5.Display.setTextColor(COLOR_GREEN);
    } else {
        M5.Display.setTextColor(COLOR_WHITE);
    }
    M5.Display.printf("%02d", hour);
    
    M5.Display.setTextColor(COLOR_WHITE);
    M5.Display.printf(":");
    
    if (!editingHour) {
        M5.Display.setTextColor(COLOR_GREEN);
    } else {
        M5.Display.setTextColor(COLOR_WHITE);
    }
    M5.Display.printf("%02d", minute);
    
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 100);
    M5.Display.setTextColor(COLOR_CYAN);
    M5.Display.printf("A=Change B=Switch");
    M5.Display.setCursor(10, 115);
    M5.Display.printf("PWR=Save & Exit");
}
