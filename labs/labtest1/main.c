/* Sets PD1 and PD2 to blink at 2Hz and PC6 and PC7 to blink at 5Hz simultaneously. After every 2 seconds, PD1 and PD2 LEDs and PC6 and PC7 LED interchange frequencies. Knight Rider shifter moves the blinking position on Port D every second, shifting right (default) with wrapping, or left. All other LEDs remain Off*/
#define __AVR_AVR128DB48__
#define F_CPU 16000000UL
#include <avr/io.h>
// #include <avr/ioavr128db48.h>

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

static inline void init_cpu(void) {
  CPU_CCP = CCP_IOREG_gc;                     // unlock protected IO regs
  CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_16M_gc; // internal HF osc 16 MHz
}

void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
  PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
  PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
}

void init_button() {
  PORTB.DIRCLR = PIN5_bm;            // Set PB5 as input
  PORTB.DIRCLR = PIN2_bm;            // Set PB2 as input
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm; // Enable as pull-up
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm; // Enable as pull-up
}

int main(void) {
  init_cpu();
  init_led();
  init_button();

  uint16_t counter_2hz = 0;  // Counter for 2Hz (250ms half-period)
  uint16_t counter_5hz = 0;  // Counter for 5Hz (100ms half-period)
  uint16_t counter_1hz = 0;  // Counter for 1Hz (500ms half-period)
  uint16_t swap_counter = 0; // Counter for 2-second swap interval
  bool frequencies_swapped = false;

  uint8_t active_d_leds = 0b00000110; // PD1 & PD2
  uint8_t direction = 1;
  
  // Button state variables
  bool pb5_prev_state = true;
  bool pb2_pressed = false;  
  bool pb2_prev_state = true;  
  
  // Master code sequence tracking
  uint8_t master_sequence = 0; // 0=start, 1=PB5, 2=PB5+PB2, 3=PB5+PB2+PB5, 4=complete
  bool pattern_wide = false;   // start false for 2 pattern, true for 4 pattern

  while (1) {
    // Button checks
    bool pb5_current = !(PORTB.IN & PIN5_bm);  // true = pressed
    bool pb2_current = !(PORTB.IN & PIN2_bm);  // true = pressed
    
    // Master sequence tracking: PB5 → PB2 → PB5 → PB2
    if (pb5_current && !pb5_prev_state) {      // PB5 just pressed
      if (master_sequence == 0) {
        master_sequence = 1;  // First PB5
      } else if (master_sequence == 2) {
        master_sequence = 3;  // Third press (second PB5)
      } else {
        master_sequence = 0;  // Reset if wrong sequence
      }
    }
    
    if (pb2_current && !pb2_prev_state) {      // PB2 just pressed
      if (master_sequence == 1) {
        master_sequence = 2;  // Second press (first PB2)
      } else if (master_sequence == 3) {
        master_sequence = 4;  // Fourth press (second PB2)
        
        if (!pattern_wide) {
          active_d_leds = 0b00001111;  // PD1&PD0 → PD3&PD2&PD1&PD0
          pattern_wide = true;         // Set to wide mode
        } else if (pattern_wide) {
          active_d_leds = 0b00000011;  // PD3&PD2&PD1&PD0 → PD1&PD0
          pattern_wide = false;        // Set to narrow mode
        }
        master_sequence = 0;  // Reset sequence
      } else {
        master_sequence = 0;  // Reset if wrong sequence
      }
    }
    
    pb5_prev_state = pb5_current;
    pb2_prev_state = pb2_current;
    
    // Normal PB5 direction toggle
    if (pb5_current && !pb5_prev_state && master_sequence == 0) {
      direction = !direction;                   // Toggle direction
    }
    
    // Check PB2 for direction freeze
    pb2_pressed = pb2_current;
    
    // Increment counters every 10ms
    counter_2hz++;
    counter_1hz++;
    counter_5hz++;
    swap_counter++;

    // Check if 2 seconds have passed for frequency swap
    if (swap_counter >= 200) {
      frequencies_swapped = !frequencies_swapped;
      swap_counter = 0;
    }

    // Pattern shifter - every 1 second (100 * 10ms)
    if (counter_1hz >= 100 && !pb2_pressed) {
        // Turn off all Port D LEDs before shifting
        PORTD.OUT = 0;
        
        if (pattern_wide) {
          // 4-adjacent pattern shifting
          if (direction) {
              // Shift right - handle all 4-adjacent patterns with wrapping
              if (active_d_leds == 0b00001111) {       // PD3&PD2&PD1&PD0
                  active_d_leds = 0b10000111;          // PD7&PD2&PD1&PD0
              } else if (active_d_leds == 0b10000111) { // PD7&PD2&PD1&PD0
                  active_d_leds = 0b11000011;          // PD7&PD6&PD1&PD0
              } else if (active_d_leds == 0b11000011) { // PD7&PD6&PD1&PD0
                  active_d_leds = 0b11100001;          // PD7&PD6&PD5&PD0
              } else if (active_d_leds == 0b11100001) { // PD7&PD6&PD5&PD0
                  active_d_leds = 0b11110000;          // PD7&PD6&PD5&PD4
              } else if (active_d_leds == 0b11110000) { // PD7&PD6&PD5&PD4
                  active_d_leds = 0b01111000;          // PD6&PD5&PD4&PD3
              } else if (active_d_leds == 0b01111000) { // PD6&PD5&PD4&PD3
                  active_d_leds = 0b00111100;          // PD5&PD4&PD3&PD2
              } else if (active_d_leds == 0b00111100) { // PD5&PD4&PD3&PD2
                  active_d_leds = 0b00011110;          // PD4&PD3&PD2&PD1
              } else if (active_d_leds == 0b00011110) { // PD4&PD3&PD2&PD1
                  active_d_leds = 0b00001111;          // PD3&PD2&PD1&PD0
              } 
              
          } else {
              // Shift left - reverse of the above
              if (active_d_leds == 0b00001111) {       // PD3&PD2&PD1&PD0
                  active_d_leds = 0b00011110;          // PD4&PD3&PD2&PD1
              } else if (active_d_leds == 0b00011110) { // PD4&PD3&PD2&PD1
                  active_d_leds = 0b00111100;          // PD5&PD4&PD3&PD2
              } else if (active_d_leds == 0b00111100) { // PD5&PD4&PD3&PD2
                  active_d_leds = 0b01111000;          // PD6&PD5&PD4&PD3
              } else if (active_d_leds == 0b01111000) { // PD6&PD5&PD4&PD3
                  active_d_leds = 0b11110000;          // PD7&PD6&PD5&PD4
              } else if (active_d_leds == 0b11110000) { // PD7&PD6&PD5&PD4
                  active_d_leds = 0b11100001;          // PD7&PD6&PD5&PD0
              } else if (active_d_leds == 0b11100001) { // PD7&PD6&PD5&PD0
                  active_d_leds = 0b11000011;          // PD7&PD6&PD1&PD0
              } else if (active_d_leds == 0b11000011) { // PD7&PD6&PD1&PD0
                  active_d_leds = 0b10000111;          // PD7&PD2&PD1&PD0
              } else if (active_d_leds == 0b10000111) { // PD7&PD2&PD1&PD0
                  active_d_leds = 0b00001111;          // PD3&PD2&PD1&PD0
              } 
          }
        } else {
          // 2-adjacent pattern shifting
          if (direction) {
              // Shift right - handle all adjacent pairs with wrapping
              if (active_d_leds == 0b00000110) {       // PD2 & PD1
                  active_d_leds = 0b00000011;          // PD1 & PD0
              } else if (active_d_leds == 0b00000011) { // PD1 & PD0
                  active_d_leds = 0b10000001;          // PD7 & PD0 (wrap around)
              } else if (active_d_leds == 0b10000001) { // PD7 & PD0
                  active_d_leds = 0b11000000;          // PD7 & PD6
              } else if (active_d_leds == 0b11000000) { // PD7 & PD6
                  active_d_leds = 0b01100000;          // PD6 & PD5
              } else if (active_d_leds == 0b01100000) { // PD6 & PD5
                  active_d_leds = 0b00110000;          // PD5 & PD4
              } else if (active_d_leds == 0b00110000) { // PD5 & PD4
                  active_d_leds = 0b00011000;          // PD4 & PD3
              } else if (active_d_leds == 0b00011000) { // PD4 & PD3
                  active_d_leds = 0b00001100;          // PD3 & PD2
              } else if (active_d_leds == 0b00001100) { // PD3 & PD2
                  active_d_leds = 0b00000110;          // PD2 & PD1
              }
          } else {
              // Shift left - reverse of the above
              if (active_d_leds == 0b00000110) {       // PD2 & PD1
                  active_d_leds = 0b00001100;          // PD3 & PD2
              } else if (active_d_leds == 0b00001100) { // PD3 & PD2
                  active_d_leds = 0b00011000;          // PD4 & PD3
              } else if (active_d_leds == 0b00011000) { // PD4 & PD3
                  active_d_leds = 0b00110000;          // PD5 & PD4
              } else if (active_d_leds == 0b00110000) { // PD5 & PD4
                  active_d_leds = 0b01100000;          // PD6 & PD5
              } else if (active_d_leds == 0b01100000) { // PD6 & PD5
                  active_d_leds = 0b11000000;          // PD7 & PD6
              } else if (active_d_leds == 0b11000000) { // PD7 & PD6
                  active_d_leds = 0b10000001;          // PD7 & PD0 (wrap around)
              } else if (active_d_leds == 0b10000001) { // PD7 & PD0
                  active_d_leds = 0b00000011;          // PD1 & PD0
              } else if (active_d_leds == 0b00000011) { // PD1 & PD0
                  active_d_leds = 0b00000110;          // PD2 & PD1
              }
          }
        }
        counter_1hz = 0;
    }

    // Handle 2Hz blinking
    if (counter_2hz >= 25) {
      if (!frequencies_swapped) {
        PORTD.OUTTGL = active_d_leds;
      } else {
        PORTC.OUTTGL = PIN6_bm | PIN7_bm;  // PC6 & PC7 at 2Hz
      }
      counter_2hz = 0;
    }

    // Handle 5Hz blinking
    if (counter_5hz >= 10) {
      if (!frequencies_swapped) {
        PORTC.OUTTGL = PIN6_bm | PIN7_bm;  // PC6 & PC7 at 5Hz
      } else {
        PORTD.OUTTGL = active_d_leds;
      }
      counter_5hz = 0;
    }

    _delay_ms(10); // Base timing interval of 10ms
  }

  return 0;
}