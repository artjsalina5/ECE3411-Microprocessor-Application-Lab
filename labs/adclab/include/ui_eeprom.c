#include "ui_eeprom.h"
#include "ui.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>

//================================
// EEPROMLab State Variables
//================================

static volatile bool eepromlab_active = false;
static volatile uint8_t stored_password[PASSWORD_LENGTH] = {0};
static volatile uint8_t input_password[PASSWORD_LENGTH] = {0};
static volatile uint8_t input_index = 0;
static volatile uint8_t lab_mode = 0;  // 0=verify, 1=update, 2=success

/**
 * Lab modes:
 * 0 = Verification mode - user enters password to verify
 * 1 = Update mode - user enters new password (button activated)
 * 2 = Success mode - password verified, LED toggling
 */

//================================
// External Functions
//================================

extern void aos_send(const char *str);
extern void aos_printf(const char *format, ...);

//================================
// EEPROM Read/Write Functions
//================================

/**
 * @brief Read 4-byte password from EEPROM
 * 
 * @param buffer Pointer to 4-byte buffer to store password
 * @param addr Starting EEPROM address
 */
static void eeprom_read_password(uint8_t *buffer, uint8_t addr) {
  eeprom_read_block(buffer, (const void *)addr, PASSWORD_LENGTH);
}

/**
 * @brief Write 4-byte password to EEPROM
 * 
 * @param buffer Pointer to 4-byte password buffer
 * @param addr Starting EEPROM address
 */
static void eeprom_write_password(const uint8_t *buffer, uint8_t addr) {
  eeprom_write_block(buffer, (void *)addr, PASSWORD_LENGTH);
}

//================================
// LED Control Functions
//================================

/**
 * @brief Initialize LED on PB3
 */
static void init_led(void) {
  PORTB.DIRSET = PIN3_bm;  // PB3 as output
  PORTB.OUTSET = PIN3_bm;  // LED off initially (active low)
}

/**
 * @brief Turn on LED (PB3) - OUTCLR because active low
 */
static void led_on(void) {
  PORTB.OUTCLR = PIN3_bm;
}

/**
 * @brief Turn off LED (PB3)
 */
static void led_off(void) {
  PORTB.OUTSET = PIN3_bm;
}

/**
 * @brief Toggle LED for visual feedback
 */
static void led_toggle(void) {
  PORTB.OUTTGL = PIN3_bm;
}

/**
 * @brief Blink LED 4 times with 250ms on/off intervals
 * 
 * Used for password denial feedback
 * Each transition is 250ms: ON 250ms, OFF 250ms, repeat 4x
 */
static void led_blink_4x(void) {
  for (uint8_t i = 0; i < 4; i++) {
    PORTB.OUTTGL = PIN3_bm;
    _delay_ms(250);
    PORTB.OUTTGL = PIN3_bm;
    _delay_ms(250);
  }
}

//================================
// Button Monitoring
//================================

/**
 * @brief Check if PB5 button is pressed
 * 
 * @return true if button pressed (active low), false otherwise
 */
static bool button_pb5_pressed(void) {
  // PB5 is configured as input with pull-up (in port init)
  // Active low: button pressed when PIN reads as 0
  return !(PORTB.IN & PIN5_bm);
}

/**
 * @brief Initialize button on PB5
 */
static void init_button(void) {
  PORTB.DIRCLR = PIN5_bm;             // PB5 as input
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm;  // Enable pull-up
}

//================================
// Password Verification Logic
//================================

// Note: Passwords are compared using strcmp() in eeprom_lab_process()

/**
 * @brief Handle successful password verification
 */
static void on_password_verified(void) {
  aos_send("\r\nPassword Correct\r\n");
  // Keep LED on and toggle it periodically in main loop
  input_index = 0;
  memset((void*)input_password, 0, PASSWORD_LENGTH);
}

/**
 * @brief Handle failed password verification
 */
static void on_password_denied(void) {
  aos_send("\r\nPassword Incorrect Try Again!!\r\n");
  // Blink LED 4 times like reference code
  led_blink_4x();
  
  // Reset input for next attempt
  input_index = 0;
  memset((void*)input_password, 0, PASSWORD_LENGTH);
  aos_send("Enter Password: ");
}

