# Digital Input

## The Tristate Buffer

- In a naive button circuit a closed button connects a pin out the MCU and Gnd.
- You need a pull-up resistor at the pin such that if the
switch is open the voltage at the pin is pulled high.
- The pull-up resistor is implicitly implemented by setting the
output of the pin to high as a result of programming PORTx.

### The Tristate Buffer Purpose

| DDxn | PORTxn | PUD | I/O | Pull-up | comment |
| --------------- | --------------- | --------------- | --------------- | ---- | --- |
| 0 | 0 | X | Input | No | Tri-state (Hi-Z) |
| 0 | 1 | 0 | Input | Yes | Pxn will source current if ext. pulled low. |
| 0 | 0 | X | Input | No | Tri-state (Hi-Z) |
| 1 | 0 | X | Input | No | Tri-state (Hi-Z) |
| 1 | 1 | X | Output | No | Output High (source) |
| 0 | 0 | X | Input | No | Tri-state (Hi-Z) |

## Reading a logic value from a Port

```c
char reg = PORTB.IN; // read the register PORTB.IN as a character variable
reg = 0b10101010;

0b10000000 = (1<<7) = (1<<PIN7_bp)
  if ( reg & (1<<PIN7_bp)){
  /* 7th pin is logic 1 */
  }
  else {
  /* 7th pin is logic 0 */
  }

```

### A Simple Test Program

```c
#include <avr/io.h>

int main(void) {
  PORTB.PIN2CTRL |= PORT_PULLUPEN_bm; // turn on PORTB2 pull up resistor
  PORTB.DIRSET = PIN3_bm;             // configure LED pin as output

  while (1) {
    if ( !(PORTB.IN & PIN2_bm) ) {  // check the button status (press -0, release -1)
      PORTB.OUTCLR = PIN3_bm;       // When button on (0), turn on LED
    }
    else {
      PORTB.OUTSET = PIN3_bm;       // button off (1) turn off LED
    }
  }
}
```

- User LED is active low

## Debounce Circuits

- Capturing a button push is a very fast process.

- when you press a switch closed, two surfaces are brought into contact with each other ->
  no perfect match - electrical contact will be made and
  unmade a few time until the surfaces are firm enough together.

```c
unsigned char counter;
unsigned char pushCount = 0;
unsigned char releaseCount = 0;

while (1) {
  if (!(PORTB.IN & PIN2_bm)) {
    releaseCount = 0;
    pushCount++;
    if (pushCount > 500) {
      counter++;
    }
    else {
      pushCount = 0;
      releaseCount++;
      if (releaseCount > 500) {
        releasecount = 0;
      }
    }
  }
}
```

- Debounce circuitry

## Lab Practice #2

- connect a button to PB5.
-
<!-- vim: set tw=80: -->
