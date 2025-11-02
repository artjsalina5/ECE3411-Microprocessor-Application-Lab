# Lab Test 2 - Countdown Timer System User Guide

## Overview
This system implements a countdown timer that replaces the alarm functionality. The countdown displays the remaining time in binary on LEDs and provides UART control commands.

## Countdown Features

### Starting a Countdown
- **Button B5**: Press the external button connected to pin B5 to start a countdown
- The countdown will start from the current minutes:seconds and count down to 00:00
- Upon button press, the system displays: "Countdown Started! Starting countdown from: MM:SS"

### Countdown Display
The countdown operates in two phases, each lasting 5 seconds:

1. **Hours Display (5 seconds)**:
   - **Onboard LED (PB3)**: ON/Active Low (indicates hours are being displayed)
   - **PORTD LEDs**: Display current hours (0-12) in binary format in LSB bits 0-3
   - Format: `0bXXXX0000` where XXXX represents hours 0-12 in binary (12-hour format)

2. **Minutes Display (5 seconds)**:
   - **Onboard LED (PB3)**: OFF/Inactive (indicates minutes are being displayed)
   - **PORTD LEDs**: Display current minutes (0-59) in binary format across full PORTD bits 0-7
   - Format: `0bXXXXXXXX` where all 8 bits represent minutes 0-59 in binary

### Countdown Completion
When the countdown reaches 00:00:
- All PORTD LEDs blink at 10Hz for exactly 5 seconds
- After 5 seconds, all LEDs turn off
- Countdown holds at 00:00 and stops

## UART Control Commands

### Single Character Commands (immediate response)
- **'p' or 'P'**: Pause the countdown (only works if countdown > 00:00)
  - Response: "Countdown Paused at MM:SS"
- **'r' or 'R'**: Resume the countdown (only works if paused and countdown > 00:00)
  - Response: "Countdown Resumed at MM:SS"

### Full Commands (require ENTER)
- **COUNTDOWN**: Display current countdown status
  - Shows current countdown time
  - Indicates if paused
  - Provides usage instructions

## LED Binary Display Examples

### Hours Display (PB3 ON/Active Low) - 12-Hour Format
- Hour 0 (12 AM): PORTD = `0b00000000` (0000 in bits 3-0)
- Hour 1 (1 AM):  PORTD = `0b00000001` (0001 in bits 3-0)
- Hour 2 (2 AM):  PORTD = `0b00000010` (0010 in bits 3-0)
- Hour 3 (3 AM):  PORTD = `0b00000011` (0011 in bits 3-0)
- Hour 4 (4 AM):  PORTD = `0b00000100` (0100 in bits 3-0)
- Hour 11 (11 AM): PORTD = `0b00001011` (1011 in bits 3-0)
- Hour 0 (12 PM): PORTD = `0b00000000` (0000 in bits 3-0, cycles every 12 hours)

### Minutes Display (PB3 OFF) - Full 8-Bit Binary
- Minutes 0:  PORTD = `0b00000000` (decimal 0)
- Minutes 1:  PORTD = `0b00000001` (decimal 1)
- Minutes 15: PORTD = `0b00001111` (decimal 15)
- Minutes 30: PORTD = `0b00011110` (decimal 30)
- Minutes 45: PORTD = `0b00101101` (decimal 45)
- Minutes 59: PORTD = `0b00111011` (decimal 59)

## System Status

### Periodic Status Updates (every 2 seconds)
The system automatically displays:
```
=== AOS System Status ===
Current Time: HH:MM:SS
Countdown: MM:SS - COUNTING DOWN (press 'p' to pause)
LED Display: HOURS 0-12 (PB3 ON, LSB bits 0-3)
AOS>
```

Status variations:
- `INACTIVE - Press button B5 to start`
- `MM:SS - COUNTING DOWN (press 'p' to pause)`
- `MM:SS - PAUSED (press 'r' to resume)`
- `FINISHED (00:00) - Press B5 for new countdown`

## Operational Flow

1. **Power On**: System starts with no active countdown
2. **Set Time**: Use `SET HH:MM:SS` to set current time if needed
3. **Start Countdown**: Press button B5 to start countdown from current MM:SS
4. **Monitor Progress**: Watch LEDs cycle between hours/minutes display
5. **Control**: Use 'p'/'r' to pause/resume as needed
6. **Completion**: Wait for 10Hz blinking when countdown reaches 00:00
7. **Reset**: Press button B5 again to start a new countdown

## Technical Details

- **Clock Frequency**: 16MHz
- **UART Baud Rate**: 9600
- **Timer Resolution**: 10ms (TCA0 interrupt)
- **RTC Resolution**: 1 second
- **LED Update Rate**: 10ms
- **Countdown Blink Frequency**: 10Hz (50ms on/off)
- **Display Cycle**: 10 seconds total (5s hours + 5s minutes)

## Troubleshooting

- **No LED Response**: Check that countdown is active and not finished
- **Commands Not Working**: Ensure UART connection at 9600 baud
- **Button Not Responsive**: Check debouncing (button held for ~1 second)
- **LEDs Stuck**: Reset system or wait for display cycle to complete