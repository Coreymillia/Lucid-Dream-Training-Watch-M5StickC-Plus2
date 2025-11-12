// minimal_test_main.cpp
// Minimal, non-blocking test for M5StickC Plus2
#include <M5StickCPlus2.h>
#include <Preferences.h>

// NVS storage
Preferences preferences;

// Buzzer configuration (M5StickC Plus2 uses GPIO 2)
#define BUZZER_PIN 2
#define BUZZER_CHANNEL 0
#define BUZZER_FREQUENCY 2000
#define BUZZER_CHIRP_DURATION 100
#define BUZZER_CHIRP_COUNT 3
#define BUZZER_CHIRP_INTERVAL 150

// Timer variables
unsigned long startMillis;
unsigned long lastRender = 0;
const unsigned long RENDER_MS = 200;

// Buzzer state variables
bool buzzerActive = false;
int buzzerChirpNumber = 0;
unsigned long buzzerLastChirp = 0;
bool buzzerToneOn = false;
unsigned long buzzerToneStart = 0;

// Mode system
enum DisplayMode {
  MODE_NORMAL,
  MODE_MENU,
  MODE_SET_TIME,
  MODE_REALITY_CHECK,
  MODE_NIGHT,
  MODE_DREAM_JOURNAL
};
DisplayMode currentMode = MODE_NORMAL;

// Menu system
int menuSelection = 0;  // Which menu item is selected
const int MENU_ITEMS = 10;  // Number of menu items
const char* menuItems[] = {
  "Set Time",
  "Morning Alarm",
  "Alarms/Day",
  "Quiet Hours",
  "12/24 Format",
  "Screen Timeout",
  "Shake Sense",
  "Brightness",
  "Clock Color",
  "Test RC"  // Reality Check test
};

// Time-setting variables
int editHour = 0;
int editMinute = 0;
bool editingHour = true;  // true = editing hour, false = editing minute

// Button B hold detection (for entering menu)
unsigned long btnBPressTime = 0;
bool btnBHeld = false;
const unsigned long BTN_B_HOLD_TIME = 2000;  // 2 seconds

// Random alarm system
int alarmsPerDay = 12;  // Default: 12 reality checks per day
int quietHoursStart = 23;  // 11 PM
int quietHoursEnd = 7;     // 7 AM
unsigned long nextAlarmMinute = 0;  // Minutes since midnight
bool alarmActive = false;
int currentRealityCheck = 0;  // Which reality check to show

// Reality check auto-dismiss
unsigned long realityCheckStartTime = 0;
const unsigned long REALITY_CHECK_TIMEOUT = 10000;  // 10 seconds

// Morning Dream Journal Alarm (formerly manual alarm)
bool manualAlarmEnabled = false;
int manualAlarmHour = 7;  // Default: 7:00 AM
int manualAlarmMinute = 0;
bool manualAlarmTriggered = false;
unsigned long dreamJournalStartTime = 0;
unsigned long lastDreamJournalBeep = 0;

// Settings editing variables
bool editingManualAlarm = false;
bool editingMAHour = true;  // true = editing hour, false = editing minute
bool editingAlarmCount = false;
bool editingScreenTimeout = false;
bool editingSensitivity = false;
bool editingBrightness = false;

// Interactive Reality Check - Light Switch
bool lightSwitchOn = false;
unsigned long lightSwitchTime = 0;

// Night Mode - REM Cue System
bool nightModeActive = false;
unsigned long sleepStartTime = 0;
unsigned long lastREMCue = 0;
const unsigned long SLEEP_CYCLE = 90UL * 60 * 1000;  // 90 minutes in ms
const unsigned long NIGHT_START_DELAY = 10UL * 60 * 1000;  // 10 min delay before starting

// Screen timeout and IMU wake settings
int screenTimeoutSeconds = 15;  // Default: 15 seconds
bool screenOn = true;
unsigned long lastActivityTime = 0;
float lastAccelX = 0, lastAccelY = 0, lastAccelZ = 0;

// Sensitivity levels: 0=Light Tap, 1=Gentle, 2=Normal, 3=Firm, 4=Hard Shake, 5=Very Hard, 6=Button Only
int sensitivityLevel = 2;  // Default: Normal
const float SENSITIVITY_VALUES[] = {0.15, 0.30, 0.50, 0.80, 1.2, 1.8, 999.0};  // Thresholds (999 = disabled)
const char* SENSITIVITY_NAMES[] = {"Light Tap", "Gentle", "Normal", "Firm", "Hard", "Very Hard", "Button Only"};

// Brightness levels: 0-10 (0=dimmest, 10=brightest)
int brightnessLevel = 8;  // Default: 80%
const int BRIGHTNESS_VALUES[] = {25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 255};  // PWM values 0-255

// Clock color selection
int clockColorIndex = 0;  // Default: White
const int CLOCK_COLORS[] = {WHITE, CYAN, GREEN, YELLOW, ORANGE, MAGENTA, RED, BLUE};
const char* COLOR_NAMES[] = {"White", "Cyan", "Green", "Yellow", "Orange", "Magenta", "Red", "Blue"};
const int NUM_COLORS = 8;
bool editingClockColor = false;

// Quiet Hours editing
bool editingQuietHours = false;
bool editingQHStart = true;  // true = editing start hour, false = editing end hour

// 12/24 Hour format
bool use24HourFormat = true;  // Default: 24-hour format
bool editingTimeFormat = false;

// Reality Check Test mode
bool testingRealityCheck = false;
int testRCIndex = 0;  // Which RC to show during test

// Forward declarations
void startBuzzer();
void stopBuzzer();
void updateBuzzer();
void checkIMUActivity();
void updateScreenTimeout();
void drawTimeSetUI();
void drawNormalUI(int hh, int mm, int ss);
void drawRealityCheckUI();
void drawMenuUI();
void drawAlarmsPerDayUI();
void drawScreenTimeoutUI();
void drawSensitivityUI();
void gentleREMBeep();
void checkNightMode();
void drawNightModeUI();
void drawDreamJournalUI();
void drawBrightnessUI();
void drawClockColorUI();
void drawQuietHoursUI();
void drawTimeFormatUI();
void drawManualAlarmUI();
void scheduleNextAlarm();
bool isQuietHours(int hour);
unsigned long getCurrentMinutes(int hh, int mm);
void loadSettings();
void saveSettings();

void setup() {
  M5.begin();
  Serial.begin(115200);
  startMillis = millis();

  // Load saved settings from NVS
  loadSettings();

  // Rotate display 90 degrees counter-clockwise (landscape mode)
  M5.Display.setRotation(3);  // 0=portrait, 1=90°CW, 2=180°, 3=90°CCW

  // Initialize buzzer
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY, 8);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);  // Start silent

  M5.Display.clear();
  M5.Display.setTextSize(2);
  M5.Display.setCursor(4, 8);
  M5.Display.println("Starting...");
  delay(300);

  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setCursor(6, 6);
  M5.Display.println("Test boot OK");
  delay(300);
  M5.Display.fillScreen(BLACK);
  
  // Initialize IMU
  auto imu_update = M5.Imu.update();
  if (imu_update) {
    Serial.println("IMU initialized successfully");
    // Get initial accelerometer values
    auto data = M5.Imu.getImuData();
    lastAccelX = data.accel.x;
    lastAccelY = data.accel.y;
    lastAccelZ = data.accel.z;
  } else {
    Serial.println("Warning: IMU initialization failed");
  }
  
  Serial.println("=== LUCID DREAM WATCH STARTED ===");
  Serial.println("Random alarms enabled (2-minute intervals for testing)");
  Serial.printf("Screen timeout: %d seconds\n", screenTimeoutSeconds);
  Serial.println("Shake watch to wake screen");
  
  // Set initial brightness
  M5.Display.setBrightness(BRIGHTNESS_VALUES[brightnessLevel]);
  Serial.printf("Brightness: %d%% (PWM: %d)\n", (brightnessLevel * 10), BRIGHTNESS_VALUES[brightnessLevel]);
  
  // Schedule first alarm
  scheduleNextAlarm();
  
  // Initialize activity timer
  lastActivityTime = millis();
}

