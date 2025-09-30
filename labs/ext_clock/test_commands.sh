#!/bin/bash

# Test script for Digital Alarm Clock
# This script demonstrates the UART command interface

echo "=== Digital Alarm Clock Test Script ==="
echo "This script shows example commands for testing the alarm clock"
echo ""

# Function to send command via serial (you would adapt this for your setup)
send_command() {
    echo "Command: $1"
    echo "Expected response: $2"
    echo "---"
}

echo "1. Initialize and show help"
send_command "HELP" "List of available commands"

echo ""
echo "2. Set current time to 10:30:00"
send_command "SET 10:30:00" "Time set to 10:30:00"

echo ""
echo "3. Set alarm for 10:30:30 (30 seconds later)"
send_command "ALARM 10:30:30" "Alarm set to 10:30:30"

echo ""
echo "4. Check current status"
send_command "SHOW" "Current Time: 10:30:xx, Alarm Set: 10:30:30"

echo ""
echo "5. Wait for alarm to trigger (LEDs should start blinking)"
echo "Expected: All 8 LEDs on PORTD will blink at 2Hz when alarm triggers"

echo ""
echo "6. Stop the alarm"
send_command "STOP" "Alarm stopped (LEDs turn off)"

echo ""
echo "=== Test Scenarios ==="
echo ""

echo "Test Case 1: Valid time formats"
send_command "SET 23:59:59" "Should accept maximum valid time"
send_command "SET 00:00:00" "Should accept minimum valid time"

echo ""
echo "Test Case 2: Invalid time formats"
send_command "SET 25:00:00" "Should reject (hours > 23)"
send_command "SET 12:60:00" "Should reject (minutes > 59)"
send_command "SET 12:30:60" "Should reject (seconds > 59)"
send_command "SET 12:30" "Should reject (missing seconds)"

echo ""
echo "Test Case 3: Alarm functionality"
send_command "SET 14:25:00" "Set current time"
send_command "ALARM 14:25:05" "Set alarm 5 seconds in future"
echo "Wait 5 seconds... LEDs should start blinking"
send_command "STOP" "Stop alarm"

echo ""
echo "Test Case 4: Midnight rollover"
send_command "SET 23:59:58" "Set time near midnight"
echo "Wait 2 seconds... should rollover to 00:00:00"
send_command "SHOW" "Should show 00:00:00"

echo ""
echo "=== Serial Communication Setup ==="
echo "To connect to the AVR device:"
echo "1. Connect USB-to-Serial adapter to USART3 pins"
echo "2. Configure terminal: 9600 baud, 8N1"
echo "3. Use commands like:"
echo "   minicom -D /dev/ttyUSB0 -b 9600"
echo "   or"
echo "   screen /dev/ttyUSB0 9600"
echo ""

echo "=== Hardware Connection ==="
echo "USART3 (AVR128DB48):"
echo "  TX: PB0 (Pin 40)"
echo "  RX: PB1 (Pin 39)"
echo ""
echo "LEDs (for alarm indication):"
echo "  PD0-PD7 (Pins 31-38)"
echo ""
echo "RTC Crystal:"
echo "  32.768 kHz crystal on XOSC32K pins"
echo ""

echo "Script complete. Use these commands to test your alarm clock!"