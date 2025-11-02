/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief Design a simple voltmeter that measures voltages between 0-3.3V with
~0.8mV resolution.
- Connect a potentiometer to produce variable voltage at AIN8 pin (PORTE0).
- Read the analog input voltage using ADC every 1s
-  Implement using tasks – no delay calls
- Convert the ADC reading to voltage measurement
- Print the voltage to the UART console.
Note: Use the full 12-bit resolution of the ADC
Now you should be able to observe different voltage readings as you twist the
potentiometer knob.
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
volatile uint16_t status_display_counter = 0;
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
// Timer Init - TCA0 for 10ms periodic interrupts
//************************************************
void init_tca0() {
  // Configure for 10ms interrupts (100Hz)
  // F_CPU = 16MHz, DIV256 = 62.5kHz
  // For 10ms: 62500Hz / 100Hz = 625 counts
  TCA0_SINGLE_CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
  TCA0_SINGLE_EVCTRL &= ~(TCA_SINGLE_CNTAEI_bm & TCA_SINGLE_CNTBEI_bm);
  TCA0_SINGLE_PER = 625 - 1; // 10ms period
  TCA0_SINGLE_CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
  TCA0_SINGLE_INTCTRL = TCA_SINGLE_OVF_bm; // Enable overflow interrupt
}

ISR(TCA0_OVF_vect) {
  // Debounce Handling
  if (button_counter < 65535) {
    if (!(PORTB.IN & PIN2_bm) || !(PORTB.IN & PIN5_bm)) {
      button_counter++;
    }
  } else {
    button_counter = 0;
  }
  if (button_counter >= 100) {
    button_pushed = true;
    button_counter = 0;
  }

  // Main Tick counter
  tca_tick_counter++;

  // Handle periodic status display (every 500 ticks = 5 seconds)
  status_display_counter++;
  if (status_display_counter >= 500) { // 5 seconds
    status_display_counter = 0;
    // Set flag for main loop to display status
    display_status_flag = true;
  }

  // DAC Update: Output next sample from sine table at each timer interrupt
  if (dac_update) {
    // Scale the sine value by amplitude percentage
    uint32_t scaled_value = sineWave[sine_index] * amplitude_scaler;
    DAC0_setVal((uint16_t)scaled_value);

    // Move to next sample in sine table
    sine_index++;
    if (sine_index >= 64) {
      sine_index = 0;
    }
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
  init_led();
  init_button();

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

  // Show welcome message
  ui_show_welcome();

  // Start waveform generation
  dac_update = 1;

  // Main loop
  while (1) {
    // Process UART commands (non-blocking)
    ui_process_commands();

    // Display periodic status
    if (display_status_flag) {
      display_status_flag = false;
      aos_send("\r\n--- Waveform Generator Status (5s) ---\r\n");
      aos_printf("Frequency: %lu Hz | Amplitude: %d%%\r\n", freq,
                 amplitude_percent);
      aos_send("---\r\n");
    }
  }

  return 0;
}
