/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief USART Interrupt-Driven Example with Non-Blocking Menu
 */

#define F_CPU 16000000UL // 16 MHz clock speed
#include "include/uart.h"
#include "include/circularbuff.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define BAUD_RATE 9600

//********************************
// LED And Button Initialization
//********************************
void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
  PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
  PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
}

/* Drive exactly one LED (0..7). Any out-of-range value turns all off. */
static inline void leds_set_position(uint8_t pos) {
  if (pos < 8U) {
    PORTD.OUT = (uint8_t)(1U << pos);
  } else {
    PORTD.OUTCLR = 0xFF;
  }
}

/* Toggle the currently selected LED without affecting others. */
static inline void leds_toggle_position(uint8_t pos) {
  if (pos < 8U) {
    PORTD.OUTTGL = (uint8_t)(1U << pos);
  }
}

static inline uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi) {
  if (v < lo) {
    return lo;
  } else if (v > hi) {
    return hi;
  } else {
    return v;
  }
}

static inline uint16_t half_ms_from_freq(uint8_t freq_hz) {
  if (freq_hz == 0U) {
    return 500U;
  }
  uint16_t half = (uint16_t)((500U + (freq_hz / 2U)) / freq_hz); /* rounded */
  if (half == 0U) {
    half = 1U;
  } else if (half > 500U) {
    half = 500U;
  }
  return half;
}

//********************************
// CPU Initialization
//********************************
/*
static inline void init_cpu(void) {
  // ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), (CLKCTRL_FRQSEL_16M_gc));
  CPU_CCP = CCP_IOREG_gc;                     // unlock protected IO regs
  CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_16M_gc; // internal HF osc 16 MHz
}
*/

static inline void init_cpu(void) {
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_ENABLE_bm;
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_EXTCLK_gc;
}

// ********************************
// USART Interrupt Service Routines
// USART3_RXC_vect Receive Complete Interrupt
// USART3_DRE_vect Data Register Empty Interrupt
// USART3_TXC_vect Transmit Complete Interrupt
// ********************************
ISR(USART3_RXC_vect) {
  // if we have received all the data, call the handler
  char receivedChar = USART3.RXDATAL; // Read received character
  uart_rx_isr_handler(receivedChar);
}