void loop() {
  M5.update(); // update button states etc.

  // Update buzzer (non-blocking)
  updateBuzzer();
  
  // Check IMU for wake-on-shake
  checkIMUActivity();
  
  // Update screen timeout
  updateScreenTimeout();
  
  // Check Night Mode REM cues
  checkNightMode();

  // Get real time from RTC
  unsigned long now = millis();
  auto dt = M5.Rtc.getDateTime();
  int hh = dt.time.hours;
  int mm = dt.time.minutes;
  int ss = dt.time.seconds;

  // Auto-return from light switch test after 10 seconds of inactivity
  if (lightSwitchTime > 0 && (now - lightSwitchTime > 10000)) {
    lightSwitchTime = 0;
    M5.Display.clear();  // Will redraw clock on next loop
  }

  // Check for manual alarm trigger (only in normal mode)
  // Check for dream journal alarm trigger (only in normal mode)
  if (currentMode == MODE_NORMAL && !alarmActive && manualAlarmEnabled && !manualAlarmTriggered) {
    if (hh == manualAlarmHour && mm == manualAlarmMinute) {
      Serial.println("DREAM JOURNAL ALARM TRIGGERED!");
      alarmActive = true;
      manualAlarmTriggered = true;  // Prevent re-trigger
      currentMode = MODE_DREAM_JOURNAL;
      dreamJournalStartTime = millis();
      lastDreamJournalBeep = 0;  // Trigger first beep immediately
      M5.Display.clear();
    }
  }
  
  // Reset manual alarm triggered flag when time passes
  if (manualAlarmTriggered && (hh != manualAlarmHour || mm != manualAlarmMinute)) {
    manualAlarmTriggered = false;
    Serial.println("Dream journal alarm ready for next trigger");
  }

  // Check for random alarm trigger (only in normal mode)
  if (currentMode == MODE_NORMAL && !alarmActive) {
    unsigned long currentMinutes = getCurrentMinutes(hh, mm);
    if (currentMinutes >= nextAlarmMinute) {
      // Check if in quiet hours
      if (!isQuietHours(hh)) {
        Serial.println("ALARM TRIGGERED! Reality check time!");
        alarmActive = true;
        currentMode = MODE_REALITY_CHECK;
        realityCheckStartTime = millis();  // Start auto-dismiss timer
        startBuzzer();
        currentRealityCheck++;  // Rotate to next reality check
        M5.Display.clear();
      } else {
        Serial.println("Alarm time but in quiet hours - scheduling next");
        scheduleNextAlarm();
      }
    }
  }

  // Display based on current mode
  if (currentMode == MODE_MENU) {
    // Menu mode - show menu selection
    static unsigned long lastMenuUpdate = 0;
    if (now - lastMenuUpdate >= 200) {
      drawMenuUI();
      lastMenuUpdate = now;
    }
  } else if (currentMode == MODE_SET_TIME) {
    // In time-setting mode - show time-setting UI
    static unsigned long lastUIUpdate = 0;
    if (now - lastUIUpdate >= 200) {  // Update UI every 200ms
      drawTimeSetUI();
      lastUIUpdate = now;
    }
  } else if (currentMode == MODE_REALITY_CHECK) {
    // Reality check mode - show reality check screen
    static unsigned long lastRCUpdate = 0;
    if (now - lastRCUpdate >= 200) {
      drawRealityCheckUI();
      lastRCUpdate = now;
    }
    
    // Auto-dismiss after 10 seconds
    if (now - realityCheckStartTime >= REALITY_CHECK_TIMEOUT) {
      Serial.println("Reality check auto-dismissed after 10 seconds");
      stopBuzzer();
      alarmActive = false;
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      scheduleNextAlarm();
      lastActivityTime = millis();  // Reset screen timeout
    }
  } else if (currentMode == MODE_NIGHT) {
    // Night mode - show night mode UI
    static unsigned long lastNightUpdate = 0;
    if (now - lastNightUpdate >= 1000) {  // Update every second
      drawNightModeUI();
      lastNightUpdate = now;
    }
  } else if (currentMode == MODE_DREAM_JOURNAL) {
    // Dream journal mode - gentle beeps every 20 seconds
    static unsigned long lastDJUpdate = 0;
    if (now - lastDJUpdate >= 200) {
      drawDreamJournalUI();
      lastDJUpdate = now;
    }
    
    // Gentle beep every 20 seconds
    if (now - lastDreamJournalBeep >= 20000) {
      gentleREMBeep();  // Reuse the gentle beep function
      lastDreamJournalBeep = now;
    }
  } else if (editingAlarmCount) {
    // Editing alarms per day
    static unsigned long lastAlarmsUpdate = 0;
    if (now - lastAlarmsUpdate >= 200) {
      drawAlarmsPerDayUI();
      lastAlarmsUpdate = now;
    }
  } else if (editingManualAlarm) {
    // Editing manual alarm
    static unsigned long lastMAUpdate = 0;
    if (now - lastMAUpdate >= 200) {
      drawManualAlarmUI();
      lastMAUpdate = now;
    }
  } else if (editingScreenTimeout) {
    // Editing screen timeout
    static unsigned long lastTimeoutUpdate = 0;
    if (now - lastTimeoutUpdate >= 200) {
      drawScreenTimeoutUI();
      lastTimeoutUpdate = now;
    }
  } else if (editingSensitivity) {
    // Editing shake sensitivity
    static unsigned long lastSensUpdate = 0;
    if (now - lastSensUpdate >= 200) {
      drawSensitivityUI();
      lastSensUpdate = now;
    }
  } else if (editingBrightness) {
    // Editing brightness
    static unsigned long lastBrightnessUpdate = 0;
    if (now - lastBrightnessUpdate >= 200) {
      drawBrightnessUI();
      lastBrightnessUpdate = now;
    }
  } else if (editingClockColor) {
    // Editing clock color
    static unsigned long lastColorUpdate = 0;
    if (now - lastColorUpdate >= 200) {
      drawClockColorUI();
      lastColorUpdate = now;
    }
  } else if (editingQuietHours) {
    // Editing quiet hours
    static unsigned long lastQHUpdate = 0;
    if (now - lastQHUpdate >= 200) {
      drawQuietHoursUI();
      lastQHUpdate = now;
    }
  } else if (editingTimeFormat) {
    // Editing time format
    static unsigned long lastTFUpdate = 0;
    if (now - lastTFUpdate >= 200) {
      drawTimeFormatUI();
      lastTFUpdate = now;
    }
  } else if (testingRealityCheck) {
    // Testing reality checks
    static unsigned long lastRCTest = 0;
    if (now - lastRCTest >= 200) {
      drawRealityCheckUI();
      lastRCTest = now;
    }
  } else {
    // Normal mode - show clock with LARGE time display (skip if light switch test active)
    if (now - lastRender >= RENDER_MS && screenOn && lightSwitchTime == 0) {
      lastRender = now;
      M5.Display.fillRect(0, 0, 240, 135, BLACK);
      
      // Format time based on 12/24 hour setting
      if (use24HourFormat) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hh, mm, ss);
        M5.Display.setCursor(15, 40);
        M5.Display.setTextSize(4);
        M5.Display.setTextColor(CLOCK_COLORS[clockColorIndex], BLACK);
        M5.Display.println(buf);
      } else {
        // 12-hour format with AM/PM on separate line
        int displayHour = hh % 12;
        if (displayHour == 0) displayHour = 12;  // 0 -> 12
        const char* ampm = (hh >= 12) ? "PM" : "AM";
        
        // Time on first line
        char buf[32];
        snprintf(buf, sizeof(buf), "%2d:%02d:%02d", displayHour, mm, ss);
        M5.Display.setCursor(15, 35);
        M5.Display.setTextSize(4);
        M5.Display.setTextColor(CLOCK_COLORS[clockColorIndex], BLACK);
        M5.Display.println(buf);
        
        // AM/PM on second line, smaller
        M5.Display.setTextSize(2);
        M5.Display.setCursor(100, 70);
        M5.Display.println(ampm);
      }
    }
  }

  // Button behavior depends on mode
  if (currentMode == MODE_NORMAL && !editingAlarmCount && !editingManualAlarm && !editingScreenTimeout && !editingSensitivity && !editingBrightness && !editingClockColor && !editingQuietHours && !editingTimeFormat && !testingRealityCheck) {
    // NORMAL MODE (not editing): Button A for light switch OR HOLD for Night Mode
    
    // Check for HOLD (1 second) to enter Night Mode
    if (M5.BtnA.pressedFor(1000) && screenOn) {
      Serial.println("BTN A HELD - ENTERING NIGHT MODE");
      nightModeActive = true;
      sleepStartTime = millis();
      currentMode = MODE_NIGHT;
      M5.Display.clear();
      drawNightModeUI();
      return;  // Skip rest of button logic
    }
    
    if (M5.BtnA.wasPressed()) {
      lastActivityTime = millis();  // Reset timeout
      
      if (!screenOn) {
        // Screen is off - just wake it
        Serial.println("BTN A PRESSED - WAKING SCREEN");
        screenOn = true;
        M5.Display.wakeup();
        M5.Display.setBrightness(BRIGHTNESS_VALUES[brightnessLevel]);
      } else {
        // Screen is on - trigger LIGHT SWITCH REALITY CHECK
        Serial.println("BTN A PRESSED - LIGHT SWITCH RC");
        lightSwitchOn = !lightSwitchOn;
        lightSwitchTime = millis();  // Reset timeout on each press
        
        // Toggle screen to full white or black
        M5.Display.fillScreen(lightSwitchOn ? WHITE : BLACK);
        
        // Add text indicator
        M5.Display.setTextColor(lightSwitchOn ? BLACK : WHITE);
        M5.Display.setTextSize(2);
        M5.Display.setCursor(20, 50);
        M5.Display.println(lightSwitchOn ? "LIGHT ON" : "LIGHT OFF");
      }
    }
    if (M5.BtnA.wasReleased()) {
      Serial.println("BTN A RELEASED");
    }
  } else if (currentMode == MODE_MENU) {
    // MENU MODE: Button A scrolls down
    if (M5.BtnA.wasPressed()) {
      menuSelection = (menuSelection + 1) % MENU_ITEMS;
      Serial.printf("Menu selection: %d (%s)\n", menuSelection, menuItems[menuSelection]);
      drawMenuUI();
    }
  } else if (currentMode == MODE_SET_TIME) {
    // TIME-SETTING MODE: Button A increments hour or minute
    if (M5.BtnA.wasPressed()) {
      if (editingHour) {
        editHour = (editHour + 1) % 24;  // 0-23
        Serial.printf("Hour changed to: %d\n", editHour);
      } else {
        editMinute = (editMinute + 1) % 60;  // 0-59
        Serial.printf("Minute changed to: %d\n", editMinute);
      }
      // Redraw immediately to show change
      drawTimeSetUI();
    }
  } else if (editingAlarmCount) {
    // EDITING ALARMS/DAY: Button A increments
    if (M5.BtnA.wasPressed()) {
      alarmsPerDay++;
      if (alarmsPerDay > 20) alarmsPerDay = 20;
      Serial.printf("Alarms/day: %d\n", alarmsPerDay);
      drawAlarmsPerDayUI();
    }
  } else if (editingManualAlarm) {
    // EDITING MANUAL ALARM: Button A toggles ON/OFF or increments time
    if (M5.BtnA.wasPressed()) {
      if (editingMAHour) {
        manualAlarmHour = (manualAlarmHour + 1) % 24;
        Serial.printf("Manual alarm hour: %d\n", manualAlarmHour);
      } else {
        manualAlarmMinute = (manualAlarmMinute + 1) % 60;
        Serial.printf("Manual alarm minute: %d\n", manualAlarmMinute);
      }
      drawManualAlarmUI();
    }
  } else if (editingScreenTimeout) {
    // EDITING SCREEN TIMEOUT: Button A increments (5 sec steps, then minutes, then always on)
    if (M5.BtnA.wasPressed()) {
      if (screenTimeoutSeconds == 0) {
        screenTimeoutSeconds = 5;  // Always On -> 5 seconds
      } else if (screenTimeoutSeconds < 60) {
        screenTimeoutSeconds += 5;  // 5-60 seconds in 5 sec steps
      } else if (screenTimeoutSeconds == 60) {
        screenTimeoutSeconds = 120;  // 60 sec -> 2 minutes
      } else if (screenTimeoutSeconds == 120) {
        screenTimeoutSeconds = 180;  // 2 min -> 3 minutes
      } else if (screenTimeoutSeconds == 180) {
        screenTimeoutSeconds = 240;  // 3 min -> 4 minutes
      } else if (screenTimeoutSeconds == 240) {
        screenTimeoutSeconds = 300;  // 4 min -> 5 minutes
      } else if (screenTimeoutSeconds == 300) {
        screenTimeoutSeconds = 0;  // 5 min -> Always On
      }
      
      if (screenTimeoutSeconds == 0) {
        Serial.println("Screen timeout: Always On");
      } else {
        Serial.printf("Screen timeout: %d sec\n", screenTimeoutSeconds);
      }
      drawScreenTimeoutUI();
    }
  } else if (editingSensitivity) {
    // EDITING SENSITIVITY: Button A increments level
    if (M5.BtnA.wasPressed()) {
      sensitivityLevel++;
      if (sensitivityLevel > 6) sensitivityLevel = 6;  // Max: Button Only
      if (sensitivityLevel == 6) {
        Serial.println("Sensitivity: Button Only (shake-to-wake DISABLED)");
      } else {
        Serial.printf("Sensitivity: %s (%.2f)\n", SENSITIVITY_NAMES[sensitivityLevel], SENSITIVITY_VALUES[sensitivityLevel]);
      }
      drawSensitivityUI();
    }
  } else if (editingBrightness) {
    // EDITING BRIGHTNESS: Button A increments level
    if (M5.BtnA.wasPressed()) {
      brightnessLevel++;
      if (brightnessLevel > 10) brightnessLevel = 10;  // Max: 100%
      M5.Display.setBrightness(BRIGHTNESS_VALUES[brightnessLevel]);
      Serial.printf("Brightness: %d%% (PWM: %d)\n", (brightnessLevel * 10), BRIGHTNESS_VALUES[brightnessLevel]);
      drawBrightnessUI();
    }
  } else if (editingClockColor) {
    // EDITING CLOCK COLOR: Button A cycles to next color
    if (M5.BtnA.wasPressed()) {
      clockColorIndex = (clockColorIndex + 1) % NUM_COLORS;
      Serial.printf("Clock color: %s\n", COLOR_NAMES[clockColorIndex]);
      drawClockColorUI();
    }
  } else if (editingQuietHours) {
    // EDITING QUIET HOURS: Button A increments hour
    if (M5.BtnA.wasPressed()) {
      if (editingQHStart) {
        quietHoursStart = (quietHoursStart + 1) % 24;
        Serial.printf("Quiet hours start: %02d:00\n", quietHoursStart);
      } else {
        quietHoursEnd = (quietHoursEnd + 1) % 24;
        Serial.printf("Quiet hours end: %02d:00\n", quietHoursEnd);
      }
      drawQuietHoursUI();
    }
  } else if (editingTimeFormat) {
    // EDITING TIME FORMAT: Button A toggles 12/24 hour format
    if (M5.BtnA.wasPressed()) {
      use24HourFormat = !use24HourFormat;
      Serial.printf("Time format: %s\n", use24HourFormat ? "24 Hour" : "12 Hour");
      drawTimeFormatUI();
    }
  } else if (testingRealityCheck) {
    // TESTING REALITY CHECKS: Button A cycles to next RC
    if (M5.BtnA.wasPressed()) {
      testRCIndex = (testRCIndex + 1) % 9;  // Cycle through 9 reality checks
      currentRealityCheck = testRCIndex;  // Update which one to show
      Serial.printf("Testing RC #%d\n", testRCIndex);
      drawRealityCheckUI();
    }
  }

  // Button B behavior
  if (currentMode == MODE_NORMAL && !editingAlarmCount && !editingManualAlarm && !editingScreenTimeout && !editingSensitivity && !editingBrightness && !editingClockColor && !editingQuietHours && !editingTimeFormat && !testingRealityCheck) {
    // NORMAL MODE (not editing): Hold Button B for 2 seconds to enter MENU
    if (M5.BtnB.isPressed()) {
      if (btnBPressTime == 0) {
        btnBPressTime = now;  // Start timing
        Serial.println("BTN B PRESSED - TIMING...");
      } else if (!btnBHeld && (now - btnBPressTime >= BTN_B_HOLD_TIME)) {
        // Held for 2 seconds - enter MENU mode
        btnBHeld = true;
        Serial.println("BTN B HELD 2 SEC - ENTERING MENU");
        currentMode = MODE_MENU;
        menuSelection = 0;  // Start at first menu item
        M5.Display.clear();
      }
    } else {
      // Button released
      if (btnBPressTime > 0 && !btnBHeld) {
        // Was a short press, not a hold
        Serial.println("BTN B SHORT PRESS");
        M5.Display.fillRect(0, 40, 160, 24, BLACK);
        M5.Display.setCursor(6, 40);
        M5.Display.println("Hold 2 sec");
      }
      btnBPressTime = 0;
      btnBHeld = false;
    }
  } else if (currentMode == MODE_MENU) {
    // MENU MODE: Button B selects menu item
    if (M5.BtnB.wasPressed()) {
      Serial.printf("Selected: %s\n", menuItems[menuSelection]);
      if (menuSelection == 0) {
        // Set Time
        currentMode = MODE_SET_TIME;
        editHour = hh;
        editMinute = mm;
        editingHour = true;
        M5.Display.clear();
      } else if (menuSelection == 1) {
        // Manual Alarm
        editingManualAlarm = true;
        editingMAHour = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 2) {
        // Alarms per Day
        editingAlarmCount = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 3) {
        // Quiet Hours
        editingQuietHours = true;
        editingQHStart = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 4) {
        // 12/24 Format
        editingTimeFormat = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 5) {
        // Screen Timeout
        editingScreenTimeout = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 6) {
        // Shake Sensitivity
        editingSensitivity = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 7) {
        // Brightness
        editingBrightness = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 8) {
        // Clock Color
        editingClockColor = true;
        currentMode = MODE_NORMAL;  // Stay in normal but editing
        M5.Display.clear();
      } else if (menuSelection == 9) {
        // Test Reality Checks
        testingRealityCheck = true;
        testRCIndex = 0;  // Start with first RC
        currentRealityCheck = 0;
        currentMode = MODE_NORMAL;  // Stay in normal but testing
        M5.Display.clear();
        Serial.println("Entering Reality Check Test mode");
      }
    }
  } else if (currentMode == MODE_SET_TIME) {
    // TIME-SETTING MODE: Button B switches between hour/minute
    if (M5.BtnB.wasPressed()) {
      editingHour = !editingHour;
      Serial.printf("Now editing: %s\n", editingHour ? "HOUR" : "MINUTE");
      // Redraw immediately to show change
      drawTimeSetUI();
    }
  } else if (editingAlarmCount) {
    // EDITING ALARMS/DAY: Button B decrements
    if (M5.BtnB.wasPressed()) {
      alarmsPerDay--;
      if (alarmsPerDay < 0) alarmsPerDay = 0;
      Serial.printf("Alarms/day: %d\n", alarmsPerDay);
      drawAlarmsPerDayUI();
    }
  } else if (editingManualAlarm) {
    // EDITING MANUAL ALARM: Button B switches between hour/minute/enabled
    if (M5.BtnB.wasPressed()) {
      if (editingMAHour) {
        editingMAHour = false;  // Switch to editing minute
        Serial.println("Now editing manual alarm: MINUTE");
      } else {
        // Toggle enabled on/off when done editing time
        manualAlarmEnabled = !manualAlarmEnabled;
        Serial.printf("Manual alarm %s\n", manualAlarmEnabled ? "ENABLED" : "DISABLED");
        editingMAHour = true;  // Reset for next time
      }
      drawManualAlarmUI();
    }
  } else if (editingScreenTimeout) {
    // EDITING SCREEN TIMEOUT: Button B decrements (reverse order)
    if (M5.BtnB.wasPressed()) {
      if (screenTimeoutSeconds == 5) {
        screenTimeoutSeconds = 0;  // 5 seconds -> Always On
      } else if (screenTimeoutSeconds <= 60) {
        screenTimeoutSeconds -= 5;  // 10-60 seconds in 5 sec steps
        if (screenTimeoutSeconds < 5) screenTimeoutSeconds = 5;
      } else if (screenTimeoutSeconds == 120) {
        screenTimeoutSeconds = 60;  // 2 min -> 60 sec
      } else if (screenTimeoutSeconds == 180) {
        screenTimeoutSeconds = 120;  // 3 min -> 2 min
      } else if (screenTimeoutSeconds == 240) {
        screenTimeoutSeconds = 180;  // 4 min -> 3 min
      } else if (screenTimeoutSeconds == 300) {
        screenTimeoutSeconds = 240;  // 5 min -> 4 min
      } else if (screenTimeoutSeconds == 0) {
        screenTimeoutSeconds = 300;  // Always On -> 5 min
      }
      
      if (screenTimeoutSeconds == 0) {
        Serial.println("Screen timeout: Always On");
      } else {
        Serial.printf("Screen timeout: %d sec\n", screenTimeoutSeconds);
      }
      drawScreenTimeoutUI();
    }
  } else if (editingSensitivity) {
    // EDITING SENSITIVITY: Button B decrements level
    if (M5.BtnB.wasPressed()) {
      sensitivityLevel--;
      if (sensitivityLevel < 0) sensitivityLevel = 0;  // Min: Light Tap
      if (sensitivityLevel == 6) {
        Serial.println("Sensitivity: Button Only (shake-to-wake DISABLED)");
      } else {
        Serial.printf("Sensitivity: %s (%.2f)\n", SENSITIVITY_NAMES[sensitivityLevel], SENSITIVITY_VALUES[sensitivityLevel]);
      }
      drawSensitivityUI();
    }
  } else if (editingBrightness) {
    // EDITING BRIGHTNESS: Button B decrements level
    if (M5.BtnB.wasPressed()) {
      brightnessLevel--;
      if (brightnessLevel < 0) brightnessLevel = 0;  // Min: 0%
      M5.Display.setBrightness(BRIGHTNESS_VALUES[brightnessLevel]);
      Serial.printf("Brightness: %d%% (PWM: %d)\n", (brightnessLevel * 10), BRIGHTNESS_VALUES[brightnessLevel]);
      drawBrightnessUI();
    }
  } else if (editingClockColor) {
    // EDITING CLOCK COLOR: Button B cycles to previous color
    if (M5.BtnB.wasPressed()) {
      clockColorIndex--;
      if (clockColorIndex < 0) clockColorIndex = NUM_COLORS - 1;
      Serial.printf("Clock color: %s\n", COLOR_NAMES[clockColorIndex]);
      drawClockColorUI();
    }
  } else if (editingQuietHours) {
    // EDITING QUIET HOURS: Button B switches between start/end OR decrements
    if (M5.BtnB.wasPressed()) {
      if (editingQHStart) {
        quietHoursStart--;
        if (quietHoursStart < 0) quietHoursStart = 23;
        Serial.printf("Quiet hours start: %02d:00\n", quietHoursStart);
      } else {
        quietHoursEnd--;
        if (quietHoursEnd < 0) quietHoursEnd = 23;
        Serial.printf("Quiet hours end: %02d:00\n", quietHoursEnd);
      }
      drawQuietHoursUI();
    }
  } else if (editingTimeFormat) {
    // EDITING TIME FORMAT: Button B toggles (same as A)
    if (M5.BtnB.wasPressed()) {
      use24HourFormat = !use24HourFormat;
      Serial.printf("Time format: %s\n", use24HourFormat ? "24 Hour" : "12 Hour");
      drawTimeFormatUI();
    }
  } else if (testingRealityCheck) {
    // TESTING REALITY CHECKS: Button B cycles to previous RC
    if (M5.BtnB.wasPressed()) {
      testRCIndex--;
      if (testRCIndex < 0) testRCIndex = 8;  // Wrap to last RC (0-8 = 9 checks)
      currentRealityCheck = testRCIndex;
      Serial.printf("Testing RC #%d\n", testRCIndex);
      drawRealityCheckUI();
    }
  } else if (currentMode == MODE_REALITY_CHECK) {
    // REALITY CHECK MODE: Button B dismisses
    if (M5.BtnB.wasPressed()) {
      Serial.println("Reality check dismissed");
      stopBuzzer();
      alarmActive = false;
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      scheduleNextAlarm();
    }
  } else if (currentMode == MODE_DREAM_JOURNAL) {
    // DREAM JOURNAL MODE: Any button dismisses
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnPWR.wasPressed()) {
      Serial.println("Dream journal alarm dismissed");
      alarmActive = false;
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      lastDreamJournalBeep = 0;
    }
  }

  // Power Button - ONLY for saving/exiting modes (NOT for entering - causes power off)
  
  // Exit Night Mode with PWR button
  if (currentMode == MODE_NIGHT && M5.BtnPWR.wasPressed()) {
    Serial.println("Exiting Night Mode");
    nightModeActive = false;
    currentMode = MODE_NORMAL;
    M5.Display.clear();
  }
  
  // Exit light switch test with PWR button
  if (lightSwitchTime > 0 && M5.BtnPWR.wasPressed()) {
    Serial.println("Exiting light switch test");
    lightSwitchTime = 0;
    M5.Display.clear();
  }
  
  if (currentMode == MODE_MENU) {
    // MENU MODE: PWR exits back to clock
    if (M5.BtnPWR.wasPressed()) {
      Serial.println("Exiting menu");
      currentMode = MODE_NORMAL;
      M5.Display.clear();
    }
  } else if (currentMode == MODE_SET_TIME) {
    // TIME-SETTING MODE: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Saving time: %02d:%02d\n", editHour, editMinute);
      // Save to RTC
      auto t = M5.Rtc.getTime();
      t.hours = editHour;
      t.minutes = editMinute;
      t.seconds = 0;
      M5.Rtc.setTime(&t);
      
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("TIME SAVED - Returning to normal mode");
    }
  } else if (editingAlarmCount) {
    // EDITING ALARMS/DAY: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Alarms/day saved: %d\n", alarmsPerDay);
      saveSettings();
      editingAlarmCount = false;
      // Reschedule next alarm with new settings
      scheduleNextAlarm();
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("SETTINGS SAVED - Returning to normal mode");
    }
  } else if (editingManualAlarm) {
    // EDITING MANUAL ALARM: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Manual alarm saved: %02d:%02d (%s)\n", manualAlarmHour, manualAlarmMinute, manualAlarmEnabled ? "ON" : "OFF");
      saveSettings();
      editingManualAlarm = false;
      manualAlarmTriggered = false;  // Reset trigger flag
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("MANUAL ALARM SAVED - Returning to normal mode");
    }
  } else if (editingScreenTimeout) {
    // EDITING SCREEN TIMEOUT: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Screen timeout saved: %d sec\n", screenTimeoutSeconds);
      saveSettings();
      editingScreenTimeout = false;
      lastActivityTime = millis();  // Reset timeout
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("SCREEN TIMEOUT SAVED - Returning to normal mode");
    }
  } else if (editingSensitivity) {
    // EDITING SENSITIVITY: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      if (sensitivityLevel == 6) {
        Serial.println("Sensitivity saved: Button Only (shake-to-wake DISABLED)");
      } else {
        Serial.printf("Sensitivity saved: %s (%.2f)\n", SENSITIVITY_NAMES[sensitivityLevel], SENSITIVITY_VALUES[sensitivityLevel]);
      }
      saveSettings();
      editingSensitivity = false;
      lastActivityTime = millis();  // Reset timeout
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("SENSITIVITY SAVED - Returning to normal mode");
    }
  } else if (editingBrightness) {
    // EDITING BRIGHTNESS: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Brightness saved: %d%% (PWM: %d)\n", (brightnessLevel * 10), BRIGHTNESS_VALUES[brightnessLevel]);
      saveSettings();
      editingBrightness = false;
      lastActivityTime = millis();  // Reset timeout
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("BRIGHTNESS SAVED - Returning to normal mode");
    }
  } else if (editingClockColor) {
    // EDITING CLOCK COLOR: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Clock color saved: %s\n", COLOR_NAMES[clockColorIndex]);
      saveSettings();
      editingClockColor = false;
      lastActivityTime = millis();  // Reset timeout
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("CLOCK COLOR SAVED - Returning to normal mode");
    }
  } else if (editingQuietHours) {
    // EDITING QUIET HOURS: PWR switches between start/end, or exits if both done
    if (M5.BtnPWR.wasPressed()) {
      if (editingQHStart) {
        editingQHStart = false;  // Switch to editing end time
        Serial.println("Now editing quiet hours: END");
        drawQuietHoursUI();
      } else {
        // Done editing, save and exit
        Serial.printf("Quiet hours saved: %02d:00 - %02d:00\n", quietHoursStart, quietHoursEnd);
        saveSettings();
        editingQuietHours = false;
        editingQHStart = true;  // Reset for next time
        lastActivityTime = millis();  // Reset timeout
        currentMode = MODE_NORMAL;
        M5.Display.clear();
        Serial.println("QUIET HOURS SAVED - Returning to normal mode");
      }
    }
  } else if (editingTimeFormat) {
    // EDITING TIME FORMAT: PWR saves and exits
    if (M5.BtnPWR.wasPressed()) {
      Serial.printf("Time format saved: %s\n", use24HourFormat ? "24 Hour" : "12 Hour");
      saveSettings();
      editingTimeFormat = false;
      lastActivityTime = millis();  // Reset timeout
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
      Serial.println("TIME FORMAT SAVED - Returning to normal mode");
    }
  } else if (testingRealityCheck) {
    // TESTING REALITY CHECKS: PWR exits test mode
    if (M5.BtnPWR.wasPressed()) {
      Serial.println("Exiting Reality Check Test mode");
      testingRealityCheck = false;
      lastActivityTime = millis();  // Reset timeout
      // Return to normal mode
      currentMode = MODE_NORMAL;
      M5.Display.clear();
    }
  }

  // Short idle so loop isn't CPU bird-dogging, but non-blocking
  delay(10);
}

