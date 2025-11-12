# Hardware Guide - M5StickC Plus2

## Device Specifications

### M5StickC Plus2
- **Processor**: ESP32-S3
- **Display**: 1.14" TFT LCD (135x240)
- **IMU**: MPU6886 (accelerometer + gyroscope)
- **RTC**: BM8563 (real-time clock)
- **Battery**: 200mAh lithium battery
- **Buttons**: 2 programmable buttons + 1 power button
- **Buzzer**: PWM-capable speaker
- **Connectivity**: USB-C, WiFi, Bluetooth

## LucidWatch Hardware Usage

### Buttons:
- **Button A** (Red/Side): Light switch test, Night mode (hold)
- **Button B** (Front/Big): Menu access (hold)
- **Button PWR** (Power): Exit modes, power on/off

### Display:
- **Clock Screen**: Main interface, time display
- **Menu Screens**: Settings configuration
- **Reality Check Screens**: Various RC prompts
- **Night Mode Screen**: Sleep tracker display

### IMU (MPU6886):
- **Shake Detection**: Wake from sleep (configurable)
- **Sensitivity Levels**: 6 levels from very light to very hard
- **Button Only Mode**: IMU disabled for battery saving

### RTC (BM8563):
- **Time Keeping**: Maintains accurate time
- **Alarm Scheduling**: Triggers reality checks
- **Battery Backed**: Keeps time when powered off

### Buzzer:
- **Reality Check Alarms**: Distinctive beep patterns
- **REM Cues**: Gentle 3-tone sequence (800-900-1000Hz)
- **Morning Alarm**: Repeating gentle beeps
- **Volume Control**: PWM modulation (25-100%)

### Battery:
- **Capacity**: 200mAh
- **Expected Life**:
  - Normal use: 12-24 hours
  - Optimized (button only, 15s timeout): 24-48 hours
  - Always-on: Use plugged in
- **Charging**: USB-C, ~1 hour to full

## Power Management

### Sleep Modes:
- **Screen Off**: Display sleeps after timeout
- **IMU Wake**: Shake to wake (if enabled)
- **Button Wake**: Press Button A to wake
- **Deep Sleep**: Not used (RTC alarms require awake)

### Battery Tips:
1. **Use Button Only Wake**: Biggest battery saver
2. **15-30s Screen Timeout**: Balance usability and battery
3. **Lower Brightness**: 40-60% sufficient for most use
4. **Reduce Alarms**: Fewer alarms = less wake events
5. **Always-On**: Only use when plugged in

## Mounting Options

### Wrist Wear:
- Use included strap or third-party watch band
- Wear snug but comfortable
- Position for easy button access

### Bedside Use:
- Place on nightstand for Night Mode
- Ensure buzzer not muffled
- Position for easy dismissal

### Charging:
- Standard USB-C cable
- Any 5V USB power source
- Can use while charging

## Hardware Limitations

### What to Expect:
- **Screen Size**: Small but readable (1.14")
- **Battery Life**: Not a week-long smart watch
- **IMU Sensitivity**: May need adjustment for your use
- **Buzzer Volume**: Loud enough for alarms, not music
- **Water Resistance**: Not waterproof (avoid moisture)

### Not Recommended:
- Swimming or showering with device
- Extreme temperatures
- Dropping or impacts
- Continuous always-on use without charging

## Technical Details

### Memory:
- **Flash**: 8MB (firmware + NVS settings)
- **PSRAM**: 8MB (display buffer, variables)
- **NVS**: Settings stored in non-volatile storage

### Pins Used:
- Display, IMU, RTC, Buzzer all via I2C/SPI
- No exposed GPIO needed for LucidWatch
- All hardware handled by M5 libraries

### Firmware Size:
- Bootloader: ~17KB @ 0x1000
- Partitions: ~3KB @ 0x8000
- App: ~441KB @ 0x10000
- Total: ~461KB (plenty of space remaining)

---

For hardware troubleshooting, see the main README troubleshooting section.
