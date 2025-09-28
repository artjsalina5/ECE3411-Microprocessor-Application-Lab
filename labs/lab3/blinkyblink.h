/* blinkyblink.h
 * Minimal millisecond-granularity blinker for AVR-Dx using OUTSET/OUTCLR.
 * - Uses only _delay_ms(1.0) (constant literal) from <util/delay.h>.
 * - Define F_CPU (e.g., -DF_CPU=16000000UL) and compile with -Os or -O2.
 */

#ifndef BLINKYBLINK_H
#define BLINKYBLINK_H

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#ifndef F_CPU
#error "Define F_CPU (e.g., -DF_CPU=16000000UL) before including blinkyblink.h"
#endif

/* ---- Internal: 1 ms chunks to keep _delay_ms argument constant ---- */
static inline void bl_delay_ms_var(uint32_t ms) {
  while (ms--) {
    _delay_ms(1.0);
  }
}

/* ---- 50% duty versions (OUTSET → delay → OUTCLR → delay) ---- */

// #include "blinkyblink.h"
//
// int main(void) {
//   PORTD.DIRSET = PIN0_bm;
//   // 50% duty, ~7 Hz
//   // blinkyblink_forever_ms(7, &PORTD, PIN0_bm);
//
//   // 200 Hz, 20% duty (permille = 200)
//   bl_pwm_forever_ms(200, 200, &PORTD, PIN0_bm);
// }
/* One full period at ~50% duty. */
static inline void blinkyblink(uint32_t freq_hz, volatile PORT_t *port,
                               uint8_t pin_bm) {
  if (freq_hz == 0U) {
    port->OUTCLR = pin_bm;
    return;
  }
  uint32_t half_ms = (500U + freq_hz / 2U) / freq_hz; /* ≈ 500/f, rounded */
  if (half_ms == 0U) {
    half_ms = 1U; /* cap at 1 ms → max ~500 Hz */
  }
  port->OUTSET = pin_bm;
  bl_delay_ms_var(half_ms);
  port->OUTCLR = pin_bm;
  bl_delay_ms_var(half_ms);
}

/* Repeat for N periods. */
static inline void blinkyblink_periods_ms(uint32_t freq_hz,
                                          volatile PORT_t *port, uint8_t pin_bm,
                                          uint32_t periods) {
  while (periods--) {
    blinkyblink_once_ms(freq_hz, port, pin_bm);
  }
}

/* Run forever at ~50% duty. */
static inline void blinkyblink_forever_ms(uint32_t freq_hz,
                                          volatile PORT_t *port,
                                          uint8_t pin_bm) {
  for (;;) {
    blinkyblink_once_ms(freq_hz, port, pin_bm);
  }
}

/* ---- Duty-cycle versions (permille 0..1000), ms granularity ---- */
/* One full PWM period with duty in permille (e.g., 200 = 20%). */
static inline void bl_pwm_once_ms(uint32_t freq_hz, uint16_t duty_pm,
                                  volatile PORT_t *port, uint8_t pin_bm) {
  if (freq_hz == 0U) {
    port->OUTCLR = pin_bm;
    return;
  }
  if (duty_pm > 1000U) {
    duty_pm = 1000U;
  }

  uint32_t period_ms = (1000U + freq_hz / 2U) / freq_hz; /* ≈ 1000/f */
  if (period_ms == 0U) {
    period_ms = 1U; /* cap at 1 ms → max ~1000 Hz */
  }

  uint32_t high_ms =
      (period_ms * (uint32_t)duty_pm + 500U) / 1000U; /* rounded */
  if (high_ms > period_ms) {
    high_ms = period_ms;
  }
  uint32_t low_ms = period_ms - high_ms;

  if (high_ms) {
    port->OUTSET = pin_bm;
    bl_delay_ms_var(high_ms);
  } else {
    port->OUTCLR = pin_bm;
  }

  if (low_ms) {
    port->OUTCLR = pin_bm;
    bl_delay_ms_var(low_ms);
  } else {
    port->OUTSET = pin_bm;
  }
}

/* Repeat PWM for N periods. */
static inline void bl_pwm_periods_ms(uint32_t freq_hz, uint16_t duty_pm,
                                     volatile PORT_t *port, uint8_t pin_bm,
                                     uint32_t periods) {
  while (periods--) {
    bl_pwm_once_ms(freq_hz, duty_pm, port, pin_bm);
  }
}

/* Run PWM forever. */
static inline void bl_pwm_forever_ms(uint32_t freq_hz, uint16_t duty_pm,
                                     volatile PORT_t *port, uint8_t pin_bm) {
  for (;;) {
    bl_pwm_once_ms(freq_hz, duty_pm, port, pin_bm);
  }
}

#endif /* BLINKYBLINK_H */
