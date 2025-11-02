/**
 * @file aos_usage_guide.md
 * @author Arturo Salinas
 * @date 2025-09-30
 * @brief Complete guide to using Arturo's Operating System
 */

# 🚀 ARTURO'S OPERATING SYSTEM - USER GUIDE

## Overview
Arturo's Operating System (AOS) is a comprehensive debugging and development environment for AVR microcontrollers. It provides powerful register inspection, memory manipulation, and hardware testing capabilities through a UART interface.

## Key Features
- **Non-blocking UART communication** (no more printf blocking!)
- **Complete register inspection** for all major peripherals
- **Memory dump and modification** capabilities
- **Hardware testing functions** for GPIO, timers, and UART
- **System monitoring** with real-time status
- **Backward compatibility** with your existing RTC alarm clock

## Getting Started

### 1. Boot Message
When you start the system, you'll see:
```
╔═══════════════════════════════════════════════════════════╗
║              🚀 ARTURO'S OPERATING SYSTEM 🚀               ║
║                        BOOTED!                            ║
╠═══════════════════════════════════════════════════════════╣
║ Version: v1.0                    Build: Sep 30 2025     ║
║ MCU: AVR128DB48              Freq: 16000000 Hz     ║
║ UART3: 9600 baud             Interrupts: ENABLED        ║
╚═══════════════════════════════════════════════════════════╝

💡 Type HELP for command list, SYSINFO for system status
AOS> 
```

### 2. Basic Commands

#### System Information
```bash
AOS> SYSINFO
📊 SYSTEM INFORMATION
═══════════════════════════════════════════════════════════
MCU: AVR128DB48                Clock: 16000000 Hz
UART3 Status: 0x40           Baud: 9600
Stack Pointer: 0x3FFF         
SREG: 0x80                  Interrupts: ENABLED
Free RAM: ~1200 bytes
RTC Status: 0x01             TCA0 Status: Running
Command Buffer: 0/128 used
═══════════════════════════════════════════════════════════
```

#### Help System
```bash
AOS> HELP
🔧 ARTURO'S OPERATING SYSTEM - COMMAND REFERENCE
═══════════════════════════════════════════════════════════
  HELP                    - Show all commands
  SYSINFO                 - Show system information
  RESET                   - Software reset
  REGS [peripheral]       - Show registers (RTC, USART3, PORTD, TCA0)
  READ <address>          - Read from memory address (hex)
  WRITE <address> <value> - Write to memory address (hex)
  DUMP <start> [length]   - Memory dump (hex addresses)
  PEEK <address>          - Peek at memory location
  POKE <address> <value>  - Poke value to memory
  UART                    - Test UART functionality
  GPIO <port> <pin> <val> - Test GPIO (D,B,C pin 0-7, val 0/1)
  TIMER                   - Show timer status
  SET HH:MM:SS            - Set current time
  ALARM HH:MM:SS          - Set alarm time
  SHOW                    - Display current time and alarm
  STOP                    - Stop current alarm
═══════════════════════════════════════════════════════════
```

## 📋 Register Inspection

### View Available Peripherals
```bash
AOS> REGS
📋 AVAILABLE PERIPHERALS
═══════════════════════════════════════════════════════════
  RTC
  USART3
  PORTD
  TCA0
═══════════════════════════════════════════════════════════
```

### Inspect Specific Peripheral
```bash
AOS> REGS RTC
📊 RTC REGISTERS
═══════════════════════════════════════════════════════════
CTRLA        @ 0x1400 = 0x81  (Control A)
STATUS       @ 0x1401 = 0x01  (Status)
INTCTRL      @ 0x1402 = 0x01  (Interrupt Control)
INTFLAGS     @ 0x1403 = 0x00  (Interrupt Flags)
TEMP         @ 0x1404 = 0x00  (Temporary)
DBGCTRL      @ 0x1405 = 0x00  (Debug Control)
CLKSEL       @ 0x1406 = 0x00  (Clock Select)
CNTL         @ 0x1408 = 0x2F  (Counter Low)
CNTH         @ 0x1409 = 0x00  (Counter High)
PERL         @ 0x140A = 0xFF  (Period Low)
PERH         @ 0x140B = 0x7F  (Period High)
CMPL         @ 0x140C = 0x00  (Compare Low)
CMPH         @ 0x140D = 0x00  (Compare High)
═══════════════════════════════════════════════════════════
```

## 🔍 Memory Operations

### Read Single Memory Location
```bash
AOS> READ 0x1400
📖 Memory Read: 0x1400 = 0x81 (129)
Binary: 10000001
```

### Write to Memory
```bash
AOS> WRITE 0x1400 0x80
✏️  Memory Write: 0x1400
   Old: 0x81 (129)
   New: 0x80 (128)
```

