/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief Digital Alarm Clock Using RTC
 *
 * Features:
 * - Maintain current time using RTC
 * - Receive time and alarm settings via UART from user
 * - Trigger alarm (LED blink) when set time is reached
 * - Periodically display current time and alarm status via UART
 *
 * Commands:
 * - SET HH:MM:SS   → Set current time
 * - ALARM HH:MM:SS → Set alarm time
 * - SHOW           → Display current time and alarm status
 */
#define __AVR_AVR128DB48__
#define F_CPU 16000000UL // 16 MHz clock speed
#include "include/uart.h"
#include "include/ui.h"
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define BAUD_RATE 9600

// Global variables for time keeping
volatile rtc_time_t current_time = {0, 0, 0};
volatile rtc_time_t alarm_time = {0, 0, 0};
volatile bool alarm_set = false;
volatile bool alarm_triggered = false;
volatile uint32_t rtc_interrupt_count = 0;

// Timer-based counters for non-blocking operation
volatile uint16_t tca_tick_counter = 0;
volatile uint16_t led_blink_counter = 0;
volatile uint16_t status_display_counter = 0;
volatile bool display_status_flag = false;

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
  // Clear interrupt flag
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;

  // Increment 10ms tick counter
  tca_tick_counter++;

  // Handle LED blinking for alarm (every 50 ticks = 500ms)
  if (alarm_triggered) {
    led_blink_counter++;
    if (led_blink_counter >= 50) { // 500ms
      led_blink_counter = 0;
      PORTB.OUTTGL = PIN3_bm;
    }
  } else {
    led_blink_counter = 0;
    PORTB.OUTSET = PIN3_bm; // Turn off LEDs when no alarm
  }

  // Handle periodic status display (every 3000 ticks = 30 seconds)
  status_display_counter++;
  if (status_display_counter >= 3000) { // 30 seconds
    status_display_counter = 0;
    // Set flag for main loop to display status
    display_status_flag = true;
  }
}

//*********************************************************************
// CPU Initialization
//*********************************************************************
//
void CLOCK_XOSCHF_crystal_init(void) {
  /* Enable crystal oscillator
   * with frequency range 16Mhz and 4K cycles start-up time
   */
  ccp_write_io((uint8_t *)&CLKCTRL.XOSCHFCTRLA,
               CLKCTRL_RUNSTDBY_bm | CLKCTRL_CSUTHF_4K_gc |
                   CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_SELHF_XTAL_gc |
                   CLKCTRL_ENABLE_bm);

  // Confirm the crystal oscillator start-up
  while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm)) {
    ; // Do nothing
  }

  // set the main clock to use XOSCH as source and enable the CLKOUT pin
  ccp_write_io((uint8_t *)&CLKCTRL.MCLKCTRLA,
               CLKCTRL_CLKSEL_EXTCLK_gc | CLKCTRL_CLKOUT_bm);

  // Wait for system oscillator chaning to complete
  while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
    ; // Do nothing
  }

  // Clear RUNSTDBY for power save when not in use
  ccp_write_io((uint8_t *)&CLKCTRL.XOSCHFCTRLA,
               CLKCTRL.XOSCHFCTRLA & ~CLKCTRL_RUNSTDBY_bm);
}
void CLOCK_CFD_CLKMAIN_init(void) {
  ccp_write_io((uint8_t *)&CLKCTRL.MCLKCTRLA,
               CLKCTRL_CFDSRC_CLKMAIN_gc | CLKCTRL_CFDEN_bm);

  ccp_write_io((uint8_t *)&CLKCTRL.MCLKINTCTRL,
               CLKCTRL_INTTYPE_bm | CLKCTRL_CFD_bm);
}

static inline void init_cpu(void) {
  // ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), (CLKCTRL_FRQSEL_16M_gc));
  CPU_CCP = CCP_IOREG_gc;                     // unlock protected IO regs
  CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_16M_gc; // internal HF osc 16 MHz
}

//*****************************************************************************
// RTC Initialization - Simplified based on example code
//*****************************************************************************
void RTC_init(void) {
  // 1. Select internal 32.768kHz oscillator (more reliable)
  RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;

  // 2. Set overflow period: 32768 ticks = 1 second
  RTC.PER = 32768;

  // 3. Enable overflow interrupt
  RTC.INTCTRL = RTC_OVF_bm;

  // 4. Enable RTC with no prescaler
  RTC.CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV1_gc;

  // 5. Global interrupts will be enabled in main()
}
// ****************************************************************************
// RTC Interrupt Service Routines - 1 second tick
// ****************************************************************************
ISR(RTC_CNT_vect) {
  // Clear interrupt flag
  RTC.INTFLAGS = RTC_OVF_bm;

  // Increment interrupt counter for debugging
  rtc_interrupt_count++;

  // With internal 32kHz oscillator and PER=32768, we get exactly 1 second
  // interrupts Increment seconds directly
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

  // Check for alarm match
  if (alarm_set && current_time.hours == alarm_time.hours &&
      current_time.minutes == alarm_time.minutes &&
      current_time.seconds == alarm_time.seconds) {
    alarm_triggered = true;
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
// Main Function - Digital Alarm Clock (Non-blocking, Interrupt-driven)
// ****************************************************************************
int main(void) {
  uint32_t F_CLK_PER = F_CPU;

  // Initialize system components
  CLOCK_XOSCHF_crystal_init();
  init_led();

  // Initialize UI command processing system
  ui_init();

  // Initialize UART for command interface
  uart_init(3, 9600, F_CLK_PER, NULL);

  // Initialize TCA0 timer for periodic tasks
  init_tca0();

  // Initialize RTC for timekeeping
  RTC_init();

  // Enable global interrupts
  sei();

  // Show welcome message
  ui_show_welcome();

  // Non-blocking main loop - everything handled by interrupts
  while (1) {
    // Process UART commands (non-blocking)
    ui_process_commands();

    // Check if we need to display periodic status
    if (display_status_flag) {
      display_status_flag = false;
      printf("\n--- Status Update ---\n");
      ui_display_time();
      printf("> ");
    }
  }

  return 0;
}
