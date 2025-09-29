# Digital Alarm Clock Using RTC

This project implements a digital alarm clock using the Real-Time Clock (RTC) on an AVR128DB48 microcontroller. The clock provides UART command interface for setting time and alarm, and uses LED indicators for alarm notifications.

## Features

- **Real-Time Clock**: Maintains accurate time using the internal RTC with external 32kHz crystal
- **UART Interface**: Command-line interface for user interaction
- **Alarm System**: Set alarm times and get LED notifications when triggered
- **Periodic Status**: Automatically displays current time and alarm status every 30 seconds

## Hardware Requirements

- AVR128DB48 microcontroller
- External 32.768 kHz crystal for RTC
- 8 LEDs connected to PORTD (PD0-PD7) for alarm indication
- UART connection (USART3) for command interface

## Commands

The system accepts the following UART commands:

### SET HH:MM:SS
Set the current time in 24-hour format.

**Example:**
```
SET 12:30:45
```

### ALARM HH:MM:SS
Set the alarm time in 24-hour format.

**Example:**
```
ALARM 12:35:00
```

### SHOW
Display the current time and alarm status.

**Example output:**
```
Current Time: 12:34:56
Alarm Set: 12:35:00
Status: Waiting
```

### STOP
Stop the currently triggered alarm and turn off LED indicators.

### DEBUG
Display RTC debug information including counter values, configuration registers, and interrupt statistics.

### HELP
Display available commands and their usage.

## Operation

1. **Initialization**: The system starts with time 00:00:00 and no alarm set
2. **Time Setting**: Use the `SET` command to set the current time
3. **Alarm Setting**: Use the `ALARM` command to set when you want the alarm to trigger
4. **Monitoring**: The system will automatically display status updates every 30 seconds
5. **Alarm Trigger**: When the current time matches the alarm time, all LEDs will start blinking
6. **Alarm Stop**: Use the `STOP` command to turn off the alarm

## Technical Details

### RTC Configuration
- Uses external 32.768 kHz crystal (XOSC32K)
- Prescaler set to DIV32 for 1024 Hz tick rate
- Interrupt every 32 ticks = 1 second
- Maintains hours:minutes:seconds format

### UART Configuration
- USART3 at 9600 baud, 8N1
- Interrupt-driven transmission and reception
- Command parsing with echo and basic line editing

### LED Indication
- 8 LEDs on PORTD (PD0-PD7)
- All LEDs blink simultaneously when alarm triggers
- Blink rate: 2 Hz (500ms on/off cycle)

### Memory Usage
- Circular buffers for UART TX/RX (64 bytes each)
- Command buffer for parsing (32 bytes)
- Global time structures for current time and alarm time

## Building and Programming

### Build
```bash
make clean
make
```

### Program to MCU
```bash
make program
```

## Example Usage Session

```
=== Digital Alarm Clock ===
RTC-based alarm clock initialized
Available commands:
  SET HH:MM:SS   - Set current time
  ALARM HH:MM:SS - Set alarm time
  SHOW           - Display current time and alarm
  STOP           - Stop current alarm
  HELP           - Show help

> SET 09:15:30
Time set to 09:15:30

> ALARM 09:16:00
Alarm set to 09:16:00

> SHOW
Current Time: 09:15:45
Alarm Set: 09:16:00

--- Status Update ---
Current Time: 09:16:00
Alarm Set: 09:16:00 [TRIGGERED!]
> STOP
Alarm stopped
```

## Code Structure

- `main.c`: Main application logic, RTC and UART handling
- `include/uart.h/c`: UART driver with interrupt support
- `include/circularbuff.h/c`: Circular buffer implementation for UART
- `Makefile`: Build configuration for AVR toolchain

## Interrupts Used

- `RTC_CNT_vect`: RTC overflow interrupt for timekeeping
- `USART3_RXC_vect`: UART receive complete interrupt
- `USART3_DRE_vect`: UART data register empty interrupt

## Future Enhancements

- Add snooze functionality
- Multiple alarm support
- Time zone adjustment
- Battery backup for time retention
- LCD display for standalone operation
- Sound/buzzer alarm in addition to LEDs