### Memory Dump
```bash
AOS> DUMP 0x1400 16
🔍 Memory Dump: 0x1400 (16 bytes)
═══════════════════════════════════════════════════════════
1400: 80 01 01 00 00 00 00 00  ........
1408: 2F 00 FF 7F 00 00 00 00  /.......
═══════════════════════════════════════════════════════════
```

### Quick Peek/Poke (aliases)
```bash
AOS> PEEK 0x1400    # Same as READ
AOS> POKE 0x1400 0xFF    # Same as WRITE
```

## ⚡ Hardware Testing

### GPIO Testing
```bash
AOS> GPIO D 3 1
✅ PORTD pin 3 set to HIGH
   DIR: 0x08  OUT: 0x08  IN: 0x08

AOS> GPIO D 3 0  
✅ PORTD pin 3 set to LOW
   DIR: 0x08  OUT: 0x00  IN: 0x00
```

### UART Diagnostics
```bash
AOS> UART
📡 UART3 DIAGNOSTIC TEST
═══════════════════════════════════════════════════════════
USART3.STATUS: 0x40
USART3.CTRLA:  0x20
USART3.CTRLB:  0xC0
USART3.CTRLC:  0x03
USART3.BAUD:   1666
TX Buffer Free: 64/64
RX Buffer Used: 0/64

Sending test pattern: 0 1 2 3 4 5 6 7 8 9 
═══════════════════════════════════════════════════════════
```

### Timer Information
```bash
AOS> TIMER
⏱️  TIMER STATUS
═══════════════════════════════════════════════════════════
TCA0.SINGLE.CTRLA:    0x01
TCA0.SINGLE.CTRLB:    0x00
TCA0.SINGLE.INTCTRL:  0x01
TCA0.SINGLE.INTFLAGS: 0x00
TCA0.SINGLE.CNT:      1234
TCA0.SINGLE.PER:      999
RTC.CTRLA:            0x81
RTC.STATUS:           0x01
RTC.CNT:              567
RTC.PER:              32767
RTC Interrupt Count:  12345
═══════════════════════════════════════════════════════════
```

## ⏰ Legacy RTC Commands

Your existing alarm clock functionality is preserved:

```bash
AOS> SET 14:30:00
⏰ Time set to 14:30:00

AOS> ALARM 15:00:00  
🔔 Alarm set to 15:00:00

AOS> SHOW
⏰ Current Time: 14:30:15
🔔 Alarm Set: 15:00:00
😴 Status: Waiting...

AOS> STOP
🔕 Alarm stopped
```

## 🔧 Advanced Usage

### Register Bit Manipulation
```bash
# Read current value
AOS> READ 0x1400
📖 Memory Read: 0x1400 = 0x81 (129)
Binary: 10000001

# Set bit 0 (enable)
AOS> WRITE 0x1400 0x81

# Clear bit 0 (disable)  
AOS> WRITE 0x1400 0x80
```

### Finding Register Addresses
```bash
# Use REGS to find the address of any register
AOS> REGS USART3
# Look for the register you need and note its address
# Then use READ/WRITE with that address
```

### Debugging Flow
1. **Check system status**: `SYSINFO`
2. **Inspect relevant registers**: `REGS <peripheral>`
3. **Read specific values**: `READ <address>`
4. **Test hardware**: `GPIO`, `UART`, `TIMER`
5. **Modify and test**: `WRITE <address> <value>`
6. **Monitor changes**: Repeat steps 2-3

### Safety Features
- All memory operations show both old and new values
- Binary representation helps with bit manipulation
- Non-blocking design prevents system lockup
- Error messages guide proper usage

## 🛠️ Integration with Your Code

The system is designed to be non-intrusive. In your main.c:

```c
#include "include/uart.h"
#include "include/ui.h"

int main(void) {
    // Initialize hardware
    uart_init(3, 9600, F_CPU, NULL);
    ui_init();
    sei();
    
    // Show boot message
    ui_show_welcome();
    
    while (1) {
        // Your main loop code
        ui_process_commands();  // Add this for AOS
        
        // Your existing code continues to work
    }
}
```

## 💡 Tips and Tricks

1. **Use tab completion**: Type first few letters and it will suggest
2. **Case insensitive**: `regs rtc` same as `REGS RTC`
3. **Hex addresses**: Always use 0x prefix for clarity
4. **Register shortcuts**: Use PEEK/POKE for quick operations
5. **System reset**: `RESET` command for quick restart
6. **Buffer monitoring**: Check SYSINFO for buffer usage

This system transforms your microcontroller into a powerful debugging platform while maintaining all your existing functionality!