//================================
// EEPROMLab Public Functions
//================================

void eeprom_lab_init(void) {
  eepromlab_active = true;
  lab_mode = 0;  // Start in verification mode
  
  // Initialize hardware
  init_led();
  init_button();
  
  // Read stored password from EEPROM
  eeprom_read_password((uint8_t*)stored_password, 
                       EEPROM_CURRENT_PASSWORD_ADDR);
  
  // Display stored password for debugging (Step 1)
  aos_send("\r\nStored Password: ");
  for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
    aos_printf("%c", stored_password[i]);
  }
  aos_send("\r\n");
  
  // Initialize input buffer
  input_index = 0;
  memset((void*)input_password, 0, PASSWORD_LENGTH);
}

void eeprom_lab_process(void) {
  // Non-blocking process - handle one character at a time
  
  char ch;
  if (uart_receive_char(&ch)) {
    uart_send_char(ch);
    
    // Check for EXIT command (case-insensitive 'E')
    if (ch == 'e' || ch == 'E') {
      aos_send("\r\n");
      eeprom_lab_exit();
      return;
    }
    
    // Enter key: process the entered password
    if (ch == '\r' || ch == '\n') {
      if (input_index == 0) return;  // Ignore empty input
      
      input_password[input_index] = '\0';  // Null terminate
      input_index = 0;
      
      // Compare passwords as strings
      if (lab_mode == 0) {
        // Verification mode
        if (strcmp((const char*)input_password, (const char*)stored_password) == 0) {
          on_password_verified();
          // Enter success mode (mode 2) where LED toggles
          lab_mode = 2;
        } else {
          on_password_denied();
          aos_send("Enter Password: ");
        }
      } else if (lab_mode == 1) {
        // Update mode - save new password to EEPROM
        eeprom_write_password((uint8_t*)input_password, 
                             EEPROM_CURRENT_PASSWORD_ADDR);
        _delay_ms(500);
        
        // Update stored copy from EEPROM
        eeprom_read_password((uint8_t*)stored_password, 
                            EEPROM_CURRENT_PASSWORD_ADDR);
        
        aos_send("\r\nPassword Updated and Added to EEPROM\r\n");
        aos_send("Enter Password: ");
        
        // Turn off LED after update
        led_off();
        
        // Return to verification mode
        lab_mode = 0;
      }
      
      aos_send("\r\n");
      return;
    }
    
    // Regular character input (build password)
    if (ch >= 32 && ch <= 126) {  // Printable characters
      if (input_index < PASSWORD_LENGTH) {
        input_password[input_index] = (uint8_t)ch;
        input_index++;
      }
    }
  }
  
  // Handle button press for mode switching (PB5)
  static uint8_t button_count = 0;
  if (button_pb5_pressed()) {
    button_count++;
    if (button_count > 10) {  // Debounce threshold
      if (lab_mode == 0 || lab_mode == 2) {
        // Switch to update mode
        lab_mode = 1;
        input_index = 0;
        memset((void*)input_password, 0, PASSWORD_LENGTH);
        aos_send("\r\nChange Password: ");
        button_count = 0;
      }
    }
  } else {
    button_count = 0;
  }
  
  // In success mode (mode 2), toggle LED non-blocking for smooth integration
  // This counter-based approach doesn't block AOS command processing
  if (lab_mode == 2) {
    static uint32_t toggle_counter = 0;
    toggle_counter++;
    
    // Toggle approximately every 1 second at main loop speed
    // Threshold tuned for smooth visual feedback without blocking
    if (toggle_counter > 65535) {  // ~1 second at typical loop speed
      led_toggle();
      toggle_counter = 0;
    }
  }
}

void eeprom_lab_show_welcome(void) {
  aos_send("\r\nPassword Collected From EEPROM\r\n");
  aos_send("Enter Password: ");
  
  _delay_ms(100);
}

void eeprom_lab_exit(void) {
  // Disable EEPROM operations
  eepromlab_active = false;
  
  // Turn off LED
  led_off();
  
  aos_send("\r\nExiting EEPROMLab, returning to AOS...\r\n\r\n");
  
  _delay_ms(500);
}

bool eeprom_lab_is_active(void) {
  return eepromlab_active;
}
