/**
 * @file   main.c
 * @author A. Salinas-Aguayo
 * @date   2025-08-27
 * @brief  Blink 'n shift.
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

static inline void init_cpu(void) {
  CPU_CCP = CCP_IOREG_gc;                     // unlock protected IO regs
  CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_16M_gc; // internal HF osc 16 MHz
}

int main(void) {

  init_cpu();

  PORTD_DIRSET = 0xFF; // all set as outputs
  PORTD_OUTCLR = 0xFF; // all off

  PORTB_DIRSET = PIN3_bm;
  PORTB_OUTCLR = PIN3_bm;

  uint8_t led_idx = 0;           // Active LED: 0..7
  bool fast = false;             // false=2 Hz (250 ms), true=4 Hz (125 ms)
  uint16_t elapsed_freq_ms = 0;  // Counts to 2000 ms (flip freq)
  uint16_t elapsed_shift_ms = 0; // Counts to 4000 ms (shift LED)

  PORTD_OUTCLR = (uint8_t)(1u << led_idx);

  while (1) {
    PORTD_OUTTGL = (uint8_t)(1u << led_idx);

    if (fast) {
      _delay_ms(125); // 4 Hz half-period
      elapsed_freq_ms += 125;
      elapsed_shift_ms += 125;
    } else {
      _delay_ms(250); // 2 Hz half-period
      elapsed_freq_ms += 250;
      elapsed_shift_ms += 250;
    }

    if (elapsed_freq_ms >=
        2000) { // Every 2 seconds: flip frequency 2 Hz -> 4 Hz.
      elapsed_freq_ms = 0;
      fast = !fast;
    }

    if (elapsed_shift_ms >= 4000) {
      elapsed_shift_ms = 0;
      PORTD_OUTCLR = (uint8_t)(1u << led_idx); // Force current LED OFF
      led_idx = (uint8_t)((led_idx + 1u) & 0x07u);
      PORTD_OUTCLR =
          (uint8_t)(1u << led_idx); // Start the new LED from a known OFF state.
    }
  }
  return 0;
}
