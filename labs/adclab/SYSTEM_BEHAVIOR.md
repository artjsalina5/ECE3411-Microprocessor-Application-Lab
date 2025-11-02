# DACLab System Behavior Documentation

## System Overview

This system implements **Arturo's Operating System (AOS)** - a comprehensive debugging interface with the **DACLab** module as a launchable application. The architecture provides a modular framework for adding additional "labs" (applications) in the future.

## Startup Sequence

### 1. **Boot (Power-on or RESET)**

```
MCU initializes:
├── Clock system (16 MHz)
├── UART3 (9600 baud)
├── DAC (off - dac_update = 0)
├── Timer TCA0 (configured but DAC updates disabled)
└── Interrupts enabled (global sei())

Display:
┌───────────────────────────────────────────────┐
│    ARTURO'S OPERATING SYSTEM - ADCLAB         │
│                  BOOTED!                       │
├───────────────────────────────────────────────┤
│ Version: v1.0        Build: [DATE]            │
│ MCU: AVR128DB48      Freq: 16000000 Hz        │
│ UART3: 9600 baud    Interrupts: ENABLED      │
├───────────────────────────────────────────────┤
│                                               │
│ Type HELP for command list, SYSINFO for...   │
│ Type DACLAB to launch Programmable Waveform  │
│                                               │
│ AOS>                                          │
└───────────────────────────────────────────────┘
```

**Key Point:** DAC output (PD6) is **NOT** active at boot. No buzzer sound will be heard.

---

## AOS Command Mode (Default After Boot)

### Available Commands

| Command                     | Function                                                     |
| --------------------------- | ------------------------------------------------------------ |
| **HELP**                    | Display all available commands with descriptions             |
| **SYSINFO**                 | Show MCU info, clock, memory usage, peripheral status        |
| **RESET**                   | Watchdog Timer reset (reboots system, shows boot menu again) |
| **REGS [peripheral]**       | Display register values (RTC, USART3, PORTD, TCA0)           |
| **READ <address>**          | Read from memory address (hex)                               |
| **WRITE <address> <value>** | Write to memory address (hex)                                |
| **DUMP <start> [length]**   | Memory dump starting at address                              |
| **PEEK <address>**          | Peek at single memory location                               |
| **POKE <address> <value>**  | Poke value to memory                                         |
| **UART**                    | Test UART functionality                                      |
| **GPIO <port> <pin> <val>** | Test GPIO (D,B,C; pin 0-7; val 0/1)                          |
| **TIMER**                   | Show timer status                                            |
| **DACLAB**                  | Launch Programmable Waveform Generator                       |

### Command Entry

```
- Type command name (case-insensitive)
- Optional parameters separated by spaces
- Press ENTER to execute
- System prompts: "AOS> "
```

### Example Usage

```
User: HELP
AOS displays all commands

User: DACLAB
AOS launches DACLab...
```

---

## DACLab Mode (Interactive Waveform Generator)

### Launching DACLab

```
1. User types: DACLAB
2. AOS prints transition message:
   "Launching DACLab - Programmable Waveform Generator..."
   "."
   ".."
   "..."
   "AOS Has loaded the Subroutine: DACLab"
   "(Type EXIT to return to AOS)"

3. DACLab Welcome Banner displays:
   ┌───────────────────────────────────────┐
   │              DACLab                   │
   │              ACTIVE!                  │
   ├───────────────────────────────────────┤
   │ Waveform Generator Status:            │
   │   - Sine wave output on DAC0 (PD6)    │
   │   - Default: 500 Hz, 100% amplitude   │
   │                                       │
   │ Interactive Mode:                     │
   │   Type F to change Frequency          │
   │   Type A to change Amplitude          │
   │   Type EXIT to return to AOS          │
   │                                       │
   │ Current Freq: 500 Hz                  │
   │ Amplitude Percent: 100%               │
   │ Do you want to change frequency or    │
   │ amplitude (F/A)?                      │
   └───────────────────────────────────────┘

4. DAC output (buzzer on PD6) STARTS at this point
```

