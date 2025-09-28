/**
 * @file tca.h
 * @author Arturo Salinas
 * @date 2025-09-24
 * @brief Enhanced TCA0 Driver with Maximum Configurability
 * 
 * @defgroup tca0_enhanced TCA0 Enhanced Driver
 * @{
 * 
 * @brief Comprehensive TCA0 driver supporting all timer modes and configurations
 * 
 * This enhanced TCA0 driver provides maximum configurability for the AVR128DB48
 * Timer/Counter Type A (TCA) module. It supports:
 * 
 * - Normal 16-bit timer mode
 * - Dual 8-bit PWM mode (split mode)
 * - Single-slope PWM
 * - Dual-slope PWM
 * - Frequency generation
 * - Event system integration
 * - Runtime reconfiguration
 * - Comprehensive register access
 * 
 * @version TCA0 Enhanced Driver Version 3.0.0
 */

#ifndef TCA_H_INCLUDED
#define TCA_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#include "timer_interface.h"

/**
 * @brief Function pointer to callback functions for TCA interrupts
 */  
typedef void (*TCA0_cb_t)(void);

/**
 * @brief TCA clock source configuration
 */
typedef enum {
    TCA_CLK_DIV1 = 0x01,     ///< System clock
    TCA_CLK_DIV2 = 0x02,     ///< System clock / 2
    TCA_CLK_DIV4 = 0x03,     ///< System clock / 4
    TCA_CLK_DIV8 = 0x04,     ///< System clock / 8
    TCA_CLK_DIV16 = 0x05,    ///< System clock / 16
    TCA_CLK_DIV64 = 0x06,    ///< System clock / 64
    TCA_CLK_DIV256 = 0x07,   ///< System clock / 256
    TCA_CLK_DIV1024 = 0x08   ///< System clock / 1024
} TCA_ClkSel_t;

/**
 * @brief TCA waveform generation modes
 */
typedef enum {
    TCA_WGMODE_NORMAL = 0x00,     ///< Normal mode
    TCA_WGMODE_FRQ = 0x01,        ///< Frequency generation mode
    TCA_WGMODE_SINGLESLOPE = 0x03, ///< Single slope PWM
    TCA_WGMODE_DUALSLOPE = 0x05,   ///< Dual slope PWM
    TCA_WGMODE_DSBOTTOM = 0x06,    ///< Dual slope PWM, update at BOTTOM
    TCA_WGMODE_DSTOP = 0x07        ///< Dual slope PWM, update at TOP
} TCA_WgMode_t;

/**
 * @brief TCA compare output modes
 */
typedef enum {
    TCA_CMPMODE_DISABLE = 0x00,   ///< Compare output disabled
    TCA_CMPMODE_TOGGLE = 0x01,    ///< Toggle on compare match
    TCA_CMPMODE_CLEAR = 0x02,     ///< Clear on compare match
    TCA_CMPMODE_SET = 0x03        ///< Set on compare match
} TCA_CmpMode_t;

/**
 * @brief TCA interrupt configuration
 */
typedef enum {
    TCA_INT_NONE = 0x00,          ///< No interrupts
    TCA_INT_OVF = 0x01,           ///< Overflow interrupt
    TCA_INT_CMP0 = 0x10,          ///< Compare 0 interrupt
    TCA_INT_CMP1 = 0x20,          ///< Compare 1 interrupt
    TCA_INT_CMP2 = 0x40,          ///< Compare 2 interrupt
    TCA_INT_ALL_CMP = 0x70,       ///< All compare interrupts
    TCA_INT_ALL = 0x71            ///< All interrupts
} TCA_IntConfig_t;

/**
 * @brief TCA event action configuration
 */
typedef enum {
    TCA_EVACT_NONE = 0x00,        ///< No event action
    TCA_EVACT_UPDOWN = 0x01,      ///< Count on event (up/down)
    TCA_EVACT_RESTART = 0x02,     ///< Restart timer on event
    TCA_EVACT_RESET = 0x03        ///< Reset timer on event
} TCA_EvAct_t;

/**
 * @brief TCA compare channels
 */
typedef enum {
    TCA_CMP_CHANNEL0 = 0,         ///< Compare channel 0
    TCA_CMP_CHANNEL1 = 1,         ///< Compare channel 1
    TCA_CMP_CHANNEL2 = 2          ///< Compare channel 2
} TCA_CmpChannel_t;

/**
 * @brief TCA configuration structure for maximum flexibility
 */