// Buzzer control functions
void startBuzzer() {
  buzzerActive = true;
  buzzerChirpNumber = 0;
  buzzerLastChirp = 0;
  buzzerToneOn = false;
  Serial.println("BUZZER: Starting chirp sequence");
}

void stopBuzzer() {
  ledcWrite(BUZZER_CHANNEL, 0);
  buzzerActive = false;
  buzzerToneOn = false;
  Serial.println("BUZZER: Stopped");
}

void updateBuzzer() {
  if (!buzzerActive) return;
  
  unsigned long now = millis();
  
  // If tone is currently on, check if it's time to turn off
  if (buzzerToneOn) {
    if (now - buzzerToneStart >= BUZZER_CHIRP_DURATION) {
      ledcWrite(BUZZER_CHANNEL, 0);  // Turn off
      buzzerToneOn = false;
      buzzerLastChirp = now;
      buzzerChirpNumber++;
      Serial.printf("BUZZER: Chirp %d/%d complete\n", buzzerChirpNumber, BUZZER_CHIRP_COUNT);
      
      // If all chirps done, stop
      if (buzzerChirpNumber >= BUZZER_CHIRP_COUNT) {
        stopBuzzer();
      }
    }
  }
  // If tone is off and we need more chirps, wait for interval then play next
  else if (buzzerChirpNumber < BUZZER_CHIRP_COUNT) {
    if (buzzerChirpNumber == 0 || (now - buzzerLastChirp >= BUZZER_CHIRP_INTERVAL)) {
      ledcWrite(BUZZER_CHANNEL, 128);  // Turn on (50% duty cycle)
      buzzerToneOn = true;
      buzzerToneStart = now;
    }
  }
}