### Interactive Controls

#### **Frequency Control (F)**

```
Prompt: "Do you want to change frequency or amplitude (F/A)?"
User presses: F
System: "Frequency (10-1k Hz): "
User enters: 750 (valid: 10-1000 Hz)
User presses: ENTER
System updates and displays: "Frequency set to 750 Hz"
System recalculates timer period for new frequency
```

#### **Amplitude Control (A)**

```
Prompt: "Do you want to change frequency or amplitude (F/A)?"
User presses: A
System: "Enter Amplitude Percent (10-100): "
User enters: 75 (valid: 10-100%)
User presses: ENTER
System updates and displays: "Amplitude percent set to 75%"
System pre-computes scaled sine table for new amplitude
```

#### **Status Display (Every 5 Seconds)**

```
While in DACLab mode, every 5 seconds the system displays:
"--- Status (5s) --- Freq: 750 Hz, Amplitude: 75%"

This timer is independent of parameter changes and
prints in addition to the interactive prompts.
```

#### **Exit to AOS**

```
Prompt: "Do you want to change frequency or amplitude (F/A)?"
User types: EXIT
System: "Exiting DACLab, returning to AOS..."
DAC output (PD6) STOPS
System displays AOS prompt again
```

---

## Important Implementation Details

### DAC Output Control

- **Initially OFF** (dac_update = 0) to avoid buzzer on boot
- **Enabled when** DACLab is launched (dac_lab_init() sets dac_update = 1)
- **Disabled when** exiting DACLab (dac_lab_exit() sets dac_update = 0)

### Waveform Generation

- **Frequency Range:** 10-1000 Hz
- **Amplitude Range:** 10-100%
- **Waveform:** 64-sample sine table (pre-computed at startup)
- **Amplitude Scaling:** Pre-scaled sine table (updated when amplitude changes)
- **Timer:** TCA0 with dynamic period = F_CPU/(freq×64)-1
- **Output:** DAC0 on PD6, 10-bit resolution, VDD reference

### Timer Interrupt (TCA0_OVF)

- Occurs at rate: frequency × 64 samples/second
- **For 500 Hz:** ~32,000 interrupts/second
- **Actions in ISR:**
  1. Output next DAC sample (if dac_update=1)
  2. Count samples for 5-second status timer
  3. Set display_status_flag every 5 seconds

### Non-blocking Architecture

- **ui_process_commands()**: Processes ONE character per call
- **dac_lab_process()**: Processes ONE character per call
- **Main loop:** Polls both, switching between AOS and DACLab modes
- **UART:** Circular buffer with interrupt-driven TX/RX

### Watchdog Reset (RESET Command)

- Enables WDT with 8-cycle timeout
- System reboots and displays AOS welcome banner
- All settings reset to defaults (500 Hz, 100% amplitude)

---

## Troubleshooting

### Issue: DAC buzzer sounds at startup

**Solution:** Already fixed - DAC is disabled until DACLAB is launched

### Issue: HELP command not displaying

**Solution:** Check UART connection. Command should parse and iterate through command table.

### Issue: 5-second status display not appearing in DACLab

**Solution:** The counter resets when entering DACLab. First display should appear ~5 seconds after launch.

### Issue: Welcome banner not showing when launching DACLab

**Solution:** Fixed - execute_next_command() no longer sends "AOS> " prompt when in DACLab mode

### Issue: Exiting DACLab returns to AOS with DAC still running

**Solution:** Already fixed - dac_lab_exit() disables DAC output

---

## System Architecture

