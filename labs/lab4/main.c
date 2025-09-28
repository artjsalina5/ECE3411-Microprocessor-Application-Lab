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

enum { FREQ_MIN = 1, FREQ_MAX = 10 };

static inline void delay_halfperiod_by_freq(uint8_t f) {
  switch (f) {
  case 1:
    _delay_ms(500.0);
    break; // 1 Hz  → T=1000 ms → T/2 = 500 ms
  case 2:
    _delay_ms(250.0);
    break; // 2 Hz  → 500 ms → 250 ms
  case 3:
    _delay_ms(166.667);
    break; // 3 Hz  → 333.333 ms → 166.667 ms
  case 4:
    _delay_ms(125.0);
    break; // 4 Hz  → 250 ms → 125 ms
  case 5:
    _delay_ms(100.0);
    break; // 5 Hz  → 200 ms → 100 ms
  case 6:
    _delay_ms(83.333);
    break; // 6 Hz  → 166.667 → 83.333
  case 7:
    _delay_ms(71.429);
    break; // 7 Hz  → 142.857 → 71.429
  case 8:
    _delay_ms(62.5);
    break; // 8 Hz  → 125 → 62.5
  case 9:
    _delay_ms(55.556);
    break; // 9 Hz  → 111.111 → 55.556
  case 10:
    _delay_ms(50.0);
    break; // 10 Hz → 100 → 50
  default:
    _delay_ms(125.0);
    break; // fallback (4 Hz default)
  }
}

ISR(PORTB_PORT_vect) {
  if (PORTB.INTFLAGS & PIN2_bm) {
    // Button 0 pressed - blink at 8Hz
    current_freq = 8;
    PORTB.INTFLAGS = PIN2_bm; // Clear the interrupt flag
  } else if (PORTB.INTFLAGS & PIN5_bm) {
    // Button 1 pressed - blink at 1Hz
    current_freq = 1;
    PORTB.INTFLAGS = PIN5_bm; // Clear the interrupt flag
  }
}

void Ext_Int_Init() {
  PORTB.DIRCLR = PIN2_bm; // Set PB2 as input
  PORTB.DIRCLR = PIN5_bm; // Set PB5 as input
  PORTB.PIN2CTRL =
      PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm; // Falling edge + pullup
  PORTB.PIN5CTRL =
      PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm; // Falling edge + pullup
  sei();                                      // Enable global interrupts
}

void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
}

// Two buttons will be used. One is the on-board button1 (PB2) and the
// other is the external button2 connect to PB5 and set up both as input.
// • Set up PB2 and PB5 for External Interrupt.
// • Connect 8 LEDs to PD 0 ~ 7 and set up as output.
// • Blink all 8 LEDs one after another with the frequency of 4Hz.
// • When Button 0 is pressed, LED blinks at 8Hz.
// • When Button 1 is pressed, LED blinks at 1Hz.
//
int main(void) {
  init_cpu();
  init_led();
  Ext_Int_Init();

  uint8_t led_idx = 0;   // Active LED: 0..7
  uint8_t direction = 1; // 1: forward (0->7), 0: backward (7->0)

  // Turn on first LED
  PORTD.OUTSET = (1 << led_idx);

  while (1) {
    delay_halfperiod_by_freq(current_freq);
    PORTD.OUTCLR = (1 << led_idx);

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
    PORTD.OUTSET = (1 << led_idx);
    delay_halfperiod_by_freq(current_freq);
  }

  return 0;
}
