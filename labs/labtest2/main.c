/**
 * @file main.c
 * @author Arturo Salinas
 * @date 2025-09-21
 * @brief Lab Test 2
 */
#define F_CPU 16000000UL // 16 MHz clock speed
#define __AVR_AVR128DB48__
#include "include/cpu.h"
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

// Global variables for time keeping
volatile rtc_time_t current_time = {0, 0, 0};
volatile rtc_time_t countdown_time = {0, 0, 0};
volatile bool countdown_set = false;
volatile bool countdown_finished = false;
volatile bool countdown_paused = false;
volatile uint32_t rtc_interrupt_count = 0;

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

//********************************
// LED Initialization
// Set up 8 LEDs
//********************************
void init_led() {
  PORTD.DIRSET = 0xFF;    // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF;    // all off
  PORTB.DIRSET = PIN3_bm; // configure PB3 as output
  PORTB.OUTSET = PIN3_bm; // drive high = LED off
  // PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
  // PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
}

//*********************************
// Button Initialization
// Configure 1 Button at Pin B5 for polling
// ********************************
void init_button() {
  PORTB.DIRCLR = PIN5_bm;            // External button - input
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Enable pull-up only (no interrupts)
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
  if (button_counter >= 100) {
    button_pushed = true;
    button_counter = 0;
  }

  // Increment 10ms tick counter
  tca_tick_counter++;

  // Handle LED blinking when countdown finished (10Hz for 5 seconds)
  if (countdown_finished && !countdown_blink_done) {
    countdown_blink_counter++;
    led_blink_counter++;

    if (led_blink_counter >= 5) { // 50ms period for 10Hz
      led_blink_counter = 0;
      PORTD.OUTTGL = 0xFF; // Toggle all LEDs
    }

    // Stop blinking after 5 seconds (500 * 10ms)
    if (countdown_blink_counter >= 500) {
      countdown_blink_done = true;
      PORTD.OUTCLR = 0xFF; // Turn off all LEDs
    }
  }

  // Handle LED binary display (5 seconds each for hours and minutes)
  if (!countdown_finished || countdown_blink_done) {
    led_display_counter++;

    if (led_display_counter < 500) { // First 5 seconds - display hours
      display_hours = true;
      PORTB.OUTCLR = PIN3_bm; // PB3 ON (active low) to indicate hours
      // Display hours 0-12 in binary across LSB of PORTD (bits 0-3 for 0-15
      // range)
      uint8_t hours_12 = current_time.hours % 12; // Convert to 12-hour format
      PORTD.OUT = (PORTD.OUT & 0xF0) | (hours_12 & 0x0F); // Use bits 0-3
    } else if (led_display_counter < 1000) { // Next 5 seconds - display minutes
      display_hours = false;
      PORTB.OUTSET = PIN3_bm; // PB3 OFF (active low) to indicate minutes
      // Display minutes 0-59 in binary across full PORTD (bits 0-7)
      PORTD.OUT = current_time.minutes &
                  0xFF; // Use all 8 bits (0-255 range, perfect for 0-59)
    } else {
      led_display_counter = 0; // Reset cycle
    }
  }

  // Handle periodic status display (every 200 ticks = 2 seconds)
  status_display_counter++;
  if (status_display_counter >= 200) { // 2 seconds
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

  // Handle countdown logic
  if (countdown_set && !countdown_paused && !countdown_finished) {
    // Decrement countdown time
    if (countdown_time.seconds > 0) {
      countdown_time.seconds--;
    } else if (countdown_time.minutes > 0) {
      countdown_time.minutes--;
      countdown_time.seconds = 59;
    } else {
      // Countdown reached 00:00
      countdown_finished = true;
      countdown_blink_done = false;
      countdown_blink_counter = 0;
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
//*****************************************************************************
// Countdown Initializer
// When a button press activates, copy the current time and start down counting
// until 0. The countdown is only minutes and seconds.
// ****************************************************************************
void init_countdown() {
  countdown_time.minutes = current_time.minutes;
  countdown_time.seconds = current_time.seconds;
  countdown_set = true;
  countdown_finished = false;
  countdown_paused = false;
  countdown_blink_done = false;
  countdown_blink_counter = 0;
}

//*****************************************************************************
// Countdown Control Functions
// ****************************************************************************
void pause_countdown() {
  if (countdown_set && !countdown_finished && !countdown_paused &&
      (countdown_time.minutes > 0 || countdown_time.seconds > 0)) {
    countdown_paused = true;
    aos_printf("COUNTDOWN PAUSED at %02d:%02d (press 'r' to resume)\r\n",
               countdown_time.minutes, countdown_time.seconds);
  } else if (countdown_paused) {
    aos_send("Countdown already paused\r\n");
  } else if (countdown_finished) {
    aos_send("Cannot pause - countdown finished\r\n");
  } else {
    aos_send("No active countdown to pause\r\n");
  }
}

void resume_countdown() {
  if (countdown_set && countdown_paused && !countdown_finished &&
      (countdown_time.minutes > 0 || countdown_time.seconds > 0)) {
    countdown_paused = false;
    aos_printf("COUNTDOWN RESUMED at %02d:%02d (press 'p' to pause)\r\n",
               countdown_time.minutes, countdown_time.seconds);
  } else if (!countdown_paused && countdown_set && !countdown_finished) {
    aos_send("Countdown already running\r\n");
  } else if (countdown_finished) {
    aos_send("Cannot resume - countdown finished\r\n");
  } else {
    aos_send("No paused countdown to resume\r\n");
  }
}

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
    if (button_pushed && !(PORTB.IN & PIN5_bm)) {
      aos_printf("\r\nButton Pressed! Countdown Started!\r\n");
      aos_printf("Starting countdown from: %02d:%02d\r\n", current_time.minutes,
                 current_time.seconds);
      init_countdown();
      button_pushed = false; // Reset flag after handling
    }

    // Display periodic status including countdown
    if (display_status_flag) {
      display_status_flag = false;
      aos_send("\r\n=== AOS System Status ===\r\n");
      ui_display_time();

      // Show countdown status
      if (countdown_set) {
        if (countdown_finished) {
          aos_send(
              "Countdown: FINISHED (00:00) - Press B5 for new countdown\r\n");
        } else {
          aos_printf("Countdown: %02d:%02d ", countdown_time.minutes,
                     countdown_time.seconds);
          if (countdown_paused) {
            aos_send("- PAUSED (press 'r' to resume)\r\n");
          } else {
            aos_send("- COUNTING DOWN (press 'p' to pause)\r\n");
          }
        }
      } else {
        aos_send("Countdown: INACTIVE - Press button B5 to start\r\n");
      }

      aos_send("AOS> ");
    }
  }

  return 0;
}