typedef struct {
    uint16_t period;              ///< Timer period value
    uint16_t compare0;            ///< Compare 0 value
    uint16_t compare1;            ///< Compare 1 value
    uint16_t compare2;            ///< Compare 2 value
    TCA_ClkSel_t clockSelect;     ///< Clock source selection
    TCA_WgMode_t waveformMode;    ///< Waveform generation mode
    TCA_IntConfig_t interrupts;   ///< Interrupt configuration
    bool runInStandby;            ///< Run in standby mode
    bool autoLockUpdate;          ///< Auto lock update enable
    bool enableCmp0;              ///< Enable compare channel 0
    bool enableCmp1;              ///< Enable compare channel 1
    bool enableCmp2;              ///< Enable compare channel 2
    TCA_EvAct_t eventActionA;     ///< Event action for input A
    TCA_EvAct_t eventActionB;     ///< Event action for input B
    bool countOnEventA;           ///< Enable counting on event A
    bool countOnEventB;           ///< Enable counting on event B
} TCA_Config_t;    

extern const struct TMR_INTERFACE TCA0_Interface;

// =============================================================================
// ENHANCED INITIALIZATION AND CONFIGURATION FUNCTIONS
// =============================================================================

/**
 * @brief Initialize TCA0 with default configuration (Normal mode, DIV4 prescaler)
 * 
 * This function provides a simple initialization with sensible defaults:
 * - Normal mode operation
 * - System clock / 4 prescaler
 * - 16-bit period (0xFFFF)
 * - All compare channels disabled
 * - No interrupts enabled
 * 
 * @param None
 * @return None
 */
void TCA0_Initialize(void);

/**
 * @brief Initialize TCA0 with comprehensive configuration
 * 
 * This function allows complete customization of all TCA0 parameters
 * in a single call, providing maximum flexibility.
 * 
 * @param config Pointer to TCA_Config_t structure with all parameters
 * @return None
 */
void TCA0_InitializeAdvanced(const TCA_Config_t *config);

/**
 * @brief Initialize TCA0 for PWM generation with specified parameters
 * 
 * Configures TCA0 for single-slope PWM generation with the specified
 * frequency and initial duty cycles.
 * 
 * @param frequency_hz Desired PWM frequency in Hz
 * @param duty0_percent Initial duty cycle for channel 0 (0-100%)
 * @param duty1_percent Initial duty cycle for channel 1 (0-100%)
 * @param duty2_percent Initial duty cycle for channel 2 (0-100%)
 * @return true if configuration successful, false if frequency too high/low
 */
bool TCA0_InitializePWM(uint32_t frequency_hz, uint8_t duty0_percent, 
                       uint8_t duty1_percent, uint8_t duty2_percent);

/**
 * @brief Initialize TCA0 for frequency generation
 * 
 * @param frequency_hz Target frequency in Hz
 * @param channel Compare channel to use (0, 1, or 2)
 * @return true if successful, false if frequency cannot be achieved
 */
bool TCA0_InitializeFrequencyGenerator(uint32_t frequency_hz, TCA_CmpChannel_t channel);

// =============================================================================
// BASIC TIMER CONTROL FUNCTIONS
// =============================================================================

/**
 * @brief Start the TCA0 timer
 * @param None
 * @return None
 */
void TCA0_Start(void);

/**
 * @brief Stop the TCA0 timer
 * @param None
 * @return None
 */
void TCA0_Stop(void);

/**
 * @brief Reset TCA0 to default state
 * 
 * Stops the timer, clears all registers, and resets configuration
 * to power-on defaults.
 * 
 * @param None
 * @return None
 */
void TCA0_Reset(void);

/**
 * @brief Check if TCA0 is enabled and running
 * @param None
 * @retval true Timer is enabled and running
 * @retval false Timer is stopped
 */
bool TCA0_IsEnabled(void);

// =============================================================================
// COUNTER AND PERIOD FUNCTIONS
// =============================================================================

/**
 * @brief Write counter value
 * @param timerVal Counter value to write
 * @return None
 */
void TCA0_Write(uint16_t timerVal);

/**
 * @brief Read current counter value
 * @param None
 * @return Current counter value
 */
uint16_t TCA0_Read(void);

/**
 * @brief Set the timer period
 * @param period New period value
 * @return None
 */
void TCA0_SetPeriod(uint16_t period);

/**
 * @brief Get the current timer period
 * @param None
 * @return Current period value
 */
uint16_t TCA0_GetPeriod(void);

/**
 * @brief Set timer frequency by calculating appropriate period and prescaler
 * @param frequency_hz Desired frequency in Hz
 * @return true if successful, false if frequency cannot be achieved
 */
bool TCA0_SetFrequency(uint32_t frequency_hz);

/**
 * @brief Get current timer frequency
 * @param None
 * @return Current frequency in Hz
 */
uint32_t TCA0_GetFrequency(void);

// =============================================================================
// COMPARE CHANNEL FUNCTIONS
// =============================================================================

/**
 * @brief Set compare value for specified channel
 * @param channel Compare channel (0, 1, or 2)
 * @param value Compare value
 * @return None
 */
void TCA0_SetCompare(TCA_CmpChannel_t channel, uint16_t value);

