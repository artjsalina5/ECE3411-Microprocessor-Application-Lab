#define F_CPU 16000000UL
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/ioavr128db48.h>
#include <util/delay.h>

void init_cpu() {
  ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FRQSEL_16M_gc);
}

// #include <avr/ioavr128db48.h>
#include <stdbool.h>
#include <stdint.h>

bool debounceInput(uint8_t port, uint8_t pin) {
  if (!(port & (1 << pin))) {
    _delay_ms(10);
    if (!(port & (1 << pin))) {
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
    _delay_ms(166.667);
    break; // fallback (3 Hz)
  }
}

static inline uint8_t clamp_freq(int16_t f) {
  if (f < FREQ_MIN)
    return FREQ_MIN;
  if (f > FREQ_MAX)
    return FREQ_MAX;
  return (uint8_t)f;
}

int main(void) {
  init_cpu();

  PORTD.DIRSET = 0xFF; // PD0..PD7 = outputs
  PORTD.OUTCLR = 0xFF; // all off

  PORTB.PIN2CTRL |= PORT_PULLUPEN_bm;
  PORTB.PIN5CTRL |= PORT_PULLUPEN_bm;

  PORTB.DIRCLR = (1 << 2) | (1 << 5);

  uint8_t led_idx = 2;   // Active LED: 0..7
  uint8_t freq = 3;      // 3 Hz
  uint8_t direction = 1; // 1: up, 0: down

  while (1) {

    bool BTN1 = debounceInput(PORTB.IN, 2); // Port B, Pin 2
    bool BTN2 = debounceInput(PORTB.IN, 5); // Port B, Pin 5

    if (!BTN1 && !BTN2) {
      // forward 0..7
      for (uint8_t i = 0; i < 8; i++) {
        blinkyblink(freq, &PORTB, (uint8_t)(1u << i)); // or (PIN0_bm << i)
      }

      // reverse 7..0 (safe descending idiom)
      for (int8_t i = 7; i >= 0; i--) {
        blinkyblink(freq, &PORTB, (uint8_t)(1u << i));
      }

    } else if (BTN1 && !BTN2) {
      freq = freq + 1; // Increase frequency

    } else if (BTN2 && !BTN1) {
      freq = freq - 1; // Decrease frequency

    } else if (BTN1 && BTN2) {
      if (direction) {
        PORTD.OUTCLR = (1 << led_idx);
        if (led_idx < 7)
          led_idx++;
        if (led_idx == 7)
          direction = false;
      } else {
        PORTD.OUTCLR = (1 << led_idx);
        if (led_idx > 0)
          led_idx--;
        if (led_idx == 0)
          direction = true;
      }
      PORTD.OUTSET = (1 << led_idx);
    }
  }
  return 0;
}