// Draw time-setting UI
void drawTimeSetUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(10, 5);
  M5.Display.println("SET TIME");
  
  // Draw large time with highlighting
  M5.Display.setTextSize(3);
  M5.Display.setCursor(15, 40);
  
  // Hour (green if editing)
  if (editingHour) {
    M5.Display.setTextColor(GREEN);
  } else {
    M5.Display.setTextColor(WHITE);
  }
  M5.Display.printf("%02d", editHour);
  
  M5.Display.setTextColor(WHITE);
  M5.Display.print(":");
  
  // Minute (green if editing)
  if (!editingHour) {
    M5.Display.setTextColor(GREEN);
  } else {
    M5.Display.setTextColor(WHITE);
  }
  M5.Display.printf("%02d", editMinute);
  
  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 100);
  M5.Display.println("A:+  B:Switch");
  M5.Display.setCursor(5, 115);
  M5.Display.println("PWR:Save&Exit");
}

// Get current time in minutes since midnight
unsigned long getCurrentMinutes(int hh, int mm) {
  return (hh * 60) + mm;
}

// Check if current hour is in quiet hours
bool isQuietHours(int hour) {
  if (quietHoursStart < quietHoursEnd) {
    // Normal range: e.g., 23-7 wraps midnight
    return false;  // Not implemented for now
  } else {
    // Wraps midnight: e.g., 23:00 to 07:00
    return (hour >= quietHoursStart || hour < quietHoursEnd);
  }
}

