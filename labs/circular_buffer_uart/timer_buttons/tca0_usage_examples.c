/**
 * @file tca0_usage_examples.c
 * @author Arturo Salinas
 * @date 2025-09-24
 * @brief Comprehensive TCA0 Driver Usage Examples
 * 
 * This file demonstrates the full configurability and capabilities of the
 * enhanced TCA0 driver, showing various usage patterns for different applications.
 */

#include "tca.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>

// Example callback functions
void tca_overflow_handler(void);
void tca_compare0_handler(void);
void tca_compare1_handler(void);
void tca_compare2_handler(void);

// Global variables for examples
volatile uint32_t overflow_count = 0;
volatile uint32_t compare_matches = 0;

/**
 * @brief Example 1: Basic Timer Usage - 1Hz overflow interrupt
 */
void example_basic_timer_1hz(void) {
    printf("=== Example 1: Basic Timer - 1Hz Overflow ===\n");
    
    // Initialize with default settings
    TCA0_Initialize();
    
    // Configure for ~1Hz overflow (assuming 4MHz system clock)
    // With DIV4 prescaler: 4MHz/4 = 1MHz
    // For 1Hz: period = 1000000 - 1 = 999999 (too large for 16-bit)
    // Use DIV1024: 4MHz/1024 = 3906.25Hz
    // For 1Hz: period = 3906 - 1 = 3905
    
    TCA0_SetClockSelect(TCA_CLK_DIV1024);
    TCA0_SetPeriod(3905);  // ~1Hz overflow
    
    // Register overflow callback
    TCA0_OverflowCallbackRegister(tca_overflow_handler);
    
    // Enable overflow interrupt
    TCA0_EnableSpecificInterrupt(TCA_INT_OVF);
    
    // Start timer
    TCA0_Start();
    
    printf("Timer configured for ~1Hz overflow interrupts\n");
    printf("Current frequency: %lu Hz\n", TCA0_GetFrequency());
}

/**
 * @brief Example 2: PWM Generation - 3-channel RGB LED control
 */
void example_pwm_rgb_led(void) {
    printf("=== Example 2: PWM RGB LED Control ===\n");
    
    // Initialize for 1kHz PWM with different initial duty cycles
    if (TCA0_InitializePWM(1000, 25, 50, 75)) {  // 25%, 50%, 75% duty cycles
        printf("PWM initialized successfully at 1kHz\n");
        printf("Red (Ch0): 25%%, Green (Ch1): 50%%, Blue (Ch2): 75%%\n");
    } else {
        printf("Failed to initialize PWM at 1kHz\n");
        return;
    }
    
    printf("PWM frequency: %lu Hz\n", TCA0_GetFrequency());
    
    // Demonstrate dynamic duty cycle changes
    printf("Changing duty cycles dynamically...\n");
    
    // Fade red channel from 0 to 100%
    for (int duty = 0; duty <= 100; duty += 10) {
        TCA0_SetPWMDutyCycle(TCA_CMP_CHANNEL0, duty);
        printf("Red: %d%%, Green: %d%%, Blue: %d%%\n", 
               TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL0),
               TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL1),
               TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL2));
        // In real application, add delay here
    }
}

/**
 * @brief Example 3: Frequency Generation - Multiple tone generator
 */
void example_frequency_generator(void) {
    printf("=== Example 3: Frequency Generation ===\n");
    
    // Musical note frequencies (in Hz)
    uint32_t notes[] = {261, 293, 329, 349, 392, 440, 493, 523};  // C4 to C5
    const char* note_names[] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"};
    
    // Generate each note on channel 0
    for (int i = 0; i < 8; i++) {
        if (TCA0_InitializeFrequencyGenerator(notes[i], TCA_CMP_CHANNEL0)) {
            printf("Generating %s at %lu Hz\n", note_names[i], notes[i]);
            // In real application, add delay to hear each note
        } else {
            printf("Failed to generate %s at %lu Hz\n", note_names[i], notes[i]);
        }
    }
}

/**
 * @brief Example 4: Advanced Configuration - Custom timer setup
 */
