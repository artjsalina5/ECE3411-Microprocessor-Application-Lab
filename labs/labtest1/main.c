#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/ioavr128db48.h>

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

volatile uint8_t current_freq = 4;

void init_cpu() {
  ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FRQSEL_16M_gc);
}

bool debounceInput(uint8_t port_val, uint8_t pin) {
  if (!(port_val & (1 << pin))) { // Button pressed (active low with pullup)
    _delay_ms(10);
    if (!(PORTB.IN & (1 << pin))) { // Re-read port to confirm
      return true;
    }
  }
  return false;
}

void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
  PORTC.DIR = PIN6_bm;
  PORTC.DIR = PIN7_bm;
  PORTC.OUTCLR = 0b11000000;
}

int main(void) {
  init_cpu();
  init_led();

  uint8_t led_idx = 0;   // Active LED: 0..7
  uint8_t direction = 1; // 1: forward (0->7), 0: backward (7->0)

  while (1) {

    for (uint8_t i = 0; i <= 8; i++) {
      // PD1 & PD2 Toggle at 2Hz
      PORTD.OUTTGL = 0b0000110;
      _delay_ms(250);

      // PC6 & PC7 Toggle at 5Hz
      PORTC.OUTTGL = 0b1100000;
      _delay_ms(100);
    }

    for (uint8_t i = 0; i <= 8; i++) {
      // PD1 & PD2 Toggle at 5Hz
      PORTD.OUTTGL = 0b0000110;
      _delay_ms(100);

      // PC6 & PC7 Toggle at 2Hz
      PORTC.OUTTGL = 0b1100000;
      _delay_ms(250);
    }

    if (direction) { // Move to next LED position
      led_idx++;     // Forward direction
      if (led_idx >= 7) {
        led_idx = 7;
        direction = 0; // Change direction to backward
      }
    } else {
      led_idx--; // Backward Direction
      if (led_idx <= 0) {
        led_idx = 0;
        direction = 1; // Change direction to forward
      }
    }
  }

  return 0;
}
