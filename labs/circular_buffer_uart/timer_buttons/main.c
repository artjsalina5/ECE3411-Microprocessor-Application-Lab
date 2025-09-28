/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief USART Interrupt-Driven Example with Non-Blocking Menu
 */

#define F_CPU 16000000UL // 16 MHz clock speed
#include "include/uart.h"
#include "include/circularbuff.h"
#include "include/tca.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/ioavr128db48.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define BAUD_RATE 9600

// Global variables for LED control and frequency
volatile uint8_t led_frequency = 3;    // Start at 3Hz for LED 2 (3rd LED)
volatile uint16_t timer_ticks = 0;     // Timer tick counter
volatile uint16_t led_toggle_ticks = 0; // Ticks needed for LED toggle
volatile uint16_t uart_report_ticks = 0; // Ticks for 5-second UART reporting
volatile bool led_state = false;       // Current state of LED 2

// Button debouncing variables
volatile uint8_t btn1_prev = 1;
volatile uint8_t btn2_prev = 1;
volatile uint8_t btn1_debounce = 0;
volatile uint8_t btn2_debounce = 0;

// Function declarations
void handle_buttons(void);

//===============================================================================
// LED And Button Initialization
//===============================================================================
void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
  PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
  PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
}

void init_button() {
  // BTN1 (increment frequency) - PB5
  PORTB.DIRCLR = PIN5_bm;            // Set PB5 as input
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Enable pull-up
  
  // BTN2 (decrement frequency) - PB2
  PORTB.DIRCLR = PIN2_bm;            // Set PB2 as input
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up
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

//===================================================================================
// CPU Initialization
//===================================================================================
static inline void init_cpu(void) {
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_ENABLE_bm;
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_EXTCLK_gc;
}

// ===================================================================================
// USART Interrupt Service Routines
// USART3_RXC_vect Receive Complete Interrupt
// USART3_DRE_vect Data Register Empty Interrupt
// USART3_TXC_vect Transmit Complete Interrupt
// ===================================================================================
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


//============================================================================
// Timer callback function - called every 1ms
//============================================================================
void timer_callback(void) {
    timer_ticks++;
    
    // Handle button debouncing every 1ms
    handle_buttons();
    
    // Handle LED blinking - calculate ticks needed for current frequency
    // For frequency F, we need to toggle every (1000ms / (2*F)) ticks
    uint16_t toggle_period = 1000 / (2 * led_frequency);
    
    if (timer_ticks >= led_toggle_ticks + toggle_period) {
        led_toggle_ticks = timer_ticks;
        // Toggle LED 2 (3rd LED, index 2)
        leds_toggle_position(2);
        led_state = !led_state;
    }
    
    // Handle 5-second UART reporting (5000ms = 5000 ticks)
    if (timer_ticks >= uart_report_ticks + 5000) {
        uart_report_ticks = timer_ticks;
        // Send frequency to UART - this is the "second task"
        printf("Current LED frequency: %d Hz\n", led_frequency);
    }
}
//============================================================================
// Button handling debounce
//============================================================================
void handle_buttons(void) {
    // Read current button states (active low)
    uint8_t btn1_current = !(PORTB.IN & PIN5_bm);
    uint8_t btn2_current = !(PORTB.IN & PIN2_bm);
    
    // Button 1 (increment frequency) debouncing
    if (btn1_current && !btn1_prev && btn1_debounce == 0) {
        // Button 1 pressed - increment frequency
        if (led_frequency < 255) { // Reasonable upper limit
            led_frequency++;
            printf("Frequency increased to: %d Hz\n", led_frequency);
        }
        btn1_debounce = 50; // Set debounce counter
    }
    if (btn1_debounce > 0) {
        btn1_debounce--;
    }
    btn1_prev = btn1_current;
    
    // Button 2 (decrement frequency) debouncing  
    if (btn2_current && !btn2_prev && btn2_debounce == 0) {
        // Button 2 pressed - decrement frequency (minimum 1 Hz)
        if (led_frequency > 1) {
            led_frequency--;
            printf("Frequency decreased to: %d Hz\n", led_frequency);
        }
        btn2_debounce = 50; // Set debounce counter
    }
    if (btn2_debounce > 0) {
        btn2_debounce--;
    }
    btn2_prev = btn2_current;
}
// ****************************************************************************

int main(void) {
  uint32_t F_CLK_PER = F_CPU;
  init_cpu();
  init_led();
  init_button();
  
  /* Initialize UART stdio on USART3 @ 9600 8N1 */
  /* (uart_init now automatically enables RX interrupt for unified API) */
  uart_init(3, 9600, F_CLK_PER, NULL);

  // Initialize Timer for 1ms interrupts
  TCA0_Initialize();
  
  // Set timer period for 1ms interrupt at 16MHz with DIV4 prescaler
  // Timer frequency = 16MHz / 4 = 4MHz
  // For 1ms: 4MHz / 1000 = 4000 ticks
  TCA0.SINGLE.PER = 3999; // 4000 - 1 (0-based count)
  
  // Register timer callback
  TCA0_OverflowCallbackRegister(timer_callback);
  
  // Enable only overflow interrupt
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
  
  // Start the timer
  TCA0_Start();
  
  // Turn off all LEDs initially except LED 2 will be controlled by timer
  PORTD.OUTCLR = 0xFF;

  sei(); // Enable global Interrupts
  printf("LED Frequency Controller Initialized.\n");
  printf("BTN1 = Increment Frequency, BTN2 = Decrement Frequency\n");
  printf("Initial frequency: %d Hz\n", led_frequency);

  while (1) {
    // Main loop can be empty or handle other non-time-critical tasks
    // All timing-critical operations are handled in timer interrupt
    
  }

  /* not reached */
  return 0;
}
