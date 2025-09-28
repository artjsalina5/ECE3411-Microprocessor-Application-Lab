/**
 * @file rtc_usage_examples.c
 * @author Arturo Salinas
 * @date 2025-09-24
 * @brief Comprehensive RTC Driver Usage Examples
 * 
 * This file demonstrates the full generality and capabilities of the enhanced
 * RTC driver, showing various usage patterns from simple to advanced.
 */

#include "rtc.h"
#include <avr/interrupt.h>
#include <stdio.h>

// Example callback functions
void rtc_overflow_handler(void);
void rtc_compare_handler(void);
void rtc_pit_handler(void);

/**
 * @brief Example 1: Basic RTC usage with default settings
 */
void example_basic_rtc(void) {
    // Initialize RTC with sensible defaults
    RTC_Initialize_Default();
    
    // Set up overflow callback for 1-second intervals
    RTC_SetOVFIsrCallback(rtc_overflow_handler);
    
    // Start the RTC
    RTC_Start();
    
    printf("Basic RTC started with default configuration\n");
}

/**
 * @brief Example 2: Custom RTC configuration for precise timing
 */
void example_custom_rtc_timing(void) {
    // Configure RTC for precise 100ms intervals
    // Using 32kHz clock: 32768 / 10 = 3276.8 ≈ 3277 for 100ms
    
    RTC_Initialize(
        3277,                           // Compare at 100ms
        0,                              // Start counter at 0
        3277,                           // Period for 100ms intervals
        RTC_CLK_OSC32K,                // Internal 32kHz clock
        RTC_INT_BOTH,                   // Both overflow and compare interrupts
        RTC_PRESCALER_DIV1 | RTC_RTCEN_bm, // No prescaler, RTC enabled
        0x00                            // No PIT
    );
    
    // Set up callbacks
    RTC_SetOVFIsrCallback(rtc_overflow_handler);
    RTC_SetCMPIsrCallback(rtc_compare_handler);
    
    printf("Custom RTC configured for 100ms precision timing\n");
}

/**
 * @brief Example 3: Advanced RTC with PIT usage
 */
void example_advanced_rtc_with_pit(void) {
    // Configure main RTC for long intervals (1 second)
    RTC_Initialize(
        32767,                          // Compare at ~1 second
        0,                              // Start counter at 0
        32767,                          // Period for 1-second intervals
        RTC_CLK_OSC32K,                // Internal 32kHz clock
        RTC_INT_OVF,                    // Only overflow interrupt for main RTC
        RTC_PRESCALER_DIV1 | RTC_RTCEN_bm, // No prescaler, RTC enabled
        0x00                            // Configure PIT separately
    );
    
    // Configure PIT for fast interrupts (every 32 cycles = ~1ms at 32kHz)
    RTC_ConfigurePIT(RTC_PIT_PERIOD_CYC32);
    RTC_EnablePIT();
    RTC_EnablePITInterrupt();
    
    // Set up callbacks
    RTC_SetOVFIsrCallback(rtc_overflow_handler);  // 1-second intervals
    RTC_SetPITIsrCallback(rtc_pit_handler);       // ~1ms intervals
    
    printf("Advanced RTC with PIT configured\n");
}

/**
 * @brief Example 4: Runtime RTC reconfiguration
 */
void example_runtime_reconfiguration(void) {
    // Start with basic configuration
    RTC_Initialize_Default();
    RTC_Start();
    
    // Simulate some runtime operation...
    printf("Initial RTC configuration running...\n");
    
    // Check current status
    if (RTC_IsEnabled()) {
        printf("RTC is currently enabled\n");
        printf("Current clock source: 0x%02X\n", RTC_GetClockSource());
        printf("Current prescaler: 0x%02X\n", RTC_GetPrescaler());
        printf("Current counter: %u\n", RTC_ReadCounter());
        printf("Current period: %u\n", RTC_ReadPeriod());
    }
    
    // Reconfigure for different timing requirements
    printf("Reconfiguring RTC...\n");
    
    // Stop RTC
    RTC_Stop();
    
    // Wait for all synchronization to complete
    while (RTC_IsBusy()) {
        // Wait for sync
    }
    
    // Change to 1kHz clock source and different prescaler
    RTC_SetClockSource(RTC_CLK_OSC1K);
    RTC_SetPrescaler(RTC_PRESCALER_DIV2);
    
    // Set new period and compare values
    RTC_WritePeriod(512);      // ~1 second with 1kHz/2 = 500Hz
    RTC_WriteCompare(256);     // ~0.5 second
    
    // Enable both interrupts
    RTC_EnableOVFInterrupt();
    RTC_EnableCMPInterrupt();
    
    // Restart RTC
    RTC_Start();
    
    printf("RTC reconfigured with new timing\n");
}

/**
 * @brief Example 5: RTC calibration and correction
 */
void example_rtc_calibration(void) {
    // Initialize RTC with correction enabled
    RTC_Initialize(
        0xFFFF,                         // Compare disabled
        0,                              // Start counter at 0
        32767,                          // Period for ~1 second
        RTC_CLK_XOSC32K,               // External 32kHz crystal (more accurate)
        RTC_INT_OVF,                    // Overflow interrupt
        RTC_PRESCALER_DIV1 | RTC_RTCEN_bm, // No prescaler, RTC enabled
        0x00                            // No PIT
    );
    
    // Enable correction for better accuracy
    RTC_SetCorrection(true);
    
    // Set calibration value (example: slight frequency adjustment)
    // Calibration value depends on crystal accuracy measurements
    RTC_SetCalibration(0x02);  // Small positive correction
    
    // Enable run in standby for continuous operation
    RTC_SetRunInStandby(true);
    
    printf("RTC configured with calibration and correction\n");
    printf("Correction enabled: %s\n", RTC_IsCorrectionEnabled() ? "Yes" : "No");
    printf("Run in standby: %s\n", RTC_IsRunInStandbyEnabled() ? "Yes" : "No");
    printf("Calibration value: 0x%02X\n", RTC_GetCalibration());
}