/**
 * @brief Get compare value for specified channel
 * @param channel Compare channel (0, 1, or 2)
 * @return Compare value
 */
uint16_t TCA0_GetCompare(TCA_CmpChannel_t channel);

/**
 * @brief Enable compare channel output
 * @param channel Compare channel to enable
 * @return None
 */
void TCA0_EnableCompareOutput(TCA_CmpChannel_t channel);

/**
 * @brief Disable compare channel output
 * @param channel Compare channel to disable
 * @return None
 */
void TCA0_DisableCompareOutput(TCA_CmpChannel_t channel);

/**
 * @brief Check if compare channel output is enabled
 * @param channel Compare channel to check
 * @retval true Channel output is enabled
 * @retval false Channel output is disabled
 */
bool TCA0_IsCompareOutputEnabled(TCA_CmpChannel_t channel);

// =============================================================================
// PWM FUNCTIONS
// =============================================================================

/**
 * @brief Set PWM duty cycle for specified channel
 * @param channel PWM channel (0, 1, or 2)
 * @param duty_percent Duty cycle percentage (0-100%)
 * @return None
 */
void TCA0_SetPWMDutyCycle(TCA_CmpChannel_t channel, uint8_t duty_percent);

/**
 * @brief Get PWM duty cycle for specified channel
 * @param channel PWM channel (0, 1, or 2)
 * @return Duty cycle percentage (0-100%)
 */
uint8_t TCA0_GetPWMDutyCycle(TCA_CmpChannel_t channel);

/**
 * @brief Set PWM frequency for all channels
 * @param frequency_hz Desired PWM frequency in Hz
 * @return true if successful, false if frequency cannot be achieved
 */
bool TCA0_SetPWMFrequency(uint32_t frequency_hz);

/**
 * @brief Enable PWM output on specified channel
 * @param channel PWM channel to enable
 * @return None
 */
void TCA0_EnablePWM(TCA_CmpChannel_t channel);

/**
 * @brief Disable PWM output on specified channel
 * @param channel PWM channel to disable
 * @return None
 */
void TCA0_DisablePWM(TCA_CmpChannel_t channel);

// =============================================================================
// ADVANCED CONFIGURATION FUNCTIONS
// =============================================================================

/**
 * @brief Set clock prescaler
 * @param clockSelect Clock prescaler selection
 * @return None
 */
void TCA0_SetClockSelect(TCA_ClkSel_t clockSelect);

/**
 * @brief Get current clock prescaler
 * @param None
 * @return Current clock prescaler
 */
TCA_ClkSel_t TCA0_GetClockSelect(void);

/**
 * @brief Set waveform generation mode
 * @param mode Waveform generation mode
 * @return None
 */
void TCA0_SetWaveformMode(TCA_WgMode_t mode);

/**
 * @brief Get current waveform generation mode
 * @param None
 * @return Current waveform generation mode
 */
TCA_WgMode_t TCA0_GetWaveformMode(void);

/**
 * @brief Enable/disable run in standby mode
 * @param enable true to enable, false to disable
 * @return None
 */
void TCA0_SetRunInStandby(bool enable);

/**
 * @brief Check if run in standby is enabled
 * @param None
 * @retval true Run in standby is enabled
 * @retval false Run in standby is disabled
 */
bool TCA0_IsRunInStandbyEnabled(void);

/**
 * @brief Set count direction
 * @param up true for up counting, false for down counting
 * @return None
 */
void TCA0_SetCountDirection(bool up);

/**
 * @brief Get count direction
 * @param None
 * @retval true Counting up
 * @retval false Counting down
 */
bool TCA0_GetCountDirection(void);

// =============================================================================
// EVENT SYSTEM FUNCTIONS
// =============================================================================

/**
 * @brief Configure event actions
 * @param eventA Action for event input A
 * @param eventB Action for event input B
 * @return None
 */
void TCA0_ConfigureEvents(TCA_EvAct_t eventA, TCA_EvAct_t eventB);

/**
 * @brief Enable event counting
 * @param enableA Enable counting on event A
 * @param enableB Enable counting on event B
 * @return None
 */
void TCA0_EnableEventCounting(bool enableA, bool enableB);

// =============================================================================
// INTERRUPT FUNCTIONS
// =============================================================================

/**
 * @brief Register callback for overflow interrupt
 * @param cb Callback function
 * @return None
 */
void TCA0_OverflowCallbackRegister(TCA0_cb_t cb);

/**
 * @brief Register callback for compare 0 interrupt
 * @param cb Callback function
 * @return None
 */
void TCA0_Compare0CallbackRegister(TCA0_cb_t cb);

/**
 * @brief Register callback for compare 1 interrupt
 * @param cb Callback function
 * @return None
 */
void TCA0_Compare1CallbackRegister(TCA0_cb_t cb);

