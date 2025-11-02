/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief You will create a Programmable Waveform Generator that accepts
commands via USART to control DAC output waveforms. You need to connect DAC
output to the buzzer circuit. ▪ System Requirements ▪ Generate sine waveform
(verify with oscilloscope) ▪ Type F or A to update values Variable frequency
control (10Hz to 1kHz) Variable amplitude control (10% to 100% of reference
voltage) ▪ Real-time status reporting via USART every 5 seconds ▪ Non-blocking
operation using interrupts
 */
#define F_CPU 16000000UL // 16 MHz clock speed
#define __AVR_AVR128DB48__
#include "include/cpu.h"
#include "include/uart.h"
#include "include/ui.h"
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#define BAUD_RATE 9600
/* VREF start-up time */
#define VREF_STARTUP_TIME (50)
/* Mask needed to get the 2 LSb for DAC Data Register */
#define LSB_MASK (0x03)
/* Number of samples for a sine wave period */
#define SINE_PERIOD_STEPS (100)
/* Sine wave amplitude */
#define SINE_AMPLITUDE (511)
/* Sine wave DC offset */
#define SINE_DC_OFFSET (512)
/* Frequency of the sine wave */
#define SINE_FREQ (100)
/* Step delay for the loop */
#define STEP_DELAY_TIME ((1000000 / SINE_FREQ) / SINE_PERIOD_STEPS)

// Global variables for time keeping
volatile rtc_time_t current_time = {0, 0, 0};
volatile rtc_time_t countdown_time = {0, 0, 0};
volatile bool countdown_set = false;
volatile bool countdown_finished = false;
volatile bool countdown_paused = false;
volatile uint32_t rtc_interrupt_count = 0;
volatile uint8_t index = 0;

// Global variable for buzzer frequency (10Hz to 1Khz)
// and Amplitude
volatile uint16_t buzzer_frequency = 100; // Default 100Hz
volatile uint8_t buzzer_amplitude = 100;  // Default 100%

// Global variables for waveform generation
volatile uint8_t sine_index = 0;
volatile uint16_t samples_per_cycle =
    SINE_PERIOD_STEPS;                      // Use dynamic sine wave samples
volatile uint16_t timer_reload_value = 625; // Timer reload for sample rate

// Function declarations
void init_tca1_dac_timer(void);
static void DAC0_setVal(uint16_t value);

// Global variable for button pushing
volatile uint16_t button_counter = 0;
volatile bool button_pushed = false;

// Timer-based counters
volatile uint16_t tca_tick_counter = 0;
volatile uint16_t led_blink_counter = 0;
volatile uint16_t status_display_counter = 0;
volatile bool display_status_flag = false;

// LED display state variables
volatile uint16_t led_display_counter = 0;
volatile bool display_hours = true;
volatile uint16_t countdown_blink_counter = 0;
volatile bool countdown_blink_done = false;

//**********************************************************************
// Sine Table
// *********************************************************************
uint16_t sineWave[SINE_PERIOD_STEPS];

static void sineWaveInit(void) {
  uint8_t i;
  for (i = 0; i < SINE_PERIOD_STEPS; i++) {
    sineWave[i] =
        SINE_DC_OFFSET + SINE_AMPLITUDE * sin(2 * M_PI * i / SINE_PERIOD_STEPS);
  }
}
//***********************************************************
// DAC Initialization - PD6 is the DAC output pin
// **********************************************************
void DAC_init(void) {
  // Disable digital I/O on PD6 (DAC output pin) - following Microchip example
  PORTD.PIN6CTRL &= ~PORT_ISC_gm;
  PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc; // Disable digital input buffer
  PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;         // Disable pull-up resistor

  // Configure DAC0 with output enable and run in standby
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm | DAC_RUNSTDBY_bm;

  // Set reference voltage to VDD (3.3V) with always on mode
  VREF.DAC0REF = VREF_REFSEL_VDD_gc | VREF_ALWAYSON_bm;

  // Wait for VREF startup time
  _delay_us(50);
}

// DAC value setting function - following Microchip example
static void DAC0_setVal(uint16_t value) {
  // Store the two LSbs in DAC0.DATAL (shifted to bits 7:6)
  DAC0.DATAL = (value & 0x03) << 6;
  // Store the eight MSbs in DAC0.DATAH
  DAC0.DATAH = value >> 2;
}

//************************************************
// Timer Init - TCA1 for DAC sine wave sampling
// Configure TCA1 to control DAC output sample rate
//************************************************
void init_tca1_dac_timer() {
  // Calculate initial timer reload value for default frequency
  // Sample rate = frequency * samples_per_cycle
  // TCA1 frequency = 1MHz (16MHz / 16 prescaler)
  // So: reload = (1MHz / (frequency * samples_per_cycle)) - 1
  uint32_t target_sample_rate = (uint32_t)buzzer_frequency * samples_per_cycle;
  uint32_t timer_freq = 1000000UL; // 16MHz / 16 prescaler
  timer_reload_value = (timer_freq / target_sample_rate) - 1;

  // Ensure reasonable bounds
  if (timer_reload_value < 20)
    timer_reload_value = 20;
  if (timer_reload_value > 65535)
    timer_reload_value = 65535;

  // Configure TCA1 in normal mode
  TCA1.SINGLE.CTRLA = 0;                           // Disable TCA1 to configure
  TCA1.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // Normal mode
  TCA1.SINGLE.EVCTRL = 0;                          // No event control

  // Set period for desired sample rate
  TCA1.SINGLE.PER = timer_reload_value;

  // Enable overflow interrupt
  TCA1.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

  // Enable TCA1 with appropriate prescaler (DIV8 = 2MHz)
  TCA1.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm;
}

