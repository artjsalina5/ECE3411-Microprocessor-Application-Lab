#ifndef UI_H_
#define UI_H_

/**
 * @file ui.h
 * @author Arturo Salinas
 * @date 2025-09-30
 * @brief Arturo's Operating System - Generic Debugging Interface
 *
 * A comprehensive debugging system that provides:
 * - Register inspection and modification
 * - Memory dump capabilities
 * - System status monitoring
 * - Hardware testing functions
 * - Non-blocking UART interface
 * - Legacy RTC alarm clock functionality
 */

#include "circularbuff.h"
#include <stdbool.h>
#include <stdint.h>

//================================
// Arturo's OS Configuration
//================================
#define AOS_VERSION "v1.0"
#define CMD_BUFFER_SIZE 128
#define MAX_CMD_LENGTH 64

//================================
// RTC Time Structure (shared with main.c)
//================================
typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} rtc_time_t;

//================================
// External variables (defined in main.c)
//================================
extern volatile rtc_time_t current_time;
extern volatile rtc_time_t alarm_time;
extern volatile rtc_time_t countdown_time;
extern volatile bool countdown_set;
extern volatile bool countdown_finished;
extern volatile bool alarm_set;
extern volatile bool alarm_triggered;
extern volatile uint32_t rtc_interrupt_count;

//================================
// Arturo's OS Interface
//================================

/**
 * @brief Initialize Arturo's Operating System
 *
 * Sets up command processing system, buffers, and internal state.
 * Must be called before using any other AOS functions.
 */
void ui_init(void);

/**
 * @brief Process UART commands (non-blocking)
 *
 * This function should be called regularly from the main loop.
 * It collects input from UART, builds command lines, and executes
 * any complete commands that have been received.
 */
void ui_process_commands(void);

/**
 * @brief Display Arturo's OS boot message and help
 *
 * Shows the fancy boot screen with system information and available commands.
 */
void ui_show_welcome(void);

/**
 * @brief Provide runtime system info to UI (for accurate banner)
 * @param f_cpu_hz CPU frequency in Hz
 * @param uart_baud Baud rate configured for UART console
 */
void ui_set_system_info(uint32_t f_cpu_hz, uint32_t uart_baud);

/**
 * @brief Reprint prompt and any partially typed input (after async output)
 */
void ui_reprompt(void);

//================================
// Utility Functions
//================================

/**
 * @brief Parse time string in HH:MM:SS format
 *
 * @param time_str String containing time in HH:MM:SS format
 * @param time Pointer to rtc_time_t structure to fill
 * @return true if parsing successful, false otherwise
 */
bool ui_parse_time(const char *time_str, rtc_time_t *time);

/**
 * @brief Display current time and alarm status with emojis
 *
 * Shows formatted time, alarm settings, and current status.
 */
void ui_display_time(void);

//================================
// Debug Functions (AOS-specific)
//================================

/**
 * @brief Non-blocking printf replacement
 *
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void aos_printf(const char *format, ...);

/**
 * @brief Send string via non-blocking UART
 *
 * @param str String to send
 */
void aos_send(const char *str);

#endif /* UI_H_ */
