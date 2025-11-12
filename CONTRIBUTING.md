# Contributing to LucidWatch

Thank you for your interest in contributing to LucidWatch! 🌙

## How to Contribute

### Reporting Bugs
1. Check if the issue already exists
2. Create a new issue with:
   - Clear description of the problem
   - Steps to reproduce
   - Expected vs actual behavior
   - Device version and firmware version
   - Serial monitor output if available

### Suggesting Features
1. Check existing feature requests
2. Open an issue with:
   - Clear description of the feature
   - Use case and benefits
   - Any implementation ideas

### Code Contributions
1. Fork the repository
2. Create a new branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Test thoroughly on actual hardware
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## Development Setup

### Requirements:
- PlatformIO (CLI or VSCode extension)
- M5StickC Plus2 device
- USB-C cable

### Build and Test:
```bash
# Clone your fork
git clone https://github.com/yourusername/LucidWatch.git
cd LucidWatch

# Build
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

## Code Style

- Use clear, descriptive variable names
- Comment complex logic
- Keep functions focused and small
- Test on actual hardware before submitting

## Areas We Need Help With

- **Additional Reality Checks**: New creative reality check methods
- **Battery Optimization**: Power-saving improvements
- **Translations**: Multi-language support
- **Documentation**: Tutorials, guides, examples
- **Testing**: Bug reports and edge case testing
- **REM Algorithms**: Alternative sleep cycle timing methods

## Testing Your Changes

Please test on actual M5StickC Plus2 hardware:
1. Fresh firmware flash
2. Test all menu options
3. Test reality checks
4. Test Night Mode (at least partially)
5. Test morning alarm
6. Verify settings persist after power cycle
7. Check battery life impact

## Pull Request Process

1. Update README.md if adding features
2. Update CHANGELOG if applicable
3. Ensure code compiles without errors
4. Test on hardware
5. Describe your changes clearly in the PR

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help newcomers learn
- Keep discussions on-topic

## Questions?

Open an issue or start a discussion - we're happy to help!

---

Thank you for helping make LucidWatch better! 🌙✨
