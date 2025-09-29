# Digital Alarm Clock - Troubleshooting Guide

## Issue: Time Not Incrementing

If you see that the time stays stuck (like 12:30:00 never changing), this indicates the RTC interrupt is not working properly.

### Debugging Steps

1. **Check if RTC interrupts are firing:**
   ```
   DEBUG
   ```
   Look at the "Interrupt count" value. Run DEBUG again after a few seconds - this number should be increasing.

2. **Verify RTC configuration:**
   The DEBUG command shows several important values:
   - `RTC.CTRLA`: Should be `0x81` (RTC enabled with DIV1 prescaler)
   - `RTC.PER`: Should be `32767` (for 1-second periods)
   - `RTC.INTCTRL`: Should be `0x01` (overflow interrupt enabled)
   - `CLKCTRL.MCLKSTATUS`: Should show external 32kHz crystal is stable
   - `Clock source`: Should be "External 32kHz"

3. **Common Issues and Solutions:**

   **a) External Crystal Not Connected:**
   - Ensure 32.768 kHz crystal is connected to XOSC32K pins
   - Add load capacitors (typically 6-22pF) if needed
   - Check crystal orientation and connections

   **b) Crystal Not Oscillating:**
   - Verify crystal specifications (32.768 kHz, appropriate load capacitance)
   - Check for broken crystal
   - Ensure good solder joints

   **c) Clock Source Issues:**
   - If DEBUG shows "Other" for clock source, the external crystal selection failed
   - System may be falling back to internal oscillator
   - Check CLKCTRL.XOSC32KCTRLA register value in DEBUG

### Alternative: Use Internal Oscillator (for testing)

If external crystal is not available, you can modify the code to use internal oscillator:

In `RTC_init()` function, replace the external crystal setup with:
```c
// Use internal 32kHz oscillator instead of external crystal
RTC_CLKSEL = RTC_CLKSEL_OSC32K_gc;  // Internal 32kHz
```

Note: Internal oscillator is less accurate than external crystal.

### Hardware Verification

1. **Power Supply:**
   - Check that VCC is stable (3.3V or 5V as per design)
   - Verify all ground connections

2. **Crystal Circuit:**
   - 32.768 kHz crystal between XOSC32K pins
   - Load capacitors to ground (value depends on crystal specs)
   - Keep crystal traces short and away from digital signals

3. **MCU Configuration:**
   - Verify fuse settings allow external oscillator use
   - Check that no other peripherals are interfering with clock

### Testing Procedure

1. Program the MCU and connect serial terminal
2. Run `DEBUG` command immediately after reset
3. Set a time: `SET 12:00:00`
4. Wait 10 seconds and run `SHOW` - time should be `12:00:10`
5. If time doesn't advance, check interrupt count with `DEBUG`

### Expected DEBUG Output (Working System)
```
RTC Debug Info:
  RTC.CNT: 15234          (should be changing)
  RTC.PER: 32767          (1 second period)
  RTC.CTRLA: 0x81         (enabled, DIV1)
  RTC.STATUS: 0x00        (synchronized)
  RTC.INTCTRL: 0x01       (overflow interrupt enabled)
  RTC.INTFLAGS: 0x00      (no pending flags)
  CLKCTRL.MCLKSTATUS: 0x10  (external 32kHz stable)
  CLKCTRL.XOSC32KCTRLA: 0x01  (external crystal enabled)
  Interrupt count: 45     (should increase each second)
  Clock source: External 32kHz
```

### Interrupt Not Firing Troubleshooting

If interrupt count stays at 0:

1. **Global Interrupts:** Ensure `sei()` was called
2. **RTC Enable:** Check that RTC.CTRLA has enable bit set
3. **Interrupt Enable:** Verify RTC.INTCTRL has overflow interrupt enabled
4. **Clock Running:** Check that RTC.CNT is incrementing

## Hardware Connection Checklist

```
AVR128DB48 RTC Connections:
┌─────────────────┐
│     MCU         │
│                 │
│ PB4/XOSC32K_1  ├──[32.768kHz]──┐
│                 │                │
│ PB5/XOSC32K_2  ├────────────────┘
│                 │       │
└─────────────────┘    [6-22pF]
                          │
                        GND
```

Load capacitors help ensure proper crystal oscillation. Value depends on crystal specifications (typically 6-22pF for most 32kHz crystals).