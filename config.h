#ifndef CONFIG_H
#define CONFIG_H

// Battery
#define MAX_VOLTAGE 4350
#define MIN_VOLTAGE 3000

// Timing
#define TIMEZONE 0  // Adjust for your timezone
#define MILLIS_PER_SECOND 1000
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400

// Display
#define DEFAULT_BRIGHTNESS 10
#define IDLE_BRIGHTNESS 1
#define SCREEN_TIMEOUT_MS 15000
#define CLOCK_FREQUENCY 80

// Buttons
#define BTN_A 0
#define BTN_B 1
#define BTN_PWR 2

// Reality Check Settings
#define MIN_CHECKS_PER_DAY 8
#define MAX_CHECKS_PER_DAY 20
#define DEFAULT_CHECKS_PER_DAY 12
#define MIN_INTERVAL_MINUTES 20
#define MAX_INTERVAL_MINUTES 90

// Buzzer Settings (M5StickC Plus2 uses GPIO 2)
#define BUZZER_PIN 2
#define BUZZER_CHANNEL 0
#define BUZZER_FREQUENCY 2000
#define BUZZER_CHIRP_DURATION 100
#define BUZZER_CHIRP_COUNT 3
#define BUZZER_CHIRP_INTERVAL 150

// Night Quiet Hours
#define QUIET_START_HOUR 1
#define QUIET_END_HOUR 7

// Reality Check Types
#define RC_FINGER_PALM 0
#define RC_TEXT_STABILITY 1
#define RC_NOSE_BREATHING 2
#define RC_DIGITAL_CLOCK 3
#define RC_MIRROR_TEST 4
#define RC_IMU_PHYSICS 5
#define RC_LIGHT_SWITCH 6
#define RC_HAND_COUNT 7
#define RC_MEMORY_RECALL 8
#define RC_TYPE_COUNT 9

// NVS Storage Keys
#define NVS_NAMESPACE "lucid_watch"
#define NVS_CHECKS_PER_DAY "checks_day"
#define NVS_LAST_ALARM "last_alarm"
#define NVS_NEXT_ALARM "next_alarm"
#define NVS_IMU_ENABLED "imu_enabled"
#define NVS_RC_ENABLED "rc_enabled"
#define NVS_QUIET_HOURS "quiet_hours"
#define NVS_BRIGHTNESS "brightness"
#define NVS_CHECK_COUNT "check_count"

// Menu Items
#define MENU_CLOCK 0
#define MENU_SET_TIME 1
#define MENU_SETTINGS 2
#define MENU_STATS 3
#define MENU_TEST_BUZZER 4
#define MENU_COUNT 5

// Settings Menu
#define SETTING_CHECKS_PER_DAY 0
#define SETTING_IMU_TRIGGER 1
#define SETTING_QUIET_HOURS 2
#define SETTING_BRIGHTNESS 3
#define SETTING_RC_TYPES 4
#define SETTING_COUNT 5

// Colors
#define COLOR_WHITE 0xFFFF
#define COLOR_BLACK 0x0000
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F
#define COLOR_YELLOW 0xFFE0
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_ORANGE 0xFD20

// IMU Thresholds for dream detection
#define IMU_FALL_THRESHOLD 15.0f
#define IMU_ROTATION_THRESHOLD 200.0f
#define IMU_WEIRD_PHYSICS_THRESHOLD 20.0f

#endif