ISR(USART3_DRE_vect) {
  // if the data register is empty and we have data to send, send it.
  char data_to_send;
  if (uart_tx_isr_handler(&data_to_send)) {
    USART3.TXDATAL = data_to_send; // Send next character
  } else {
    USART3.CTRLA &= ~USART_DREIE_bm; // Disable DRE interrupt if buffer empty
  }
}
// static void generic_state_machine(uint8_t *mod1, uint8_t *mod2) {
// Placeholder for a generic state machine function
// define an enum for states
/*
static enum {
  STATE_IDLE,
  STATE_WAIT_CHOICE,
  STATE_WAIT_MODIFIER1,
  STATE_WAIT_MODIFIER2
  } state = STATE_IDLE;
*/
//}
// Non-blocking menu handler using UART interrupt-driven input
static void prompt_and_handle_menu(uint8_t *freq_hz, uint8_t *pos) {
  // **************************************************************************
  // Mode Flag with States:
  // Waiting for F/P choice
  // Waiting for Frequency input
  // Waiting for Position input
  //***************************************************************************
  static enum {
    MENU_IDLE,
    MENU_WAIT_CHOICE,
    MENU_WAIT_FREQ,
    MENU_WAIT_POS
  } state = MENU_IDLE;

  static char input_buffer[64];
  static uint8_t input_index = 0;
  static bool prompt_shown = false;
  char ch;

  // Show prompt only when transitioning to IDLE state and not already shown
  if (state == MENU_IDLE && !prompt_shown) {
    printf("\nDo you want to change the frequency or position? (F/P)\n> ");
    // fflush(stdout);
    prompt_shown = true;
  }

  // Try to read one char using unified API (non-blocking)
  if (!uart_receive_char(&ch)) {
    return; // No input available, return immediately
  }

  // Echo the character
  printf("%c", ch);
  switch (state) {
  case MENU_IDLE:
    // On any input, move to choice state
    state = MENU_WAIT_CHOICE;
    input_index = 0;
    memset(input_buffer, 0, sizeof(input_buffer));
    prompt_shown = false; // Reset for next time
    // Store first char
    if (input_index < 63) {
      input_buffer[input_index++] = ch;
    }
    break;

  case MENU_WAIT_CHOICE:
    if (ch == '\n' || ch == '\r') {
      if (input_index == 0)
        break; // Ignore empty input
      char choice = input_buffer[0];
      if (choice == 'F' || choice == 'f') {
        printf("\nFrequency (1-10 Hz):\n> ");
        state = MENU_WAIT_FREQ;
        input_index = 0;
        memset(input_buffer, 0, sizeof(input_buffer));
      } else if (choice == 'P' || choice == 'p') {
        printf("\nPosition (0-7):\n> ");
        state = MENU_WAIT_POS;
        input_index = 0;
        memset(input_buffer, 0, sizeof(input_buffer));
      } else {
        printf("\nUnrecognized option '%c'. Please enter F or P next time.\n",
               choice);
        state = MENU_IDLE;
        prompt_shown = false;
      }
    } else if (input_index < 63) {
      input_buffer[input_index++] = ch;
    }
    break;

  case MENU_WAIT_FREQ:
    if (ch == '\n' || ch == '\r') {
      if (input_index == 0)
        break;
      unsigned int newf = atoi(input_buffer);
      uint8_t nf = clamp_u8((uint8_t)newf, 1U, 10U);
      if (nf != newf) {
        printf("\nOut of range. Clamped to %u Hz.\n", (unsigned)nf);
      }
      *freq_hz = nf;
      printf("OK. Frequency set to %u Hz.\n", (unsigned)*freq_hz);
      state = MENU_IDLE;
      prompt_shown = false;
    } else if (input_index < 63) {
      input_buffer[input_index++] = ch;
    }
    break;

  case MENU_WAIT_POS:
    if (ch == '\n' || ch == '\r') {
      if (input_index == 0)
        break;
      unsigned int newp = atoi(input_buffer);
      uint8_t np = clamp_u8((uint8_t)newp, 0U, 7U);
      if (np != newp) {
        printf("\nOut of range. Clamped to %u.\n", (unsigned)np);
      }
      *pos = np;
      leds_set_position(*pos);
      printf("OK. Position set to %u.\n", (unsigned)*pos);
      state = MENU_IDLE;
      prompt_shown = false;
    } else if (input_index < 63) {
      input_buffer[input_index++] = ch;
    }
    break;
  }
}
// ****************************************************************************

int main(void) {
  uint32_t F_CLK_PER = F_CPU;
  init_cpu();
  init_led();
  /* Initialize UART stdio on USART3 @ 9600 8N1 */
  /* (uart_init now automatically enables RX interrupt for unified API) */
  uart_init(3, 9600, F_CLK_PER, NULL);

  sei(); // Enable global Interrupts
  printf("UART Interrupt-Driven LED Blinker Initialized.\n");
  printf("Setting up interrrrrnaaalll pointerrr varrraiblee\n");
  /* State */
  uint8_t freq_hz = 2U; /* starts at 2 Hz */
  uint8_t led_pos = 0U; /* starts at PD0  */
  uint16_t half_ms = half_ms_from_freq(freq_hz);

  /* Timekeeping using 10 ms ticks */
  uint16_t tick10_ms = 0U; /* accumulates to half_ms */

  /* Initialize LED output for the current position */
  leds_set_position(led_pos);
  bool led_on_phase = true;

  while (1) {
    /* 10 ms base tick */
    _delay_ms(10);
    tick10_ms += 10U;

    /* Toggle at half-period boundary */
    if (tick10_ms >= half_ms) {
      leds_toggle_position(led_pos);
      led_on_phase = !led_on_phase;
      tick10_ms = 0U;
    }

    /* Non-blocking menu handler: call every tick */
    prompt_and_handle_menu(&freq_hz, &led_pos);

    /* If frequency changed, recompute timing */
    half_ms = half_ms_from_freq(freq_hz);

    /* Make sure only the selected LED is driven after interaction */
    if (led_on_phase) {
      leds_set_position(led_pos);
    } else {
      PORTD.OUTCLR = 0xFF;
    }
  }

  /* not reached */
  return 0;
}