```
main.c
├── Initialization
│   ├── CLOCK_XOSCHF_16M_init()
│   ├── ui_init() → cmd_line_buffer setup
│   ├── uart_init() → USART3 circular buffers
│   ├── DAC_init() → PD6 output config
│   ├── sine_wave_init() → Pre-compute sine table
│   └── init_tca0() → Timer config
│
├── ISR(TCA0_OVF_vect)
│   ├── DAC0_setVal() if dac_update=1
│   ├── Sample counter for 5-second timing
│   └── Set display_status_flag every 5 seconds
│
└── Main Loop
    └── while(1)
        ├── if dac_lab_is_active()
        │   ├── dac_lab_process() → Handle one character
        │   └── if display_status_flag → dac_lab_status_display()
        │
        └── else
            └── ui_process_commands() → Handle one character
                └── execute_next_command() → Dispatch AOS commands

include/ui.c (AOS)
├── Command table (14 commands)
├── Command parsing and dispatch
├── cmd_run_adclab() → Launches DACLab
└── Individual command handlers

include/ui_dac.c (DACLab Module)
├── State machine (F/A entry)
├── dac_lab_init() → Enable DAC, init state
├── dac_lab_process() → Handle input
├── dac_lab_show_welcome() → Display banner
├── dac_lab_exit() → Disable DAC, return to AOS
└── dac_lab_is_active() → Mode check

include/dac.c
├── sine_wave_init() → Pre-compute sine table
├── update_sine_wave_scaled() → Scale by amplitude
└── DAC0_setVal() → Output 10-bit value

include/uart.c
├── uart_init() → Configure USART3
├── uart_receive_char() → Non-blocking RX
├── uart_send_char() → Non-blocking TX
└── ISR handlers for RX/TX
```

---

## Command Flow Example: User Launches DACLab

```
1. User at AOS> prompt types "DACLAB"

2. ui_process_commands()
   └─ uart_receive_char() returns 'D'
   └─ accumulates in current_cmd_line[]

3. After "DACLAB\r" received and Enter pressed
   └─ queue_command_line("DACLAB")
   └─ execute_next_command()
      ├─ Parse: cmd_name="DACLAB", params=NULL
      ├─ Search command table
      ├─ Find and execute cmd_run_adclab()
      │  ├─ Send "Launching DACLab..."
      │  ├─ dac_lab_init() → daclab_active=true, dac_update=1
      │  └─ dac_lab_show_welcome()
      └─ if (!dac_lab_is_active()) send "AOS> "  ← SKIPPED because now in DACLab

4. Next main loop iteration
   └─ if (dac_lab_is_active()) is TRUE
      └─ dac_lab_process() handles input

5. User is now in DACLab interactive mode
```

---

## Expected Output Sequence

### Boot

```
UART Initialized at 9600 baud
DAC0 Initialized to Output at Pin D.6
Configured DAC0 with output enable and run in standby
Set DAC0 reference voltage to VDD (3.3V) with always on mode

+-----------------------------------------------------------+
|          ARTURO'S OPERATING SYSTEM - ADCLAB               |
|                          BOOTED!                          |
+-----------------------------------------------------------+
| Version: v1.0        Build: Oct 27 2025                   |
| MCU: AVR128DB48           Freq:       16000000 Hz         |
| UART3: 9600baud         Interrupts: ENABLED             |
+-----------------------------------------------------------+

Type HELP for command list, SYSINFO for system status

Type DACLAB to launch the Programmable Waveform Generator

AOS>
```

### After DACLAB Command

```
AOS> DACLAB

Launching DACLab - Programmable Waveform Generator...
.
..
...
AOS Has loaded the Subroutine: DACLab
(Type EXIT to return to AOS)

+-----------------------------------------------------------+
|                          DACLab                           |
|                          ACTIVE!                          |
+-----------------------------------------------------------+

Waveform Generator Status:
  - Sine wave output on DAC0 (PD6)
  - Default: 500 Hz, 100% amplitude

Interactive Mode:
  Type F to change Frequency
  Type A to change Amplitude
  Type EXIT to return to AOS

Current Freq: 500 Hz
Amplitude Percent: 100%
Do you want to change frequency or amplitude (F/A)?
```

