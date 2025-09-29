/**
 * @file ui.c
 * @author Arturo Salinas
 * @date 2025-09-28
 * @brief User Interface implementation for Digital Alarm Clock
 */

#include "ui.h"
#include "uart.h"
#include "circularbuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>

//================================
// Internal State
//================================
static uint8_t cmd_line_storage[CMD_BUFFER_SIZE];
static cbuf_handle_t cmd_line_buffer = NULL;
static char current_cmd_line[MAX_CMD_LENGTH];
static uint8_t current_cmd_index = 0;

//================================
// Command Structure
//================================
typedef struct {
  const char* name;
  void (*handler)(const char* params);
  const char* help_text;
} command_t;

//================================
// Forward Declarations
//================================
static void cmd_set_time(const char* params);
static void cmd_set_alarm(const char* params);
static void cmd_show_status(const char* params);
static void cmd_stop_alarm(const char* params);
static void cmd_debug_info(const char* params);
static void cmd_help(const char* params);

static void queue_command_line(const char* cmd_line);
static void collect_uart_input(void);
static void execute_next_command(void);

//================================
// Command Table
//================================
static const command_t commands[] = {
  {"SET",   cmd_set_time,   "SET HH:MM:SS   - Set current time"},
  {"ALARM", cmd_set_alarm,  "ALARM HH:MM:SS - Set alarm time"},
  {"SHOW",  cmd_show_status,"SHOW           - Display current time and alarm"},
  {"STOP",  cmd_stop_alarm, "STOP           - Stop current alarm"},
  {"DEBUG", cmd_debug_info, "DEBUG          - Show RTC debug info"},
  {"HELP",  cmd_help,       "HELP           - Show this help"},
  {NULL, NULL, NULL} // End marker
};

//================================
// Public Interface Implementation
//================================

void ui_init(void) {
  cmd_line_buffer = circular_buf_init(cmd_line_storage, CMD_BUFFER_SIZE);
  current_cmd_index = 0;
}

void ui_process_commands(void) {
  // Collect any new input from UART
  collect_uart_input();
  
  // Execute any queued commands
  execute_next_command();
}

void ui_show_welcome(void) {
  printf("\n=== Digital Alarm Clock ===\n");
  printf("RTC-based alarm clock initialized\n");
  printf("Available commands:\n");
  for (const command_t* cmd = commands; cmd->name; cmd++) {
    printf("  %s\n", cmd->help_text);
  }
  printf("\n> ");
}

bool ui_parse_time(const char* time_str, rtc_time_t* time) {
  int h, m, s;
  if (sscanf(time_str, "%d:%d:%d", &h, &m, &s) == 3) {
    if (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60) {
      time->hours = h;
      time->minutes = m;
      time->seconds = s;
      return true;
    }
  }
  return false;
}

void ui_display_time(void) {
  printf("Current Time: %02d:%02d:%02d\n", 
         current_time.hours, current_time.minutes, current_time.seconds);
  
  if (alarm_set) {
    printf("Alarm Set: %02d:%02d:%02d", 
           alarm_time.hours, alarm_time.minutes, alarm_time.seconds);
    if (alarm_triggered) {
      printf(" [TRIGGERED!]");
    }
    printf("\n");
  } else {
    printf("No alarm set\n");
  }
  
  // Add status line
  if (alarm_triggered) {
    printf("Status: Alarming!!!!\n");
  } else {
    printf("Status: Waiting...\n");
  }
}

//================================
// Internal Helper Functions
//================================

static void queue_command_line(const char* cmd_line) {
  // Add each character of the command line to the circular buffer
  for (const char* p = cmd_line; *p && !circular_buf_full(cmd_line_buffer); p++) {
    circular_buf_try_put(cmd_line_buffer, (uint8_t)*p);
  }
  // Add null terminator
  if (!circular_buf_full(cmd_line_buffer)) {
    circular_buf_try_put(cmd_line_buffer, 0);
  }
}

static void collect_uart_input(void) {
  char ch;
  
  while (uart_receive_char(&ch)) {
    // Echo character
    printf("%c", ch);
    
    if (ch == '\n' || ch == '\r') {
      if (current_cmd_index > 0) {
        current_cmd_line[current_cmd_index] = '\0';
        queue_command_line(current_cmd_line);
        current_cmd_index = 0;
        printf("\n> ");
      }
    }
    else if (ch == 8 || ch == 127) { // Backspace or DEL
      if (current_cmd_index > 0) {
        current_cmd_index--;
        printf("\b \b");
      }
    }
    else if (ch >= 32 && ch <= 126) { // Printable characters
      if (current_cmd_index < MAX_CMD_LENGTH - 1) {
        current_cmd_line[current_cmd_index++] = ch;
      }
    }
  }
}

