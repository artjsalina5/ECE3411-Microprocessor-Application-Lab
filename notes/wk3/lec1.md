# ECE3411 External Interrupts

## Comic

- In order to recieve the package, the character must have a bookmark and a door-bell
- The bookmark is like the peripheral flag
- The doorbell itself is the interrupt itself.

## Interrupts

- A peripheral may have multiple interrupts
- An interrupt flag is set in the interrupt flag register of the peripheral, even
  if the interrupt is not enabled.
- An interrupt is enabled in the peripherals INTCTRL register
 colloquiollay `peripheral.INTCTRL`.

## Polling vs. Interrupts

- The previous debounce technique using `_delay_ms()` did not allow the CPU to perform
  any other task other than wait. This wastes cycles.
- With interrupts, there are periodic interrupts in the loop to be able to check for a
  specific condition if necessary.

## Interrupt Subsystem

u

- The microcontroller handles unscheduled, although planned, higher priority events.
- The program is transisitoned into a specific task, organized into a function called an Interrupt Service Routine (ISR)
- Once the ISR is over, the microcontroller continues to run in its state.

### Registers for Interrupt Enable

- Intit procedure:
  - Setup Tables
  - Initialize external interrupt, Pin Change interrupt, or Timer interrupt
  - Do bookkeeping before you can enable interrupts

### Execution of an ISR

- A HW Interrupt even which makes to be enabled

## Interrupt Vector Tables

- Contains a list ordered from highest priority down IE

| vector number | Address | Source |
| ---- | --- | ---- |
| 0 | 0x00 | Reset |
| 1 | 0x02 | NMI |

- In this case, the Reset is a higher priority than the nonmaskable interrupt

### Registers for External Interrupts

- PORTx.DIRCLR
- PORTx.PINnCTRL
- PORTx.INTFLAGS
- PORTx.INTCTRL
- ISR(PORTx_PORT_vect)

### Input Sense Configuration (ISC)

- ISC Register the input/sense configuration, is [2:0]
- Table 18.5.12 Multi-Pin Configuration
- This allows for Intdisable (default), bothedges, rising, falling, input disable, level

## Include the interrupt.h

```c
#include <interrupt.h>
```

### Example

- This example configures a pin change on PORTA

```c
#include <avr/io.h>
#include <avr/interrupt.h>

// Interrupt Service Routine for PORTA 
ISR(PORTA_PORT_vect) {
  if (PORTA.INTFLAGS & PIN2_bm) {
    PORTB.OUTTGL = PIN5_bm;
    PORTA.INTFLAGS = PIN2_bm;
  }
}

void Ext_Int_init() {
  PORTA.DIRCLR = PIN2_bm;
  PORTA.PIN2CTRL = PORT)ISC_BOTHEDGES_gc;
  sei(); // to actually enable the interrupt macro
}
```

### Variables for Interrupt Problems

- A common use pattern is that the `ISR` sets a global variable
- Use `volatile` so that the variable can be used globallly by the `C` program.
- This way a race condition such as when a variable change in one subroutine doesn't
  interfere with another variable change

### Solutions

```c
char flag = 0;
int main() {
  while(1) {
    if (flag==1) {
      cli(); // Clear interrupts
      if (flag==1) { flag = 0;}
      sei(); // Set interrupts
    }
  }
}
```

## Lab Practice #3

- Two buttons will be used. One is the on-board button1 (PB2), the other is buttoon2, connect PB5 and set up btoh as input.
- Set up PB2 and PB5 for external input
- Connect 8 LEDs to PD 0 ~ 7 and set up as output.
- Blink all 8 LEDs one aft r the other at 4Hz
- When button 0 is pressed, LED blinks at 8Hz
- When button 1 is pressed, LED blinks at 1Hz
