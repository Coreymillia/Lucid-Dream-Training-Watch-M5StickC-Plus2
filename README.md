# LucidWatch v2.2 - Final Release

## Lucid Dream Reality Check Watch for M5StickC Plus2

### ✨ What's New in v2.2
- **✅ FIXED:** Night Mode alarm bug - alarms now properly resume after exiting Night Mode
- **✅ FIXED:** Reality check display persistence - no more flickering!
- **✅ NEW:** Fast scroll - Hold Button A in menus for rapid value changes
- **✅ IMPROVED:** Reality checks use user's screen timeout setting (not a separate timer)
- **✅ IMPROVED:** First alarm after boot triggers in 1 minute for testing

### 🎯 Core Features
- **Random Reality Checks** - Every 20-90 minutes, 9 different check types
- **Night Mode** - REM cues during sleep cycles (4.5-7.25 hours after sleep)
- **Dream Journal Alarm** - Morning wake-up reminder with gentle beeps
- **Full Customization** - Screen timeout, brightness, shake sensitivity, quiet hours, clock colors, and more
- **Power Efficient** - Screen auto-sleep, wake on shake or button press

### 📱 Reality Check Types
1. **Finger Through Palm** - Try pushing your finger through your palm
2. **Text Stability** - Read text, look away, and read again
3. **Nose Breathing** - Pinch nose - can you still breathe?
4. **Digital Clock** - Look at watch, look away, look again (shows live time)
5. **Mirror Test** - Touch a mirror - does your hand go through?
6. **Physics Test** - Jump or push - does physics work normally?
7. **Light Switch** - Flip a light switch - does it work?
8. **Hand Count** - Count your fingers
9. **Memory Recall** - How did you get here?

### 🎮 Controls

#### Normal Mode
- **Button A (Tap)**: Light switch reality check test
- **Button A (Hold 1 sec)**: Enter Night Mode
- **Button B (Hold 2 sec)**: Open Menu

#### Reality Check Active
- **Button A (1st press)**: Wake screen if sleeping
- **Button A (2nd press)**: Dismiss reality check

#### Menu Navigation
- **Button A**: Scroll down / Increment value / **HOLD for fast scroll**
- **Button B**: Scroll up / Decrement value / Switch field
- **PWR**: Save and exit

### ⚙️ Menu Options
1. **Set Time** - Set current time
2. **Morning Alarm** - Dream journal alarm (gentle beeps every 20 sec)
3. **Alarms/Day** - Not used (fixed 20-90 min random interval)
4. **Quiet Hours** - No alarms during sleep (default: 11 PM - 7 AM)
5. **12/24 Format** - Toggle time display format
6. **Screen Timeout** - 5s-60s, 2-5 minutes, or Always On
7. **Shake Sense** - Wake sensitivity (Light Tap to Button Only)
8. **Brightness** - 0-100% in 10% steps
9. **Clock Color** - 8 colors (White, Cyan, Green, Yellow, Orange, Magenta, Red, Blue)
10. **Test RC** - Preview all 9 reality checks

### 🌙 Night Mode
1. Hold Button A for 1 second to enter
2. Watch enters sleep mode for 4.5 hours (deep sleep phase)
3. After 4.5 hours, gentle REM cues play every 7-8 minutes
4. Cues continue until 7.25 hours total
5. Press PWR to exit Night Mode

### 🔋 Battery Tips
- Set screen timeout to 15-30 seconds for daily use
- Use "Button Only" shake sensitivity to disable IMU (saves power)
- Night Mode automatically uses minimal power

### 📦 Installation

#### Method 1: M5Burner (Recommended)
1. Download M5Burner from https://m5stack.com/pages/download
2. Connect M5StickC Plus2 via USB
3. Select "LucidWatch v2.1" from firmware list
4. Click "Burn" button
5. Wait for completion and enjoy!

#### Method 2: Manual Flash
```bash
esptool.py --port /dev/ttyACM0 write_flash 0x0 LucidWatch-v2.1-FINAL-MERGED.bin
```

### 🐛 Known Issues
- None! This is the stable release.

### 📝 Changelog

#### v2.1.0 (2025-11-12)
- Fixed: Night Mode alarm scheduling bug
- Fixed: Reality check display persistence
- Added: Fast scroll when holding Button A
- Improved: Reality check timeout uses user settings
- Changed: Alarm interval fixed to 20-90 minutes random

#### v2.0.0
- Initial stable release
- 9 reality check types
- Night Mode with REM cues
- Dream Journal alarm
- Full settings menu

### 💝 Support
If you enjoy LucidWatch, please star the GitHub repo and share with other lucid dreamers!

### 📄 License
MIT License - See LICENSE file

---
Made with ❤️ for lucid dreamers everywhere
