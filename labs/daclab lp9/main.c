/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief ADCLab implementation using AOS
 */
#define F_CPU 16000000UL
#define __AVR_AVR128DB48__
#include "include/cpu.h"
#include "include/dac.h"
#include "include/uart.h"
#include "include/ui.h"
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
// #include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define BAUD_RATE 9600

// Global variable for button pushing
volatile uint16_t button_counter = 0;
volatile bool button_pushed = false;

// Timer-based counters
volatile uint16_t tca_tick_counter = 0;
volatile uint16_t led_blink_counter = 0;
volatile uint32_t status_display_counter =
    0; // Counts actual samples for 5-second intervals
volatile bool display_status_flag = false;

// Sine Wave Table (64 samples for one period)
extern uint16_t sineWave[64];

// Global variables for DAC Operation
volatile uint32_t freq = 500;          // Current frequency in Hz (10-1000)
volatile uint16_t sine_index = 0;      // Current position in sine table
volatile int amplitude_percent = 100;  // Current amplitude (10-100%)
volatile float amplitude_scaler = 1.0; // Amplitude scaling factor (0.1 to 1.0)

// DAC update flag from timer
volatile int dac_update = 0;

//*********************************
// Sine Table Initialization
// ********************************

// In dac.h, uses math.h to generate the sine wave

//********************************
// LED Initialization
//********************************
void init_led() {
  PORTD.DIRSET = 0xFF;    // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF;    // all off
  PORTB.DIRSET = PIN3_bm; // configure PB3 as output
  PORTB.OUTSET = PIN3_bm; // drive high = LED off
  PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
  PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
}

//*********************************
// Button Initialization
// ********************************
void init_button() {
  PORTB.DIRCLR = PIN2_bm;            // Onboard button - input
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up only
  PORTB.DIRCLR = PIN5_bm;            // External button - input
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Enable pull-up only
}

//************************************************
// Timer Init - TCA0 for waveform generation
//************************************************
// TCA0 generates samples at: freq * 64 samples/second
// With DIV1 prescaler: period = F_CPU / (freq * 64) - 1
void init_tca0() {
  // Configure TCA0 for waveform generation
  // For 500 Hz default: period = 16000000 / (500 * 64) - 1 = 499
  TCA0_SINGLE_CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
  TCA0_SINGLE_EVCTRL &= ~(TCA_SINGLE_CNTAEI_bm & TCA_SINGLE_CNTBEI_bm);

  // Calculate period for current frequency with DIV1
  uint32_t period = F_CPU / (freq * 64UL) - 1;
  TCA0_SINGLE_PER = (uint16_t)period;

  TCA0_SINGLE_CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
  TCA0_SINGLE_INTCTRL = TCA_SINGLE_OVF_bm; // Enable overflow interrupt
}

// Update timer period when frequency changes
void update_tca0_frequency(uint32_t new_freq) {
  uint32_t period = F_CPU / (new_freq * 64UL) - 1;
  TCA0_SINGLE_PER = (uint16_t)period;
}

ISR(TCA0_OVF_vect) {
  // DAC Update: Output next sample from pre-scaled sine table at each timer interrupt
  if (dac_update) {
    // Use pre-computed scaled sine table
    DAC0_setVal(sine_wave_scaled[sine_index]);

    // Move to next sample in sine table
    sine_index++;
    if (sine_index >= 64) {
      sine_index = 0;
    }
  }

  // Always count samples for status display (independent of dac_update flag)
  // 5 seconds = freq * 64 * 5 samples
  // With 500 Hz default: 500 * 64 * 5 = 160,000 samples
  status_display_counter++;

  uint32_t samples_per_5_seconds = freq * 64UL * 5UL;
  if (status_display_counter >= samples_per_5_seconds) {
    status_display_counter = 0;
    display_status_flag = true;
  }

  // Clear interrupt flag
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

//*****************************************************************************
// USART Interrupt Service Routines
//*****************************************************************************
ISR(USART3_RXC_vect) {
  char receivedChar = USART3.RXDATAL;
  uart_rx_isr_handler(receivedChar);
}

ISR(USART3_DRE_vect) {
  char data_to_send;
  if (uart_tx_isr_handler(&data_to_send)) {
    USART3.TXDATAL = data_to_send;
  } else {
    USART3.CTRLA &= ~USART_DREIE_bm;
  }
}

/*
ISR(USART3_RXC_vect) {
  keypress_val = USART3.RXDATAL;
  keypress_int = keypress_val - '0';
  keypress_flag = 1;
}
*/
// ****************************************************************************
// Main Function
// ****************************************************************************
int main(void) {
  uint32_t F_CLK_PER = F_CPU;

  // Initialize system components
  CLOCK_XOSCHF_16M_init();
  // init_led();
  // init_button();

  // Initialize UI command processing system
  ui_init();

  // Initialize UART for command interface
  uart_init(3, BAUD_RATE, F_CLK_PER, NULL);
  ui_set_system_info(F_CLK_PER, BAUD_RATE);

  // Initialize DAC for waveform output
  DAC_init();

  // Compute sine wave table at startup
  sine_wave_init();

  // Initialize TCA0 timer for periodic tasks (10ms interrupts)
  init_tca0();

  // Enable global interrupts
  sei();
  aos_printf("UART Initialized at 9600 baud\r\n");
  aos_printf("DAC0 Initialized to Output at Pin D.6\n");

  aos_printf("Configured DAC0 with output enable and run in standby\n");
  aos_printf("Set DAC0 reference voltage to VDD (3.3V) with always on mode\n");
  PORTD.DIRSET = 0b00000001;

  // Show welcome message
  ui_show_welcome();

  // Start waveform generation
  dac_update = 1;

  // Main loop
  while (1) {
    // Process UART commands (non-blocking)
    ui_process_commands();

    // Display periodic status every 5 seconds
    if (display_status_flag) {
      display_status_flag = false;
      // Print status line with current frequency and amplitude
      aos_printf("\r\nSTATUS: Freq: %lu Hz, Amplitude: %d%%\r\n",
                 freq, amplitude_percent);
    }
  }

  return 0;
}