/**
 * @brief Example 6: Comprehensive status monitoring
 */
void example_status_monitoring(void) {
    // Initialize RTC
    RTC_Initialize_Default();
    
    // Monitor all aspects of RTC status
    printf("=== RTC Status Monitoring ===\n");
    
    // Basic status
    printf("RTC Enabled: %s\n", RTC_IsEnabled() ? "Yes" : "No");
    printf("RTC Busy: %s\n", RTC_IsBusy() ? "Yes" : "No");
    
    // Detailed synchronization status
    printf("Counter Busy: %s\n", RTC_IsCounterBusy() ? "Yes" : "No");
    printf("Period Busy: %s\n", RTC_IsPeriodBusy() ? "Yes" : "No");
    printf("Compare Busy: %s\n", RTC_IsCompareBusy() ? "Yes" : "No");
    
    // Interrupt status
    printf("OVF Interrupt Enabled: %s\n", RTC_IsOVFInterruptEnabled() ? "Yes" : "No");
    printf("CMP Interrupt Enabled: %s\n", RTC_IsCMPInterruptEnabled() ? "Yes" : "No");
    printf("PIT Interrupt Enabled: %s\n", RTC_IsPITInterruptEnabled() ? "Yes" : "No");
    
    // Current values
    printf("Current Counter: %u\n", RTC_ReadCounter());
    printf("Current Period: %u\n", RTC_ReadPeriod());
    printf("Current Compare: %u\n", RTC_ReadCompare());
    
    // Raw register values
    printf("Status Register: 0x%02X\n", RTC_GetStatus());
    printf("Interrupt Flags: 0x%02X\n", RTC_GetInterruptFlags());
    printf("PIT Status: 0x%02X\n", RTC_GetPITStatus());
    
    // Start RTC and show dynamic status
    RTC_Start();
    printf("\nRTC Started - Dynamic monitoring:\n");
    
    // Monitor for a few iterations
    for (int i = 0; i < 5; i++) {
        // Small delay simulation (in real code, this would be actual delay)
        printf("Counter: %u, Status: 0x%02X\n", RTC_ReadCounter(), RTC_GetStatus());
    }
}

/**
 * @brief Example 7: Complete RTC reset and reinitialization
 */
void example_rtc_reset(void) {
    printf("=== RTC Reset Example ===\n");
    
    // Start with some configuration
    RTC_Initialize_Default();
    RTC_Start();
    printf("Initial RTC state configured\n");
    
    // Show current state
    printf("Before reset - Counter: %u, Enabled: %s\n", 
           RTC_ReadCounter(), RTC_IsEnabled() ? "Yes" : "No");
    
    // Perform complete reset
    printf("Performing complete RTC reset...\n");
    RTC_Reset();
    
    // Show reset state
    printf("After reset - Counter: %u, Enabled: %s\n", 
           RTC_ReadCounter(), RTC_IsEnabled() ? "Yes" : "No");
    
    // Reconfigure from clean state
    printf("Reconfiguring from clean state...\n");
    RTC_Initialize(
        1000,                           // Compare at 1000 counts
        0,                              // Start at 0
        2000,                           // Period at 2000 counts
        RTC_CLK_OSC32K,                // 32kHz clock
        RTC_INT_BOTH,                   // Both interrupts
        RTC_PRESCALER_DIV4 | RTC_RTCEN_bm, // Prescaler /4, enabled
        0x00                            // No PIT
    );
    
    printf("RTC reset and reconfigured successfully\n");
}

// Example callback implementations
void rtc_overflow_handler(void) {
    static uint32_t overflow_count = 0;
    overflow_count++;
    printf("RTC Overflow #%lu\n", overflow_count);
    
    // Clear the flag (optional, as ISR handler does this)
    RTC_ClearOVFInterruptFlag();
}

void rtc_compare_handler(void) {
    static uint32_t compare_count = 0;
    compare_count++;
    printf("RTC Compare Match #%lu\n", compare_count);
    
    // Clear the flag (optional, as ISR handler does this)
    RTC_ClearCMPInterruptFlag();
}

void rtc_pit_handler(void) {
    static uint32_t pit_count = 0;
    pit_count++;
    if (pit_count % 1000 == 0) {  // Print every 1000 PIT interrupts
        printf("PIT Interrupt #%lu\n", pit_count);
    }
    
    // Clear the flag (optional, as ISR handler does this)
    RTC_ClearPITInterruptFlag();
}

/**
 * @brief Main function demonstrating all RTC capabilities
 */
int main(void) {
    // Enable global interrupts
    sei();
    
    printf("=== Comprehensive RTC Driver Examples ===\n\n");
    
    // Run all examples
    example_basic_rtc();
    example_custom_rtc_timing();
    example_advanced_rtc_with_pit();
    example_runtime_reconfiguration();
    example_rtc_calibration();
    example_status_monitoring();
    example_rtc_reset();
    
    printf("\n=== All RTC examples completed ===\n");
    
    // Main loop
    while (1) {
        // Application main loop
        // RTC interrupts will handle timing events
    }
    
    return 0;
}