void example_advanced_configuration(void) {
    printf("=== Example 4: Advanced Custom Configuration ===\n");
    
    // Create a comprehensive custom configuration
    TCA_Config_t config = {
        .period = 10000,              // Custom period
        .compare0 = 2500,             // 25% of period
        .compare1 = 5000,             // 50% of period  
        .compare2 = 7500,             // 75% of period
        .clockSelect = TCA_CLK_DIV64, // 64 prescaler
        .waveformMode = TCA_WGMODE_DUALSLOPE,  // Dual-slope PWM
        .interrupts = TCA_INT_ALL,    // All interrupts enabled
        .runInStandby = true,         // Run in standby
        .autoLockUpdate = true,       // Auto-lock updates
        .enableCmp0 = true,           // Enable all channels
        .enableCmp1 = true,
        .enableCmp2 = true,
        .eventActionA = TCA_EVACT_RESTART,  // Event actions
        .eventActionB = TCA_EVACT_NONE,
        .countOnEventA = false,
        .countOnEventB = false
    };
    
    // Apply advanced configuration
    TCA0_InitializeAdvanced(&config);
    
    // Register all callbacks
    TCA0_OverflowCallbackRegister(tca_overflow_handler);
    TCA0_Compare0CallbackRegister(tca_compare0_handler);
    TCA0_Compare1CallbackRegister(tca_compare1_handler);
    TCA0_Compare2CallbackRegister(tca_compare2_handler);
    
    printf("Advanced configuration applied:\n");
    printf("- Dual-slope PWM mode\n");
    printf("- DIV64 prescaler\n");
    printf("- All interrupts enabled\n");
    printf("- Run in standby enabled\n");
    printf("- Period: %u, Frequency: %lu Hz\n", 
           TCA0_GetPeriod(), TCA0_GetFrequency());
    
    // Show status
    printf("Timer enabled: %s\n", TCA0_IsEnabled() ? "Yes" : "No");
    printf("Run in standby: %s\n", TCA0_IsRunInStandbyEnabled() ? "Yes" : "No");
    printf("Waveform mode: %d\n", TCA0_GetWaveformMode());
}

/**
 * @brief Example 5: Runtime Reconfiguration
 */
void example_runtime_reconfiguration(void) {
    printf("=== Example 5: Runtime Reconfiguration ===\n");
    
    // Start with basic configuration
    TCA0_Initialize();
    TCA0_Start();
    
    printf("Initial configuration:\n");
    printf("- Frequency: %lu Hz\n", TCA0_GetFrequency());
    printf("- Period: %u\n", TCA0_GetPeriod());
    printf("- Clock select: %d\n", TCA0_GetClockSelect());
    
    // Runtime frequency changes
    uint32_t new_frequencies[] = {100, 1000, 10000, 50000};
    
    for (int i = 0; i < 4; i++) {
        printf("\nChanging to %lu Hz...\n", new_frequencies[i]);
        
        if (TCA0_SetFrequency(new_frequencies[i])) {
            printf("Success! New settings:\n");
            printf("- Actual frequency: %lu Hz\n", TCA0_GetFrequency());
            printf("- Period: %u\n", TCA0_GetPeriod());
            printf("- Clock select: %d\n", TCA0_GetClockSelect());
        } else {
            printf("Failed to set frequency %lu Hz\n", new_frequencies[i]);
        }
    }
    
    // Change waveform mode at runtime
    printf("\nChanging to single-slope PWM mode...\n");
    TCA0_SetWaveformMode(TCA_WGMODE_SINGLESLOPE);
    
    // Enable PWM on all channels with different duty cycles
    TCA0_EnablePWM(TCA_CMP_CHANNEL0);
    TCA0_EnablePWM(TCA_CMP_CHANNEL1);
    TCA0_EnablePWM(TCA_CMP_CHANNEL2);
    
    TCA0_SetPWMDutyCycle(TCA_CMP_CHANNEL0, 20);
    TCA0_SetPWMDutyCycle(TCA_CMP_CHANNEL1, 60);
    TCA0_SetPWMDutyCycle(TCA_CMP_CHANNEL2, 90);
    
    printf("PWM enabled on all channels:\n");
    printf("- Ch0: %d%%\n", TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL0));
    printf("- Ch1: %d%%\n", TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL1));  
    printf("- Ch2: %d%%\n", TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL2));
}

/**
 * @brief Example 6: Precision Timing and Measurement
 */
void example_precision_timing(void) {
    printf("=== Example 6: Precision Timing ===\n");
    
    // Configure for maximum resolution (highest frequency)
    TCA0_SetClockSelect(TCA_CLK_DIV1);  // No prescaler
    TCA0_SetPeriod(0xFFFF);             // Maximum period
    
    // Enable compare interrupts for precise timing
    TCA0_SetCompare(TCA_CMP_CHANNEL0, 1000);   // 1000 ticks
    TCA0_SetCompare(TCA_CMP_CHANNEL1, 5000);   // 5000 ticks
    TCA0_SetCompare(TCA_CMP_CHANNEL2, 10000);  // 10000 ticks
    
    TCA0_EnableSpecificInterrupt(TCA_INT_ALL_CMP);
    
    TCA0_Compare0CallbackRegister(tca_compare0_handler);
    TCA0_Compare1CallbackRegister(tca_compare1_handler);
    TCA0_Compare2CallbackRegister(tca_compare2_handler);
    
    TCA0_Start();
    
    printf("Precision timing configured:\n");
    printf("- No prescaler (maximum resolution)\n");
    printf("- Compare triggers at 1000, 5000, and 10000 ticks\n");
    printf("- Timer frequency: %lu Hz\n", TCA0_GetFrequency());
    printf("- Tick resolution: %.2f µs\n", 1000000.0 / TCA0_GetSystemClockFreq());
    
    // Monitor timer status
    printf("\nTimer status monitoring:\n");
    for (int i = 0; i < 10; i++) {
        printf("Counter: %5u, At TOP: %s, At BOTTOM: %s, Flags: 0x%02X\n",
               TCA0_Read(),
               TCA0_IsAtTop() ? "Yes" : "No",
               TCA0_IsAtBottom() ? "Yes" : "No", 
               TCA0_GetInterruptFlags());
        
        // In real application, add small delay
    }
}

