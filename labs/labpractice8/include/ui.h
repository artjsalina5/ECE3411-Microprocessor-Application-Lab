#ifndef UI_H_
#define UI_H_

/**
 * @file ui.h
 * @author Arturo Salinas
 * @date 2025-09-28
 * @brief User Interface module for Digital Alarm Clock
 * 
 * This module handles all user interface functionality including:
 * - UART command processing with circular buffer
 * - Command parsing and execution
 * - User interaction and feedback
 */

#include <stdint.h>
#include <stdbool.h>
#include "circularbuff.h"

//================================
// Configuration
//================================
#define CMD_BUFFER_SIZE 64
#define MAX_CMD_LENGTH 32

//================================
// RTC Time Structure (shared)
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
extern volatile bool alarm_set;
extern volatile bool alarm_triggered;
extern volatile uint32_t rtc_interrupt_count;

//================================
// UI System Interface
//================================

/**
 * @brief Initialize the UI command processing system
 * 
 * Sets up circular buffers and internal state for command processing.
 * Must be called before using any other UI functions.
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
 * @brief Display welcome message and command help
 * 
 * Shows the initial startup message with available commands.
 */
void ui_show_welcome(void);

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
bool ui_parse_time(const char* time_str, rtc_time_t* time);

/**
 * @brief Display current time and alarm status
 * 
 * Shows formatted time, alarm settings, and current status.
 */
void ui_display_time(void);

#endif /* UI_H_ */