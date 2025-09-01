# Lecture 2 Digital Output

## Real Life Examples of Digital Inputs

- LED's
- Switches

## AVR128DB48 Package

- Various Power Supplies
  - VDD Power Domain
  - AVDD Power Domain
  - VDDIO2 Power Domain

- Various Functionalities
  - Programming/Debug
  - Clock/Crystal
  - Digital Function Only
  - Analog Function

- Ports A-F
  - 8 Pins each

- Clocks
  - Ability to configure an eXTernAL 32KHz clock on PF1 and PF0
  - Ability to configure an eXTernAL Clock with a positive and negative lead

## AVRDB Device Designation

- AVR 128 DB 48 T
  - 128 Memory
  - Device Revision
  - Pin Count
  - Temperature

- DA vs DB
  - DA lacks OPAMS
  - DA lacks DAC
  - DA only has 1 vs 2 comparators
  - DB has more alternate functions

## I/O Multiplexing

- The pins are able to be Multiplexed with different functionalities.

## Architecture

- To maximize performance and parallelism, the AVR CPU uses a Harvard
  architecture with separate buses for program and data. Instructions in the program
  memory are executed with a single-level pipeline. While one instruction is being
  executed, the next instruction is pre-fetched from the program memory. This enables
  instructions to be executed on every clock cycle.

- 32 Registers

## Memory Map

## Register & Port

- A Register is a collection of flip-flops
- These can be simultaneously loaded in parallel or read
- Interface between users and subsystems
- Viewed as a software configurable switch

| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
| --- |  ----- | ---- | --- | ---   | --- | ---  | -----|

- Each pin must be `initialized` with the three primary registers associated with it.

    These registers must be used to enable and the use of these ports.

  - **DIR**
  - **OUT**
  - **IN**

### Initialization

  After Reset, all outputs are tri-stated, and digital input buffers enabled even if there is no clock running.
  The following steps are all optional when initializing PORT operation:

- Enable or disable the output driver for pin Pxn by respectively writing ‘1’ to bit n in the PORTx.DIRSET or
  PORTx.DIRCLR register
- Set the output driver for pin Pxn to high or low level respectively by writing ‘1’ to bit n in the PORTx.OUTSET or
  PORTx.OUTCLR register
- Read the input of pin Pxn by reading bit n in the PORTx.IN register
- Configure the individual pin configurations and interrupt control for pin Pxn in PORTx.PINnCTRL

## Predefined Registers

- How do you write to the PORTB OUT register?

```c
#include <avr/ioavr128db48.h>
PORTB_OUT = 0b00000000
```

- The registers can be thought of as regular variables
- AVR library has predefined keywords for each `bit position` of each port register.
  - The 5th position of PIN5 is `PIN5_bp`

  ```c
  #define PIN5_bp 5
  ```

## Hardware Registers

What about the other PORT registers?

- DIRSET, DIRCLR, DIRTGL
- OUTSET, OUTCLR, OUTTGL

### VPORT

- These are single cycle registers

## Structure of AVR C Code

```c
[preamble & includes]
[possibly some function definitions]

int main(void) {
  [chip initalizations]
  while (1) {
    [do this stuff forever]
    // never returns
  }
}
```

## AVR Software Development

1. Write Source
2. Pre-process Code
3. Compile Code
4. Assemble Code
5. Link Code
6. Programming
7. Target Flash

## Test Code

```c
// -------- Preamble ---------- //
#define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void) {
  /* Data Direction Register D: writing a one enables output. */
  VPORTD.DIR = 0b11111111;
  VPORT_OUT  = 0b00000000;

  while (1) {
    VPORTD.OUT = 0b11111111; // Turns on all ports
    _delay_ms(1000);
    VPORTD.OUT = 0b00000000;
    _delay_ms(1000);
  }
}
```

## Bit Shifting

```c
|= Set
&= Clear
```