/**
 * @brief Example 7: Complete Reset and Reinitialize
 */
void example_reset_and_reinit(void) {
    printf("=== Example 7: Reset and Reinitialize ===\n");
    
    // Show current state
    printf("Before reset:\n");
    printf("- Enabled: %s\n", TCA0_IsEnabled() ? "Yes" : "No");
    printf("- Counter: %u\n", TCA0_Read());
    printf("- Period: %u\n", TCA0_GetPeriod());
    
    // Perform complete reset
    printf("\nPerforming complete reset...\n");
    TCA0_Reset();
    
    printf("After reset:\n");
    printf("- Enabled: %s\n", TCA0_IsEnabled() ? "Yes" : "No");
    printf("- Counter: %u\n", TCA0_Read());
    printf("- Period: %u\n", TCA0_GetPeriod());
    
    // Reinitialize with new configuration
    printf("\nReinitializing for servo control (20ms period, variable pulse)...\n");
    
    // Servo control: 50Hz (20ms period), 1-2ms pulse width
    if (TCA0_InitializePWM(50, 5, 7, 10)) {  // 1ms, 1.4ms, 2ms pulse widths
        printf("Servo control PWM initialized:\n");
        printf("- Frequency: %lu Hz (20ms period)\n", TCA0_GetFrequency());
        printf("- Servo positions: 0°, 90°, 180° (approximately)\n");
        printf("- Ch0: %d%% (%d µs)\n", TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL0), 
               (TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL0) * 20000) / 100);
        printf("- Ch1: %d%% (%d µs)\n", TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL1),
               (TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL1) * 20000) / 100);
        printf("- Ch2: %d%% (%d µs)\n", TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL2),
               (TCA0_GetPWMDutyCycle(TCA_CMP_CHANNEL2) * 20000) / 100);
    }
}

/**
 * @brief Example 8: Event System Integration
 */
void example_event_system_integration(void) {
    printf("=== Example 8: Event System Integration ===\n");
    
    // Configure for event-driven operation
    TCA_Config_t config = {
        .period = 1000,
        .compare0 = 0,
        .compare1 = 0,
        .compare2 = 0,
        .clockSelect = TCA_CLK_DIV8,
        .waveformMode = TCA_WGMODE_NORMAL,
        .interrupts = TCA_INT_OVF,
        .runInStandby = false,
        .autoLockUpdate = false,
        .enableCmp0 = false,
        .enableCmp1 = false,
        .enableCmp2 = false,
        .eventActionA = TCA_EVACT_RESTART,  // Restart timer on event A
        .eventActionB = TCA_EVACT_UPDOWN,   // Count up/down on event B
        .countOnEventA = true,              // Enable event A counting
        .countOnEventB = false
    };
    
    TCA0_InitializeAdvanced(&config);
    
    printf("Event system configured:\n");
    printf("- Event A: Restart timer\n");
    printf("- Event B: Up/down counting\n");
    printf("- Event A counting enabled\n");
    
    // The timer will now respond to external events
    // In a real application, events would be generated by other peripherals
    
    printf("Timer ready for event-driven operation\n");
}

// =============================================================================
// CALLBACK FUNCTION IMPLEMENTATIONS
// =============================================================================

void tca_overflow_handler(void) {
    overflow_count++;
    if (overflow_count % 1000 == 0) {  // Print every 1000 overflows to avoid spam
        printf("Timer overflow #%lu\n", overflow_count);
    }
}

void tca_compare0_handler(void) {
    compare_matches++;
    printf("Compare 0 match at %u ticks\n", TCA0_Read());
}

void tca_compare1_handler(void) {
    compare_matches++;
    printf("Compare 1 match at %u ticks\n", TCA0_Read());
}

void tca_compare2_handler(void) {
    compare_matches++;
    printf("Compare 2 match at %u ticks\n", TCA0_Read());
}

/**
 * @brief Main function demonstrating all TCA0 capabilities
 */
int main(void) {
    // Enable global interrupts
    sei();
    
    printf("=== Comprehensive TCA0 Driver Examples ===\n\n");
    printf("System Clock: %lu Hz\n\n", TCA0_GetSystemClockFreq());
    
    // Run all examples
    example_basic_timer_1hz();
    example_pwm_rgb_led();
    example_frequency_generator();
    example_advanced_configuration();
    example_runtime_reconfiguration();
    example_precision_timing();
    example_reset_and_reinit();
    example_event_system_integration();
    
    printf("\n=== All TCA0 examples completed ===\n");
    printf("Total overflow interrupts: %lu\n", overflow_count);
    printf("Total compare matches: %lu\n", compare_matches);
    
    // Main loop - timer interrupts will handle events
    while (1) {
        // Application main loop
        // Timer operates independently via interrupts
    }
    
    return 0;
}