### After 5 Seconds in DACLab

```
Do you want to change frequency or amplitude (F/A)?
--- Status (5s) --- Freq: 500 Hz, Amplitude: 100%
```

---

## Future Extensibility

The modular design allows easy addition of new labs:

1. Create `include/ui_mylab.c/h` with functions:
   - `mylab_init()`
   - `mylab_process()`
   - `mylab_show_welcome()`
   - `mylab_exit()`
   - `mylab_is_active()`

2. Add to ui.c:
   - `cmd_run_mylab()` handler
   - Command table entry

3. Update main.c loop:
   - Add additional `if (mylab_is_active())` check

Example additional labs:

- ADCLab: Analog-to-Digital Converter interface
- RLab: Relay/GPIO control
- UARTLab: Serial communication testing
- CalibrationLab: Hardware calibration routines

## LabTest 3:

1. Create an adaptive LED pattern based on Potentiometer input

- Read the Potentiometer input from pin.E0 with full 12 bit resolution and
  every 10ms and average every 10 readings every 100ms.
- Use the sampled voltage to select one of four LED animation modes on PORTD.
  - MODE 0: ALL OFF
  - MODE 1: LEDs blink in sequence on PORTD. (night rider chase effect LED1 ->
    LED2) Except the PD6.
  - MODE 2: LEDs on PORTD blink at 8Hz
  - MODE 3: Single LED brightness changing using PWM on PIN C1

  $$mode = ((int)(Vpot * 10)) % 4$$

- Connect button to PB5 and setup interrupt. When button is pressed the mode
  should freeze and remain the same no matter the sampled voltage.
- When frozen in MODE 3, use PWM on Pin C1 to control a singular LED brightness.
  Duty cycle updates every 100ms
  $$ duty % = (Vpot/Vref)\*100 $$
- Print current mode and duty to UART every 3 seconds in the format:
  "Voltage = XX.XX V, Duty = XX.XX %, Mode = X (frozen/Unfrozen)"

2. Using the potentiometer on PIN E0 to control a DAC generated waveform the
   system should provide both analog output and visual feedback.
   - On the same averaged potentiometer voltage from part 1 to control the DAC
     output on Pin A0.
     - Configure the DAC for 10-bit mode and continuous output.
     - Generate a sine waveform with amplitude and frequency determined by the
       potentiometer voltage
       "Amplitude % = (Vpot/3.3)" "f = 10 + (Vpot \*90) Hz"
     - This should used a timer interrupt.

- When the on board button PB2 is pressed, enable a frequency sweep mode such
  that:
  - The DAC outputs a sine waveform whose frequency increases from 10Hz ->
    100Hz linearly over 5 seconds, then decreases back to 10Hz over the next
    five seconds. - Amplitude A continues to depend on potentiometer voltage. - The sweep repeats continuously while in this mode. - Pressing the button again returns the system to normal fixed-frequency
    operation.

    $$
    f(t) = either 10 + 18t for 0 <= t < 5 or 100 - 18(t-5) for 5<= t < 100
    $$

    The key to this part is to incrementally adjust frequency at regular time
    intervals to approximate this ramp and to give the impression of a
    continuous sweep.
    Print State to UART every 2 seconds:
    "Vpot = X.XX V, A = X.XX V, f = XXX Hz, Mode = Sweep/Fixed"
    $$

3. Implement EEPROM functionality to specified data on the press of BOTH PB5 and
   PB2 buttons:
   - Save LED Animation mode and if frozen from part 1 and resume in same state
     on restart
   - Save fixed frequency or sweep frequency mode from part 2 and start on that
     mode on restart
     - If eeprom is empty (contains 0xFF) default to fixed frequency mode and
       potentiometer voltage.
     - Make a function to vetrify EEPROM contents.
