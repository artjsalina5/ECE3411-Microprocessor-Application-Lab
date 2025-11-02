/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-10-27
 * @brief Main system - Arturo's OS with DACLab module
 */
#define F_CPU 16000000UL
#define __AVR_AVR128DB48__
#include "include/cpu.h"
#include "include/dac.h"
#include "include/uart.h"
#include "include/ui.h"
#include "include/ui_dac.h"
#include "include/ui_adc.h"
#include "include/ui_eeprom.h"
#include "include/labtest3.h"
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#define BAUD_RATE 9600

// Global variables for DACLab (waveform generation)
volatile uint32_t freq = 500;          // Current frequency in Hz (10-1000)
volatile uint16_t sine_index = 0;      // Current position in sine table
volatile int amplitude_percent = 100;  // Current amplitude (10-100%)
volatile float amplitude_scaler = 1.0; // Amplitude scaling factor (0.1 to 1.0)
volatile int dac_update = 1;           // DAC update enabled by default

// Timer-based counters for 5-second status display
volatile uint32_t status_display_counter = 0;
volatile bool display_status_flag = false;

// Sine Wave Table (64 samples for one period)
extern uint16_t sineWave[64];

// Application state
static volatile bool in_daclab = false;  // true if currently in DACLab mode

//*********************************
// Sine Table Initialization
// ********************************

// In dac.h, uses math.h to generate the sine wave

//********************************
// LED Initialization
//********************************
void init_led() {
  PORTD.DIRSET = 0b10111111;    // PD0-5, PD7 as output (skip PD6 for DAC)
  PORTD.OUTCLR = 0xFF;          // all off
  PORTB.DIRSET = PIN3_bm;       // configure PB3 as output
  PORTB.OUTSET = PIN3_bm;       // drive high = LED off
  PORTC.DIRSET = PIN1_bm;       // PC1 as output
  PORTC.OUTCLR = PIN1_bm;
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
// Timer Init - TCA0
//************************************************
// TCA0 generates samples at: freq * 64 samples/second
// With DIV1 prescaler: period = F_CPU / (freq * 64) - 1
// Also used for PWM output on WO1 (PC1) for LabTest3
void init_tca0() {
  // Configure TCA0 for single slope PWM mode (needed for WO outputs)
  // For 500 Hz default: period = 16000000 / (500 * 64) - 1 = 499
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
  TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTAEI_bm & TCA_SINGLE_CNTBEI_bm);

  // Calculate period for current frequency with DIV1
  uint32_t period = F_CPU / (freq * 64UL) - 1;
  TCA0.SINGLE.PER = (uint16_t)period;

  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Enable overflow interrupt
}

// Update timer period when frequency changes
void update_tca0_frequency(uint32_t new_freq) {
  uint32_t period = F_CPU / (new_freq * 64UL) - 1;
  TCA0.SINGLE.PER = (uint16_t)period;
}

// External variables from ui_adc.c for ADCLab timing
extern volatile bool adclab_active;
extern volatile uint32_t adc_countdown;

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

  // ADCLab countdown (1ms per tick at typical ~16kHz sample rate)
  // Decrements every TCA0 tick when ADCLab is active
  if (adclab_active && adc_countdown > 0) {
    adc_countdown--;
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
  init_led();     // Initialize LEDs (PORTD, PORTB, PORTC)
  init_button();  // Initialize buttons (PB2, PB5)

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
  // PORTD.DIRSET = 0b00000001;

  // Show welcome message
  ui_show_welcome();

  // DAC will be enabled when DACLab is launched
  dac_update = 0;

  // Main loop
  while (1) {
    // Check if we're in DACLab mode
    if (dac_lab_is_active()) {
      // DACLab mode - handle interactive waveform interface
      dac_lab_process();

      // Display status every 5 seconds (but suppress during welcome)
      if (display_status_flag && !dac_lab_should_suppress_status()) {
        display_status_flag = false;
        dac_lab_status_display();
      }
    } else if (adc_lab_is_active()) {
      // ADCLab mode - handle voltmeter interface
      adc_lab_process();
    } else if (eeprom_lab_is_active()) {
      // EEPROMLab mode - handle password manager interface
      eeprom_lab_process();
    } else if (labtest3_is_active()) {
      // LabTest3 mode - handle LED animation and DAC waveform
      labtest3_process();
    } else {
      // AOS command mode - normal debugging interface
      ui_process_commands();
    }
  }

  return 0;
}