// TCA1 interrupt - generates next DAC sample from sine table
ISR(TCA1_OVF_vect) {
  // Clear interrupt flag
  TCA1.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;

  // Get next sine wave sample from the dynamically generated sine wave
  uint16_t sample = sineWave[sine_index];

  // Apply amplitude scaling (10% to 100%)
  uint32_t scaled_sample = ((uint32_t)sample * buzzer_amplitude) / 100;

  // Ensure we don't exceed 10-bit DAC range
  if (scaled_sample > 1023) {
    scaled_sample = 1023;
  }

  DAC0_setVal((uint16_t)scaled_sample);

  // Advance to next sample in sine table
  sine_index++;
  if (sine_index >= samples_per_cycle) {
    sine_index = 0;
  }
}

//************************************************
// Timer Init - TCA0 for 10ms periodic interrupts
// Configure TCA0 as a 10ms timer with interrupts
// Use a divisior of 256
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
  // Clear interrupt flag
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;

  if (button_counter < 65535) {
    if (!(PORTB.IN & PIN2_bm) || !(PORTB.IN & PIN5_bm)) {
      button_counter++;
    }
  } else {
    button_counter = 0;
  }
  if (button_counter >= 50) {
    button_pushed = true;
    button_counter = 0;
  }

  // Increment 10ms tick counter
  tca_tick_counter++;

  // Handle periodic status display (every 500 ticks = 5 seconds)
  status_display_counter++;
  if (status_display_counter >= 500) { // 5 seconds
    status_display_counter = 0;
    // Set flag for main loop to display status
    display_status_flag = true;
  }
}

//*****************************************************************************
// RTC Initialization
// Set up RTC to overflow every 1 second.
// Utilize a 1kHz oscillator and a divisor of 128
//*****************************************************************************
void RTC_init(void) {
  // 1. Select internal 1.024kHz oscillator
  RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc;

  // 2. Set overflow period: 128*(1+PER) / 1024Hz  => 1 second
  RTC.PER = 7;

  // 3. Enable overflow interrupt
  RTC.INTCTRL = RTC_OVF_bm;

  // 4. Enable RTC with 128 prescaler
  RTC.CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV128_gc;

  // 5. Global interrupts will be enabled in main()
}
// ****************************************************************************
// RTC Interrupt Service Routines
// ****************************************************************************
ISR(RTC_CNT_vect) {
  // Clear interrupt flag
  RTC.INTFLAGS = RTC_OVF_bm;

  // Increment interrupt counter for debugging
  rtc_interrupt_count++;
  PORTC.OUTTGL = PIN7_bm;

  // Increment current time
  current_time.seconds++;
  if (current_time.seconds >= 60) {
    current_time.seconds = 0;
    current_time.minutes++;

    if (current_time.minutes >= 60) {
      current_time.minutes = 0;
      current_time.hours++;

      if (current_time.hours >= 24) {
        current_time.hours = 0;
      }
    }
  }
}

// ********************************
// USART Interrupt Service Routines
// ********************************
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

// ****************************************************************************
// Main Function
// ****************************************************************************
int main(void) {
  uint32_t F_CLK_PER = F_CPU;

  // Initialize system components
  CLOCK_XOSCHF_16M_init();

  // Initialize DAC FIRST (before LED init to avoid conflicts on PD0)
  DAC_init();

  // Initialize sine wave lookup table
  sineWaveInit();

  // Initialize TCA1 timer for DAC sine wave generation
  init_tca1_dac_timer();

  // Initialize UI command processing system
  ui_init();

  // Initialize UART for command interface
  uart_init(3, BAUD_RATE, F_CLK_PER, NULL);
  ui_set_system_info(F_CLK_PER, BAUD_RATE);

  // Initialize TCA0 timer for periodic tasks
  init_tca0();

  // Initialize RTC for timekeeping
  RTC_init();

  // Enable global interrupts
  sei();

  // Show welcome message
  ui_show_welcome();
  // Main loop
  while (1) {
    // Process UART commands (non-blocking)
    ui_process_commands();

    // Display periodic status including countdown
    if (display_status_flag) {
      display_status_flag = false;
      aos_send("\r\n=== AOS System Status ===\r\n");
      aos_printf("Current Frequency: %d Hz\r\n", buzzer_frequency);
      aos_printf("Current Amplitude: %d%%\r\n", buzzer_amplitude);
      aos_send("AOS> ");
    }
  }

  return 0;
}
