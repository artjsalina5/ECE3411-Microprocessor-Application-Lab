/**
 * @file main.c
 * @author artj
 * @date 2025-09-08
 * @brief Lab Practice 3
 */
#define F_CPU 16000000UL
#include <avr/io.h>
// #include <avr/ioavr128db48.h>

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

volatile uint8_t current_freq = 4;

// Debouncing state variables
typedef struct {
    uint8_t pushCount;
    uint8_t releaseCount;
    uint8_t buttonHandled;
    uint8_t lastState;
} ButtonState_t;

static ButtonState_t btn0_state = {0, 0, 0, 1}; // Start with button released (1)
static ButtonState_t btn1_state = {0, 0, 0, 1};

void init_cpu() {
  ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FRQSEL_16M_gc);
}

// Clean debouncing function using counter-based approach
bool debounceButton(uint8_t port_pin_state, ButtonState_t *btn_state) {
  bool button_pressed = false;
  
  if (!port_pin_state) { // Button pressed (active low)
    btn_state->releaseCount = 0;
    btn_state->pushCount++;
    
    // Button is considered pressed after consistent readings
    if (btn_state->pushCount > 5 && !btn_state->buttonHandled) {
      button_pressed = true;
      btn_state->buttonHandled = 1;
    }
  } else { // Button released
    btn_state->pushCount = 0;
    btn_state->releaseCount++;
    
    // Reset button handled flag after stable release
    if (btn_state->releaseCount > 5) {
      btn_state->buttonHandled = 0;
      btn_state->releaseCount = 0;
    }
  }
  
  return button_pressed;
}

// Helper function to check if button was just pressed
bool isButtonPressed(uint8_t pin_mask, ButtonState_t *btn_state) {
  uint8_t current_state = (PORTB.IN & pin_mask) ? 1 : 0;
  return debounceButton(current_state, btn_state);
}

void delay_by_freq(uint8_t freq) {
    switch (freq) {
        case 1:  _delay_ms(500.0); break;  // 1 Hz  → 500ms half period
        case 2:  _delay_ms(250.0); break;  // 2 Hz  → 250ms half period
        case 3:  _delay_ms(166.67); break; // 3 Hz  → 166.67ms half period
        case 4:  _delay_ms(125.0); break;  // 4 Hz  → 125ms half period
        case 5:  _delay_ms(100.0); break;  // 5 Hz  → 100ms half period
        case 6:  _delay_ms(83.33); break;  // 6 Hz  → 83.33ms half period
        case 7:  _delay_ms(71.43); break;  // 7 Hz  → 71.43ms half period
        case 8:  _delay_ms(62.5); break;   // 8 Hz  → 62.5ms half period
        case 9:  _delay_ms(55.56); break;  // 9 Hz  → 55.56ms half period
        case 10: _delay_ms(50.0); break;   // 10 Hz → 50ms half period
        case 11: _delay_ms(45.45); break;  // 11 Hz → 45.45ms half period
        case 12: _delay_ms(41.67); break;  // 12 Hz → 41.67ms half period
        case 13: _delay_ms(38.46); break;  // 13 Hz → 38.46ms half period
        case 14: _delay_ms(35.71); break;  // 14 Hz → 35.71ms half period
        case 15: _delay_ms(33.33); break;  // 15 Hz → 33.33ms half period
        default: _delay_ms(166.67); break; // Default to 3 Hz
    }
}

ISR(PORTB_PORT_vect) {
  if (PORTB.INTFLAGS & PIN2_bm) {
    current_freq = 8;         // Button 0 pressed - blink at 8Hz
    PORTB.INTFLAGS = PIN2_bm; // Clear the interrupt flag
  } else if (PORTB.INTFLAGS & PIN5_bm) {

    current_freq = 1;         // Button 1 pressed - blink at 1Hz
    PORTB.INTFLAGS = PIN5_bm; // Clear the interrupt flag
  } else if (PORTB.IN & !(PIN2_bm & PIN5_bm)) {
    current_freq = 4; // No button pressed - blink at 4Hz
  }
}

void Ext_Int_Init() {
  PORTB.DIRCLR = PIN2_bm; // Set PB2 as input
  PORTB.DIRCLR = PIN5_bm; // Set PB5 as input
  PORTB.PIN2CTRL =
      PORT_ISC_BOTHEDGES_gc | PORT_PULLUPEN_bm; // Both edges + pullup
  PORTB.PIN5CTRL =
      PORT_ISC_BOTHEDGES_gc | PORT_PULLUPEN_bm; // Both edges + pullup
  sei();                                        // Enable global interrupts
}

void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
}

int main(void) {
  init_cpu();
  init_led();
  Ext_Int_Init();

  uint8_t led_idx = 0;   // Active LED: 0..7
  uint8_t direction = 1; // 1: forward (0->7), 0: backward (7->0)

  PORTD.OUTSET = (1 << led_idx); // Turn on first LED

  while (1) {
    delay_by_freq(current_freq);
    PORTD.OUTCLR = (1 << led_idx); // Turn off current LED

    // Update LED position based on direction
    if (direction == 1) {
      // Moving forward (0 -> 7)
      led_idx++;
      if (led_idx >= 7) {
        led_idx = 7;
        direction = 0; // Change to backward direction
      }
    } else {
      // Moving backward (7 -> 0)
      led_idx--;
      if (led_idx == 0) {
        direction = 1; // Change to forward direction
      }
    }
    
    PORTD.OUTSET = (1 << led_idx); // Turn on new LED
    delay_by_freq(current_freq);
    if (bit_is_set(PORTB.IN, 2) && bit_is_set(PORTB.IN, 5)) {
      current_freq = 4; // No button pressed - blink at 4Hz
    }
  }

  return 0;
}