static void execute_next_command(void) {
  if (circular_buf_empty(cmd_line_buffer)) {
    return;
  }
  
  // Extract command line from circular buffer
  char cmd_line[MAX_CMD_LENGTH];
  uint8_t i = 0;
  uint8_t ch;
  
  // Read until null terminator or buffer empty
  while (i < MAX_CMD_LENGTH - 1 && circular_buf_get(cmd_line_buffer, &ch) == 0) {
    if (ch == 0) break; // End of command line
    cmd_line[i++] = ch;
  }
  cmd_line[i] = '\0';
  
  if (i == 0) return; // Empty command
  
  // Parse command and parameters
  char cmd_name[16];
  char params[20];
  int num_parsed = sscanf(cmd_line, "%15s %19s", cmd_name, params);
  
  if (num_parsed < 1) return;
  
  // Convert command to uppercase
  for (int j = 0; cmd_name[j]; j++) {
    if (cmd_name[j] >= 'a' && cmd_name[j] <= 'z') {
      cmd_name[j] = cmd_name[j] - 'a' + 'A';
    }
  }
  
  // Find and execute command
  for (const command_t* cmd = commands; cmd->name; cmd++) {
    if (strcmp(cmd_name, cmd->name) == 0) {
      cmd->handler((num_parsed > 1) ? params : "");
      return;
    }
  }
  
  // Unknown command
  printf("Unknown command: %s\n", cmd_name);
  printf("Type HELP for available commands\n");
}

//================================
// Command Handler Implementations
//================================

static void cmd_set_time(const char* params) {
  rtc_time_t new_time;
  if (ui_parse_time(params, &new_time)) {
    cli();
    current_time = new_time;
    sei();
    printf("Time set to %02d:%02d:%02d\n", 
           new_time.hours, new_time.minutes, new_time.seconds);
  } else {
    printf("Invalid time format. Use HH:MM:SS\n");
  }
}

static void cmd_set_alarm(const char* params) {
  rtc_time_t new_alarm;
  if (ui_parse_time(params, &new_alarm)) {
    cli();
    alarm_time = new_alarm;
    alarm_set = true;
    alarm_triggered = false;
    sei();
    printf("Alarm set to %02d:%02d:%02d\n", 
           new_alarm.hours, new_alarm.minutes, new_alarm.seconds);
  } else {
    printf("Invalid time format. Use HH:MM:SS\n");
  }
}

static void cmd_show_status(const char* params) {
  (void)params; // Unused parameter
  ui_display_time();
}

static void cmd_stop_alarm(const char* params) {
  (void)params; // Unused parameter
  alarm_triggered = false;
  PORTD.OUTCLR = 0xFF; // Turn off all LEDs
  printf("Alarm stopped\n");
}

static void cmd_debug_info(const char* params) {
  (void)params; // Unused parameter
  printf("RTC Debug Info:\n");
  printf("  RTC.CNT: %u\n", RTC.CNT);
  printf("  RTC.PER: %u\n", RTC.PER);
  printf("  RTC.CTRLA: 0x%02X\n", RTC.CTRLA);
  printf("  RTC.STATUS: 0x%02X\n", RTC.STATUS);
  printf("  RTC.INTCTRL: 0x%02X\n", RTC.INTCTRL);
  printf("  RTC.INTFLAGS: 0x%02X\n", RTC.INTFLAGS);
  printf("  RTC.CLKSEL: 0x%02X\n", RTC.CLKSEL);
  printf("  Interrupt count: %lu\n", rtc_interrupt_count);
  printf("  Commands queued: %zu\n", circular_buf_size(cmd_line_buffer));
  printf("  Clock source: %s\n", 
         (RTC.CLKSEL == RTC_CLKSEL_OSC32K_gc) ? "Internal 32kHz" : 
         (RTC.CLKSEL == RTC_CLKSEL_XOSC32K_gc) ? "External 32kHz" : "Other");
}

static void cmd_help(const char* params) {
  (void)params; // Unused parameter
  printf("Commands:\n");
  for (const command_t* cmd = commands; cmd->name; cmd++) {
    printf("  %s\n", cmd->help_text);
  }
}