// Schedule next random alarm
void scheduleNextAlarm() {
  auto dt = M5.Rtc.getDateTime();
  int currentMinutes = getCurrentMinutes(dt.time.hours, dt.time.minutes);
  
  // Calculate random interval based on alarmsPerDay
  // Awake hours: assume 16 hours (7 AM to 11 PM = 960 minutes)
  // Divide awake time by number of alarms per day
  int awakeMinutes = 960;  // 16 hours
  int avgInterval = awakeMinutes / alarmsPerDay;
  
  // Add randomness: ±30% of average interval
  int randomness = avgInterval * 0.3;
  int minInterval = avgInterval - randomness;
  int maxInterval = avgInterval + randomness;
  
  // Random interval between min and max
  int interval = random(minInterval, maxInterval + 1);
  
  nextAlarmMinute = currentMinutes + interval;
  
  // Wrap around at midnight (1440 minutes per day)
  if (nextAlarmMinute >= 1440) {
    nextAlarmMinute -= 1440;
  }
  
  Serial.printf("Next alarm in ~%d minutes (avg interval: %d min for %d alarms/day)\n", 
                interval, avgInterval, alarmsPerDay);
}

// Draw reality check screen
void drawRealityCheckUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(10, 5);
  M5.Display.println("REALITY");
  M5.Display.setCursor(20, 25);
  M5.Display.println("CHECK!");
  
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 50);
  M5.Display.println("Are you");
  M5.Display.setCursor(5, 68);
  M5.Display.println("dreaming?");
  
  // Reality check instructions - larger font, use full screen
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(CYAN);
  
  // Rotate through 9 different reality checks
  switch(currentRealityCheck % 9) {
    case 0:  // Finger through palm
      M5.Display.setCursor(5, 92);
      M5.Display.println("Push finger");
      M5.Display.setCursor(5, 110);
      M5.Display.println("through palm");
      break;
    case 1:  // Text stability
      M5.Display.setCursor(5, 92);
      M5.Display.println("Read text,");
      M5.Display.setCursor(5, 110);
      M5.Display.println("look & reread");
      break;
    case 2:  // Nose breathing
      M5.Display.setCursor(5, 92);
      M5.Display.println("Pinch nose");
      M5.Display.setCursor(5, 110);
      M5.Display.println("Can breathe?");
      break;
    case 3:  // Digital watch - SHOW LIVE CLOCK
      {
        // Get current time
        auto dt = M5.Rtc.getDateTime();
        char timeBuf[16];
        if (use24HourFormat) {
          snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", dt.time.hours, dt.time.minutes, dt.time.seconds);
        } else {
          int displayHour = dt.time.hours % 12;
          if (displayHour == 0) displayHour = 12;
          snprintf(timeBuf, sizeof(timeBuf), "%2d:%02d:%02d", displayHour, dt.time.minutes, dt.time.seconds);
        }
        
        // Display live clock
        M5.Display.setTextSize(3);
        M5.Display.setCursor(20, 95);
        M5.Display.println(timeBuf);
      }
      break;
    case 4:  // Mirror test
      M5.Display.setCursor(5, 92);
      M5.Display.println("Touch mirror");
      M5.Display.setCursor(5, 110);
      M5.Display.println("Hand go thru?");
      break;
    case 5:  // Jump/physics test
      M5.Display.setCursor(5, 92);
      M5.Display.println("Jump or push");
      M5.Display.setCursor(5, 110);
      M5.Display.println("Physics OK?");
      break;
    case 6:  // Light switch test
      M5.Display.setCursor(5, 92);
      M5.Display.println("Flip switch");
      M5.Display.setCursor(5, 110);
      M5.Display.println("Does it work?");
      break;
    case 7:  // Hand count test
      M5.Display.setCursor(5, 92);
      M5.Display.println("Count your");
      M5.Display.setCursor(5, 110);
      M5.Display.println("fingers");
      break;
    case 8:  // Memory recall test
      M5.Display.setCursor(5, 92);
      M5.Display.println("How did you");
      M5.Display.setCursor(5, 110);
      M5.Display.println("get here?");
      break;
  }
}