/**
 * @brief Register callback for compare 2 interrupt
 * @param cb Callback function
 * @return None
 */
void TCA0_Compare2CallbackRegister(TCA0_cb_t cb);

/**
 * @brief Enable all interrupts (overflow + all compare channels)
 * @param None
 * @return None
 */
void TCA0_EnableInterrupt(void);

/**
 * @brief Disable all interrupts
 * @param None
 * @return None
 */
void TCA0_DisableInterrupt(void);

/**
 * @brief Enable specific interrupt
 * @param interrupt Interrupt to enable (use TCA_INT_* constants)
 * @return None
 */
void TCA0_EnableSpecificInterrupt(TCA_IntConfig_t interrupt);

/**
 * @brief Disable specific interrupt
 * @param interrupt Interrupt to disable (use TCA_INT_* constants)
 * @return None
 */
void TCA0_DisableSpecificInterrupt(TCA_IntConfig_t interrupt);

// =============================================================================
// INTERRUPT FLAG FUNCTIONS
// =============================================================================

/**
 * @brief Clear overflow interrupt flag
 * @param None
 * @return None
 */
void TCA0_ClearOverflowInterruptFlag(void);

/**
 * @brief Check if overflow interrupt flag is set
 * @param None
 * @retval true Flag is set
 * @retval false Flag is not set
 */
bool TCA0_IsOverflowInterruptFlagSet(void);

/**
 * @brief Clear compare 0 interrupt flag
 * @param None
 * @return None
 */
void TCA0_ClearCMP0InterruptFlag(void);

/**
 * @brief Check if compare 0 interrupt flag is set
 * @param None
 * @retval true Flag is set
 * @retval false Flag is not set
 */
bool TCA0_IsCMP0InterruptFlagSet(void);

/**
 * @brief Clear compare 1 interrupt flag
 * @param None
 * @return None
 */
void TCA0_ClearCMP1InterruptFlag(void);

/**
 * @brief Check if compare 1 interrupt flag is set
 * @param None
 * @retval true Flag is set
 * @retval false Flag is not set
 */
bool TCA0_IsCMP1InterruptFlagSet(void);

/**
 * @brief Clear compare 2 interrupt flag
 * @param None
 * @return None
 */
void TCA0_ClearCMP2InterruptFlag(void);

/**
 * @brief Check if compare 2 interrupt flag is set
 * @param None
 * @retval true Flag is set
 * @retval false Flag is not set
 */
bool TCA0_IsCMP2InterruptFlagSet(void);

/**
 * @brief Clear specific interrupt flags
 * @param flags Flags to clear (use TCA_INT_* constants)
 * @return None
 */
void TCA0_ClearInterruptFlags(TCA_IntConfig_t flags);

/**
 * @brief Get all interrupt flags status
 * @param None
 * @return Current interrupt flags status
 */
uint8_t TCA0_GetInterruptFlags(void);

// =============================================================================
// STATUS AND DIAGNOSTIC FUNCTIONS
// =============================================================================

/**
 * @brief Get comprehensive timer status
 * @param None
 * @return Status register value
 */
uint8_t TCA0_GetStatus(void);

/**
 * @brief Check if timer is at TOP (period value)
 * @param None
 * @retval true Timer is at TOP
 * @retval false Timer is not at TOP
 */
bool TCA0_IsAtTop(void);

/**
 * @brief Check if timer is at BOTTOM (zero)
 * @param None
 * @retval true Timer is at BOTTOM
 * @retval false Timer is not at BOTTOM
 */
bool TCA0_IsAtBottom(void);

// =============================================================================
// UTILITY AND CALCULATION FUNCTIONS
// =============================================================================

/**
 * @brief Calculate optimal prescaler and period for desired frequency
 * @param frequency_hz Desired frequency in Hz
 * @param prescaler Pointer to store calculated prescaler
 * @param period Pointer to store calculated period
 * @return true if calculation successful, false if frequency not achievable
 */
bool TCA0_CalculateTimingParameters(uint32_t frequency_hz, TCA_ClkSel_t *prescaler, uint16_t *period);

/**
 * @brief Get system clock frequency (needed for calculations)
 * @param None
 * @return System clock frequency in Hz
 */
uint32_t TCA0_GetSystemClockFreq(void);

/**
 * @brief Convert duty cycle percentage to compare value
 * @param duty_percent Duty cycle percentage (0-100%)
 * @return Compare value
 */
uint16_t TCA0_DutyCycleToCompareValue(uint8_t duty_percent);

/**
 * @brief Convert compare value to duty cycle percentage
 * @param compare_value Compare value
 * @return Duty cycle percentage (0-100%)
 */
uint8_t TCA0_CompareValueToDutyCycle(uint16_t compare_value);

/**
 * @}
 */

#endif /* TCA_H_INCLUDED */
