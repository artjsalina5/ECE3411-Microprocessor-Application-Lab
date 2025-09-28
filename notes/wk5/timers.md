# Timer Counter

## TCA

- 16 Bit Timer/Counter
- Three Comparison Channels
- Double-Buffered Timer Period Setting
- Double-Buffered Compare Channel
- Waveform Generation
  - Frequency Generation
  - Single-Slope PWM
  - Dual-Slop PWM
- Count on Event
- Timer Overflow Interrupts/Events
- One Compare Match per Compare
- Two 8-bit Timer/Counters in Split Mode

### Block Diagram

- Refer to Figure 23.1
  - A Prescaler and Event system go into our Base Counter
    - Base Counter Contains the Timer Period, Counter and Control Logic
  - These can be sent to the 3 Compare Channels, 0, 1, 2

## Initialization

- Write a TOP value to the TCAn.PER register
- Enable the peripheral by writing '1' to TCAn.CTRLA register.
  - Counter will start counting clock ticks according to the prescaler setting
    - CLKSEL field in TCAn.CTRLA
- By Writing a '1' to the Enable Counter Event Input A (CNTAEI) bit in the Event
  Control (TCAn.EVCTRL) register, events are counted instead of clock ticks
- Counter value can be read from the CNT bit field in the TCAn.CNT register