// Draw menu screen (2-column layout, no instructions)
void drawMenuUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(60, 2);
  M5.Display.println("MENU");
  
  M5.Display.setTextSize(1);
  
  // Draw menu items in 2 columns
  // Column 1: items 0-4 (left side)
  // Column 2: items 5-8 (right side)
  
  for (int i = 0; i < MENU_ITEMS; i++) {
    int col = (i < 5) ? 0 : 1;  // Which column (0 = left, 1 = right)
    int row = (i < 5) ? i : (i - 5);  // Which row in that column
    
    int x = (col == 0) ? 5 : 125;  // X position
    int y = 25 + (row * 22);  // Y position - tighter spacing (was 30 + row*25)
    
    if (i == menuSelection) {
      // Selected item - green with arrow
      M5.Display.setTextColor(GREEN);
      M5.Display.setCursor(x, y);
      M5.Display.print(">");
      M5.Display.setCursor(x + 10, y);
      M5.Display.println(menuItems[i]);
    } else {
      // Unselected item - white
      M5.Display.setTextColor(WHITE);
      M5.Display.setCursor(x + 10, y);
      M5.Display.println(menuItems[i]);
    }
  }
}

// Draw alarms per day editing screen
void drawAlarmsPerDayUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(10, 5);
  M5.Display.println("ALARMS/DAY");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 35);
  M5.Display.println("Reality checks:");
  
  // Show current value in large green
  M5.Display.setTextSize(4);
  M5.Display.setTextColor(GREEN);
  M5.Display.setCursor(60, 55);
  M5.Display.printf("%d", alarmsPerDay);
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 100);
  M5.Display.println("A:+  B:-  (0-20)");
  M5.Display.setCursor(5, 115);
  M5.Display.println("PWR:Save");
}

