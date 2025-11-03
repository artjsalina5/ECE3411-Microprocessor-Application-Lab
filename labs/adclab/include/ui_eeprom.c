#define F_CPU 16000000UL
#include "ui_eeprom.h"
#include "ui.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>

//================================
// EEPROM Variable Declaration
//================================

// Declare EEPROM storage location for password (4 bytes, no null terminator in EEPROM)
uint8_t EEMEM eeprom_stored_password[PASSWORD_LENGTH] = {'0', '0', '0', '0'};

//================================
// EEPROMLab State Variables
//================================

static volatile bool eepromlab_active = false;
static volatile uint8_t stored_password[PASSWORD_LENGTH + 1] = {0};  // +1 for null terminator
static volatile uint8_t input_password[PASSWORD_LENGTH + 1] = {0};   // +1 for null terminator
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
 * @brief Read password from EEPROM (4 bytes + null terminator)
 * 
 * @param buffer Pointer to buffer to store password (must be 5 bytes)
 */
static void eeprom_read_password(uint8_t *buffer) {
  eeprom_read_block(buffer, eeprom_stored_password, PASSWORD_LENGTH);
  buffer[PASSWORD_LENGTH] = '\0';  // Ensure null termination
}

/**
 * @brief Write password to EEPROM (4 bytes only, no null terminator)
 * 
 * @param buffer Pointer to password buffer (null-terminated string)
 */
static void eeprom_write_password(const uint8_t *buffer) {
  eeprom_update_block(buffer, eeprom_stored_password, PASSWORD_LENGTH);
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
 * @brief Blink LED 3 times with 250ms on/off intervals
 * 
 * Used for password denial feedback
 * Each transition is 250ms: ON 250ms, OFF 250ms, repeat 3x
 */
static void led_blink_3x(void) {
  for (uint8_t i = 0; i < 3; i++) {
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
  aos_send("\r\nAccess Granted\r\n");
  // Turn on LED (and keep it on - no toggling)
  led_on();
  input_index = 0;
  memset((void*)input_password, 0, PASSWORD_LENGTH + 1);
}

/**
 * @brief Handle failed password verification
 */
static void on_password_denied(void) {
  aos_send("\r\nAccess Denied\r\n");
  // Blink LED 3 times per requirement
  led_blink_3x();
  
  // Reset input for next attempt
  input_index = 0;
  memset((void*)input_password, 0, PASSWORD_LENGTH + 1);
  
  // Prompt for next attempt
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
  eeprom_read_password((uint8_t*)stored_password);
  
  // Display stored password for debugging (Step 1)
  aos_send("\r\nStored Password: ");
  aos_send((const char*)stored_password);
  aos_send("\r\n");
  
  // Initialize input buffer
  input_index = 0;
  memset((void*)input_password, 0, PASSWORD_LENGTH + 1);
}

void eeprom_lab_process(void) {
  // Non-blocking process - handle one character at a time
  
  char ch;
  if (uart_receive_char(&ch)) {
    // Echo character back
    uart_send_char(ch);
    
    // Check for EXIT command (case-insensitive 'E')
    if (ch == 'e' || ch == 'E') {
      aos_send("\r\n");
      eeprom_lab_exit();
      return;
    }
    
    // Enter key: process the entered password
    if (ch == '\r' || ch == '\n') {
      // In success mode, provide feedback even with no input
      if (lab_mode == 2 && input_index == 0) {
        aos_send("\r\nAccess already granted. Press PB5 to change password or E to exit.\r\n");
        return;
      }
      
      if (input_index == 0) return;  // Ignore empty input in other modes
      
      input_password[input_index] = '\0';  // Null terminate
      
      // Compare passwords as strings
      if (lab_mode == 0) {
        // Verification mode
        if (strcmp((const char*)input_password, (const char*)stored_password) == 0) {
          on_password_verified();
          // Enter success mode (mode 2) - LED stays on, no toggling needed
          lab_mode = 2;
        } else {
          on_password_denied();
        }
      } else if (lab_mode == 1) {
        // Update mode - save new password to EEPROM
        eeprom_write_password((uint8_t*)input_password);
        _delay_ms(100);
        
        // Update stored copy from EEPROM
        eeprom_read_password((uint8_t*)stored_password);
        
        aos_send("\r\nPassword Updated and Added to EEPROM\r\n");
        aos_send("Enter Password: ");
        
        // Turn off LED after update
        led_off();
        
        // Return to verification mode
        lab_mode = 0;
      } else if (lab_mode == 2) {
        // Success mode - already verified, acknowledge any input attempt
        aos_send("\r\nAccess already granted. Press PB5 to change password or E to exit.\r\n");
      }
      
      // Reset input buffer for next entry
      input_index = 0;
      memset((void*)input_password, 0, PASSWORD_LENGTH + 1);
      return;
    }
    
    // Regular character input (build password)
    // Only accept input in verification (0) and update (1) modes
    if ((lab_mode == 0 || lab_mode == 1) && ch >= 32 && ch <= 126) {  // Printable characters
      if (input_index < PASSWORD_LENGTH) {
        input_password[input_index] = (uint8_t)ch;
        input_index++;
      }
    }
  }
  
  // Handle button press for mode switching (PB5) - Simple polling
  static uint8_t prev_button_state = 1;  // 1 = not pressed (pulled high)
  uint8_t curr_button_state = (PORTB.IN & PIN5_bm) ? 1 : 0;
  
  // Detect falling edge (button press)
  if (prev_button_state == 1 && curr_button_state == 0) {
    if (lab_mode == 0 || lab_mode == 2) {
      // Switch to update mode
      lab_mode = 1;
      input_index = 0;
      memset((void*)input_password, 0, PASSWORD_LENGTH + 1);
      aos_send("\r\n\r\nChange Password (4 characters): ");
    }
  }
  prev_button_state = curr_button_state;
  
  // In success mode (mode 2), LED should stay ON (no toggling per requirements)
  // LED was already turned on in on_password_verified()
}

void eeprom_lab_show_welcome(void) {
  aos_send("\r\n+-----------------------------------------------------------+\r\n");
  aos_send("|                      EEPROMLab                            |\r\n");
  aos_send("|                  PASSWORD MANAGER                         |\r\n");
  aos_send("+-----------------------------------------------------------+\r\n");
  aos_send("\r\nPassword Collected From EEPROM\r\n");
  aos_send("\r\nInstructions:\r\n");
  aos_send("  - Enter 4-character password to verify access\r\n");
  aos_send("  - Press PB5 button to change password\r\n");
  aos_send("  - Type E to exit to AOS\r\n");
  aos_send("\r\nEnter Password (4 characters): ");
  
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