// Draw manual alarm editing screen
void drawManualAlarmUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(5, 5);
  M5.Display.println("MORNING");
  M5.Display.setCursor(5, 20);
  M5.Display.println("ALARM");
  
  // Show ON/OFF status
  M5.Display.setTextSize(2);
  M5.Display.setCursor(10, 40);
  if (manualAlarmEnabled) {
    M5.Display.setTextColor(GREEN);
    M5.Display.println("ENABLED");
  } else {
    M5.Display.setTextColor(RED);
    M5.Display.println("DISABLED");
  }
  
  // Draw large time with highlighting
  M5.Display.setTextSize(3);
  M5.Display.setCursor(15, 50);
  
  // Hour (green if editing)
  if (editingMAHour) {
    M5.Display.setTextColor(GREEN);
  } else {
    M5.Display.setTextColor(WHITE);
  }
  M5.Display.printf("%02d", manualAlarmHour);
  
  M5.Display.setTextColor(WHITE);
  M5.Display.print(":");
  
  // Minute (green if editing)
  if (!editingMAHour) {
    M5.Display.setTextColor(GREEN);
  } else {
    M5.Display.setTextColor(WHITE);
  }
  M5.Display.printf("%02d", manualAlarmMinute);
  
  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 95);
  M5.Display.println("A:+  B:Switch/Toggle");
  M5.Display.setCursor(5, 110);
  M5.Display.println("PWR:Save");
}

// Check IMU for activity (shake detection to wake screen)
void checkIMUActivity() {
  // Skip IMU checks if "Button Only" mode selected (level 6)
  if (sensitivityLevel == 6) {
    return;  // IMU disabled - button-only wake
  }
  
  static unsigned long lastIMUCheck = 0;
  unsigned long now = millis();
  
  // Check IMU every 100ms
  if (now - lastIMUCheck < 100) {
    return;
  }
  lastIMUCheck = now;
  
  if (M5.Imu.update()) {
    auto data = M5.Imu.getImuData();
    float deltaX = abs(data.accel.x - lastAccelX);
    float deltaY = abs(data.accel.y - lastAccelY);
    float deltaZ = abs(data.accel.z - lastAccelZ);
    
    // Use current sensitivity setting
    float currentThreshold = SENSITIVITY_VALUES[sensitivityLevel];
    
    // Check if significant movement detected
    if (deltaX > currentThreshold || deltaY > currentThreshold || deltaZ > currentThreshold) {
      // Movement detected - wake screen
      if (!screenOn) {
        Serial.printf("IMU: Movement detected (sensitivity: %s) - waking screen\n", SENSITIVITY_NAMES[sensitivityLevel]);
        screenOn = true;
        M5.Display.wakeup();
        M5.Display.setBrightness(BRIGHTNESS_VALUES[brightnessLevel]);
      }
      lastActivityTime = now;  // Reset timeout
    }
    
    // Update last values
    lastAccelX = data.accel.x;
    lastAccelY = data.accel.y;
    lastAccelZ = data.accel.z;
  }
}

// Update screen timeout (turn off screen after inactivity)
void updateScreenTimeout() {
  unsigned long now = millis();
  
  // Don't timeout during alarms or menu navigation
  if (currentMode == MODE_REALITY_CHECK || currentMode == MODE_MENU || 
      currentMode == MODE_SET_TIME || editingAlarmCount || editingScreenTimeout || 
      editingSensitivity || editingBrightness || editingClockColor || editingManualAlarm) {
    lastActivityTime = now;
    if (!screenOn) {
      screenOn = true;
      M5.Display.wakeup();
      M5.Display.setBrightness(BRIGHTNESS_VALUES[brightnessLevel]);
    }
    return;
  }
  
  // Skip timeout if Always On mode (0 = never timeout)
  if (screenTimeoutSeconds == 0) {
    return;
  }
  
  // Check timeout
  if (screenOn && (now - lastActivityTime > (screenTimeoutSeconds * 1000))) {
    Serial.println("Screen timeout - sleeping");
    screenOn = false;
    M5.Display.sleep();
    M5.Display.setBrightness(0);
  }
}

// Draw screen timeout editing screen
void drawScreenTimeoutUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(10, 5);
  M5.Display.println("TIMEOUT");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 35);
  M5.Display.println("Screen sleep:");
  
  // Show current value in large green
  M5.Display.setTextSize(4);
  M5.Display.setTextColor(GREEN);
  
  if (screenTimeoutSeconds == 0) {
    // Always On mode
    M5.Display.setCursor(20, 55);
    M5.Display.setTextSize(2);
    M5.Display.println("ALWAYS ON");
  } else if (screenTimeoutSeconds >= 60) {
    // Display in minutes
    M5.Display.setCursor(50, 55);
    M5.Display.printf("%d", screenTimeoutSeconds / 60);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(120, 65);
    M5.Display.println("m");
  } else {
    // Display in seconds
    M5.Display.setCursor(50, 55);
    M5.Display.printf("%d", screenTimeoutSeconds);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(120, 65);
    M5.Display.println("s");
  }
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 100);
  M5.Display.println("5s-60s, 2m-5m, Always");
  M5.Display.setCursor(5, 115);
  M5.Display.println("PWR:Save");
}

// Draw sensitivity editing screen
void drawSensitivityUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(5, 2);
  M5.Display.println("SHAKE SENSE");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 28);
  M5.Display.println("Wake sensitivity:");
  
  // Show current level name in large green
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(GREEN);
  M5.Display.setCursor(15, 48);
  M5.Display.println(SENSITIVITY_NAMES[sensitivityLevel]);
  
  // Show visual indicator (bars) - skip if Button Only mode
  if (sensitivityLevel < 6) {
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(CYAN);
    M5.Display.setCursor(5, 75);
    for (int i = 0; i <= sensitivityLevel; i++) {
      M5.Display.print("|||");
    }
  } else {
    // Button Only mode - show special indicator
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(RED);
    M5.Display.setCursor(5, 75);
    M5.Display.println("IMU DISABLED");
    M5.Display.setCursor(5, 87);
    M5.Display.println("(Button wake only)");
  }
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 105);
  M5.Display.println("A: More  B: Less");
  M5.Display.setCursor(5, 118);
  M5.Display.println("PWR: Save");
}

// Draw brightness editing screen
void drawBrightnessUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(5, 2);
  M5.Display.println("BRIGHTNESS");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 28);
  M5.Display.println("Screen brightness:");
  
  // Show current value in large green
  M5.Display.setTextSize(4);
  M5.Display.setTextColor(GREEN);
  M5.Display.setCursor(40, 48);
  M5.Display.printf("%d%%", brightnessLevel * 10);
  
  // Show visual indicator (bars)
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 80);
  for (int i = 0; i <= brightnessLevel; i++) {
    M5.Display.print("|");
  }
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(5, 95);
  M5.Display.println("A: +  B: -  (0-100%)");
  M5.Display.setCursor(5, 108);
  M5.Display.println("PWR: Save");
}

// Draw clock color editing screen
void drawClockColorUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(5, 2);
  M5.Display.println("CLOCK COLOR");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 28);
  M5.Display.println("Clock display color:");
  
  // Show current color name in that color
  M5.Display.setTextSize(3);
  M5.Display.setTextColor(CLOCK_COLORS[clockColorIndex]);
  M5.Display.setCursor(20, 52);
  M5.Display.println(COLOR_NAMES[clockColorIndex]);
  
  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 95);
  M5.Display.println("A/B: Change  PWR: Save");
}

void drawQuietHoursUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(5, 2);
  M5.Display.println("QUIET HOURS");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 28);
  M5.Display.println("No alarms between:");
  
  // Show start hour (highlight if editing)
  M5.Display.setTextSize(2);
  if (editingQHStart) {
    M5.Display.setTextColor(BLACK, YELLOW);  // Inverted
  } else {
    M5.Display.setTextColor(WHITE);
  }
  M5.Display.setCursor(30, 50);
  M5.Display.printf("%02d:00", quietHoursStart);
  
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(100, 50);
  M5.Display.print("-");
  
  // Show end hour (highlight if editing)
  if (!editingQHStart) {
    M5.Display.setTextColor(BLACK, YELLOW);  // Inverted
  } else {
    M5.Display.setTextColor(WHITE);
  }
  M5.Display.setCursor(130, 50);
  M5.Display.printf("%02d:00", quietHoursEnd);
  
  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 80);
  M5.Display.println("A/B: +/-");
  M5.Display.setCursor(5, 95);
  M5.Display.println("PWR: Switch/Save");
}

void drawTimeFormatUI() {
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(5, 2);
  M5.Display.println("TIME FORMAT");
  
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 28);
  M5.Display.println("Clock display format:");
  
  // Show current format
  M5.Display.setTextSize(3);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(40, 52);
  if (use24HourFormat) {
    M5.Display.println("24 Hour");
  } else {
    M5.Display.println("12 Hour");
  }
  
  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(5, 95);
  M5.Display.println("A/B: Toggle  PWR: Save");
}

// Load settings from NVS
void loadSettings() {
  preferences.begin("lucidwatch", false);
  
  // Load all settings with defaults
  screenTimeoutSeconds = preferences.getInt("timeout", 15);
  sensitivityLevel = preferences.getInt("sensitivity", 2);
  brightnessLevel = preferences.getInt("brightness", 8);
  clockColorIndex = preferences.getInt("clockColor", 0);
  alarmsPerDay = preferences.getInt("alarmsDay", 12);
  use24HourFormat = preferences.getBool("use24h", true);
  quietHoursStart = preferences.getInt("quietStart", 23);
  quietHoursEnd = preferences.getInt("quietEnd", 7);
  manualAlarmEnabled = preferences.getBool("manualOn", false);
  manualAlarmHour = preferences.getInt("manualHour", 7);
  manualAlarmMinute = preferences.getInt("manualMin", 0);
  
  preferences.end();
  
  Serial.println("=== SETTINGS LOADED FROM NVS ===");
  Serial.printf("Screen Timeout: %d sec\n", screenTimeoutSeconds);
  Serial.printf("Sensitivity: %s\n", SENSITIVITY_NAMES[sensitivityLevel]);
  Serial.printf("Brightness: %d%%\n", brightnessLevel * 10);
  Serial.printf("Clock Color: %s\n", COLOR_NAMES[clockColorIndex]);
  Serial.printf("Alarms/Day: %d\n", alarmsPerDay);
  Serial.printf("Time Format: %s\n", use24HourFormat ? "24h" : "12h");
  Serial.printf("Quiet Hours: %02d:00 - %02d:00\n", quietHoursStart, quietHoursEnd);
  Serial.printf("Manual Alarm: %s at %02d:%02d\n", manualAlarmEnabled ? "ON" : "OFF", manualAlarmHour, manualAlarmMinute);
}

// Save settings to NVS
void saveSettings() {
  preferences.begin("lucidwatch", false);
  
  preferences.putInt("timeout", screenTimeoutSeconds);
  preferences.putInt("sensitivity", sensitivityLevel);
  preferences.putInt("brightness", brightnessLevel);
  preferences.putInt("clockColor", clockColorIndex);
  preferences.putInt("alarmsDay", alarmsPerDay);
  preferences.putBool("use24h", use24HourFormat);
  preferences.putInt("quietStart", quietHoursStart);
  preferences.putInt("quietEnd", quietHoursEnd);
  preferences.putBool("manualOn", manualAlarmEnabled);
  preferences.putInt("manualHour", manualAlarmHour);
  preferences.putInt("manualMin", manualAlarmMinute);
  
  preferences.end();
  
  Serial.println("Settings saved to NVS");
}

// Gentle REM Beep - soft tones to trigger lucidity without waking
void gentleREMBeep() {
  Serial.println("REM Cue - Gentle beep");
  
  // 3 soft ascending tones
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(BUZZER_CHANNEL, 800 + (i * 100));  // 800Hz, 900Hz, 1000Hz
    ledcWrite(BUZZER_CHANNEL, 64);  // Very soft volume (25%)
    delay(200);
    ledcWrite(BUZZER_CHANNEL, 0);  // Off
    delay(250);
  }
}

// Check Night Mode and trigger REM cues
void checkNightMode() {
  if (!nightModeActive) return;
  
  unsigned long now = millis();
  unsigned long elapsed = now - sleepStartTime;
  
  // Start REM cues after 4.5 hours (270 minutes)
  const unsigned long FIRST_REM_WINDOW = 270UL * 60 * 1000;  // 4h 30min
  
  if (elapsed < FIRST_REM_WINDOW) {
    return;  // Too early, still in deep sleep
  }
  
  // Window 1: 4.5h - 5.75h (8 min intervals)
  // Window 2: 6h - 7.25h (7 min intervals)
  
  unsigned long cueInterval;
  if (elapsed < 345UL * 60 * 1000) {  // Before 5h 45min
    cueInterval = 8UL * 60 * 1000;  // 8 minutes
  } else {
    cueInterval = 7UL * 60 * 1000;  // 7 minutes
  }
  
  // Trigger cue if interval has passed
  if (now - lastREMCue >= cueInterval) {
    gentleREMBeep();
    lastREMCue = now;
  }
}

// Draw Night Mode UI
void drawNightModeUI() {
  M5.Display.fillScreen(BLACK);
  
  unsigned long elapsed = millis() - sleepStartTime;
  unsigned long hours = elapsed / (60UL * 60 * 1000);
  unsigned long minutes = (elapsed / (60UL * 1000)) % 60;
  
  // Title
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(BLUE);
  M5.Display.setCursor(20, 10);
  M5.Display.println("NIGHT MODE");
  
  // Moon emoji substitute
  M5.Display.setTextSize(4);
  M5.Display.setCursor(80, 35);
  M5.Display.println("Z");
  M5.Display.setTextSize(3);
  M5.Display.setCursor(70, 45);
  M5.Display.println("Z");
  M5.Display.setTextSize(2);
  M5.Display.setCursor(60, 52);
  M5.Display.println("Z");
  
  // Time elapsed
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(30, 80);
  M5.Display.printf("%02luh %02lum", hours, minutes);
  
  // Status
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(GREEN);
  M5.Display.setCursor(15, 105);
  if (elapsed < 270UL * 60 * 1000) {
    M5.Display.println("Deep Sleep Phase");
  } else {
    M5.Display.println("REM Cue Active");
  }
  
  // Exit instruction
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(25, 120);
  M5.Display.println("PWR: Exit & Wake");
}

// Draw Dream Journal UI
void drawDreamJournalUI() {
  M5.Display.fillScreen(BLACK);
  
  // Title
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(YELLOW);
  M5.Display.setCursor(25, 10);
  M5.Display.println("GOOD");
  M5.Display.setCursor(10, 30);
  M5.Display.println("MORNING!");
  
  // Main prompt
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(20, 60);
  M5.Display.println("What did");
  M5.Display.setCursor(5, 80);
  M5.Display.println("I dream");
  M5.Display.setCursor(5, 100);
  M5.Display.println("last night?");
  
  // Instruction
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(10, 125);
  M5.Display.println("Press any button");
}
