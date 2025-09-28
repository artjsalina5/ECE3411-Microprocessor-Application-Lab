/**
 * @file tca0.c
 * @author Arturo Salinas  
 * @date 2025-09-24
 * @brief Enhanced TCA0 Driver Implementation with Maximum Configurability
 * 
 * @ingroup tca0_enhanced
 * 
 * This enhanced TCA0 driver provides comprehensive control over all aspects
 * of the Timer/Counter Type A (TCA) module on the AVR128DB48. It supports
 * all operating modes, PWM generation, frequency generation, and advanced
 * features like event system integration.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "tca.h"

// =============================================================================
// PRIVATE CONSTANTS AND MACROS
// =============================================================================

#ifndef F_CPU
#define F_CPU 4000000UL  ///< Default CPU frequency (4 MHz internal oscillator)
#endif

/// Prescaler divisor values for calculations
static const uint16_t prescaler_values[] = {
    1, 2, 4, 8, 16, 64, 256, 1024
};

// =============================================================================
// CALLBACK FUNCTION POINTERS AND INTERFACE
// =============================================================================

const struct TMR_INTERFACE TCA0_Interface = {
    .Initialize = TCA0_Initialize,
    .Start = NULL,
    .Stop = NULL,
    .PeriodCountSet = TCA0_Write,
    .TimeoutCallbackRegister = TCA0_OverflowCallbackRegister,
    .Tasks = NULL
};

// Default callback functions
void TCA0_DefaultCompare0CallbackRegister(void);
void TCA0_DefaultCompare1CallbackRegister(void);
void TCA0_DefaultCompare2CallbackRegister(void);
void TCA0_DefaultOverflowCallbackRegister(void);

// Callback function pointers
void (*TCA0_CMP0_isr_cb)(void) = &TCA0_DefaultCompare0CallbackRegister;
void (*TCA0_CMP1_isr_cb)(void) = &TCA0_DefaultCompare1CallbackRegister;
void (*TCA0_CMP2_isr_cb)(void) = &TCA0_DefaultCompare2CallbackRegister;
void (*TCA0_OVF_isr_cb)(void) = &TCA0_DefaultOverflowCallbackRegister;

void TCA0_DefaultCompare0CallbackRegister(void)
{
    //Add your ISR code here
}

void TCA0_DefaultCompare1CallbackRegister(void)
{
    //Add your ISR code here
}

void TCA0_DefaultCompare2CallbackRegister(void)
{
    //Add your ISR code here
}

void TCA0_DefaultOverflowCallbackRegister(void)
{
    //Add your ISR code here
}

void TCA0_OverflowCallbackRegister(TCA0_cb_t cb)
{
    TCA0_OVF_isr_cb = cb;
}

void TCA0_Compare0CallbackRegister(TCA0_cb_t cb)
{
    TCA0_CMP0_isr_cb = cb;
}

void TCA0_Compare1CallbackRegister(TCA0_cb_t cb)
{
    TCA0_CMP1_isr_cb = cb;
}

void TCA0_Compare2CallbackRegister(TCA0_cb_t cb)
{
    TCA0_CMP2_isr_cb = cb;
}

ISR(TCA0_CMP0_vect)
{
    if (TCA0_CMP0_isr_cb != NULL)
        (*TCA0_CMP0_isr_cb)();
    
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
}

ISR(TCA0_CMP1_vect)
{
    if (TCA0_CMP1_isr_cb != NULL)
        (*TCA0_CMP1_isr_cb)();
    
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm;
}

ISR(TCA0_CMP2_vect)
{
    if (TCA0_CMP2_isr_cb != NULL)
        (*TCA0_CMP2_isr_cb)();
    
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP2_bm;
}

ISR(TCA0_OVF_vect)
{
    if (TCA0_OVF_isr_cb != NULL)
        (*TCA0_OVF_isr_cb)();
    
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}


void TCA0_Initialize(void) {
    // Compare 0
    TCA0.SINGLE.CMP0 = 0x0;
        
    // Compare 1
    TCA0.SINGLE.CMP1 = 0x0;
    
    // Compare 2
    TCA0.SINGLE.CMP2 = 0x0;
        
    // Count
    TCA0.SINGLE.CNT = 0x0;
    
    // ALUPD disabled; CMP0EN disabled; CMP1EN disabled; CMP2EN disabled; WGMODE NORMAL; 
    TCA0.SINGLE.CTRLB = 0x0;
    
    // CMP0OV disabled; CMP1OV disabled; CMP2OV disabled; 
    TCA0.SINGLE.CTRLC = 0x0;
    
    // SPLITM disabled; 
    TCA0.SINGLE.CTRLD = 0x0;
    
    // CMD NONE; DIR disabled; LUPD disabled; 
    TCA0.SINGLE.CTRLECLR = 0x0;
    
    // CMD NONE; DIR UP; LUPD disabled; 
    TCA0.SINGLE.CTRLESET = 0x0;
    
    // CMP0BV disabled; CMP1BV disabled; CMP2BV disabled; PERBV disabled; 
    TCA0.SINGLE.CTRLFCLR = 0x0;
    
    // CMP0BV disabled; CMP1BV disabled; CMP2BV disabled; PERBV disabled; 
    TCA0.SINGLE.CTRLFSET = 0x0;
    
    // DBGRUN disabled; 
    TCA0.SINGLE.DBGCTRL = 0x0;
    
    // CNTAEI disabled; CNTBEI disabled; EVACTA CNT_POSEDGE; EVACTB NONE; 
    TCA0.SINGLE.EVCTRL = 0x0;
    
    // CMP0 disabled; CMP1 disabled; CMP2 disabled; OVF disabled; 
    TCA0.SINGLE.INTCTRL = 0x0;
    
    // CMP0 disabled; CMP1 disabled; CMP2 disabled; OVF disabled; 
    TCA0.SINGLE.INTFLAGS = 0x0;
    
    // Period
    TCA0.SINGLE.PER = 0xEA5F; // Max period = 59999
    
    // Temporary data for 16-bit Access
    TCA0.SINGLE.TEMP = 0x0;
    
    // CLKSEL DIV4; ENABLE enabled; RUNSTDBY disabled; 
    TCA0.SINGLE.CTRLA = 0x5;
    
}

void TCA0_Start(void)
{
    TCA0.SINGLE.CTRLA|= TCA_SINGLE_ENABLE_bm;
}

void TCA0_Stop(void)
{
    TCA0.SINGLE.CTRLA&= ~TCA_SINGLE_ENABLE_bm;
}

void TCA0_Write(uint16_t timerVal)
{
    TCA0.SINGLE.CNT=timerVal;
}

uint16_t TCA0_Read(void)
{
    uint16_t readVal;

    readVal = TCA0.SINGLE.CNT;

    return readVal;
}

void TCA0_EnableInterrupt(void)
{
        TCA0.SINGLE.INTCTRL = 1 << TCA_SINGLE_CMP0_bp /* Compare 0 Interrupt: enabled */
	 				| 1 << TCA_SINGLE_CMP1_bp /* Compare 1 Interrupt: enabled */
	 				| 1 << TCA_SINGLE_CMP2_bp /* Compare 2 Interrupt: enabled */
	 				| 1 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: enabled */
}
void TCA0_DisableInterrupt(void)
{
    TCA0.SINGLE.INTCTRL = 0 << TCA_SINGLE_CMP0_bp /* Compare 0 Interrupt: disabled */
	 				| 0 << TCA_SINGLE_CMP1_bp /* Compare 1 Interrupt: disabled */
	 				| 0 << TCA_SINGLE_CMP2_bp /* Compare 2 Interrupt: disabled */
	 				| 0 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: disabled */
}
void TCA0_ClearOverflowInterruptFlag(void)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; /* Clear Overflow Interrupt Flag */
}
bool TCA0_IsOverflowInterruptFlagSet(void)
{
    return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm) > 0);
}

void TCA0_ClearCMP0InterruptFlag(void)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm; /* Clear Compare Channel-0 Interrupt Flag */
}

bool TCA0_IsCMP0InterruptFlagSet(void)
{
    return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_CMP0_bm) > 0);
}

void TCA0_ClearCMP1InterruptFlag(void)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm; /* Clear Compare Channel-1 Interrupt Flag */
}

bool TCA0_IsCMP1InterruptFlagSet(void)
{
    return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_CMP1_bm) > 0);
}

void TCA0_ClearCMP2InterruptFlag(void)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP2_bm; /* Clear Compare Channel-2 Interrupt Flag */
}

bool TCA0_IsCMP2InterruptFlagSet(void)
{
    return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_CMP2_bm) > 0);
}

// =============================================================================
// ENHANCED INITIALIZATION AND CONFIGURATION FUNCTIONS
// =============================================================================

void TCA0_InitializeAdvanced(const TCA_Config_t *config)
{
    if (config == NULL) return;
    
    // Stop timer during configuration
    TCA0_Stop();
    
    // Configure period and compare values
    TCA0.SINGLE.PER = config->period;
    TCA0.SINGLE.CMP0 = config->compare0;
    TCA0.SINGLE.CMP1 = config->compare1;
    TCA0.SINGLE.CMP2 = config->compare2;
    
    // Reset counter
    TCA0.SINGLE.CNT = 0x0;
    
    // Configure Control B (waveform mode and compare enables)
    uint8_t ctrlb = ((config->waveformMode & 0x07) << TCA_SINGLE_WGMODE_gp);
    if (config->enableCmp0) ctrlb |= TCA_SINGLE_CMP0EN_bm;
    if (config->enableCmp1) ctrlb |= TCA_SINGLE_CMP1EN_bm;
    if (config->enableCmp2) ctrlb |= TCA_SINGLE_CMP2EN_bm;
    if (config->autoLockUpdate) ctrlb |= TCA_SINGLE_ALUPD_bm;
    TCA0.SINGLE.CTRLB = ctrlb;
    
    // Clear other control registers  
    TCA0.SINGLE.CTRLC = 0x0;
    TCA0.SINGLE.CTRLD = 0x0;
    TCA0.SINGLE.CTRLECLR = 0xFF;  // Clear all flags
    TCA0.SINGLE.CTRLFCLR = 0xFF;  // Clear all buffer flags
    
    // Configure event system
    uint8_t evctrl = 0x0;
    evctrl |= (config->eventActionA & 0x03) << TCA_SINGLE_EVACTA_gp;
    evctrl |= (config->eventActionB & 0x03) << TCA_SINGLE_EVACTB_gp;
    if (config->countOnEventA) evctrl |= TCA_SINGLE_CNTAEI_bm;
    if (config->countOnEventB) evctrl |= TCA_SINGLE_CNTBEI_bm;
    TCA0.SINGLE.EVCTRL = evctrl;
    
    // Configure interrupts
    TCA0.SINGLE.INTCTRL = config->interrupts;
    
    // Clear interrupt flags
    TCA0.SINGLE.INTFLAGS = 0xFF;
    
    // Configure Control A (clock select, enable, run in standby)
    uint8_t ctrla = (config->clockSelect & 0x0F);
    if (config->runInStandby) ctrla |= TCA_SINGLE_RUNSTDBY_bm;
    ctrla |= TCA_SINGLE_ENABLE_bm;  // Enable timer
    TCA0.SINGLE.CTRLA = ctrla;
}

bool TCA0_InitializePWM(uint32_t frequency_hz, uint8_t duty0_percent, 
                       uint8_t duty1_percent, uint8_t duty2_percent)
{
    TCA_ClkSel_t prescaler;
    uint16_t period;
    
    // Calculate optimal timing parameters
    if (!TCA0_CalculateTimingParameters(frequency_hz, &prescaler, &period)) {
        return false;
    }
    
    // Create configuration structure
    TCA_Config_t config = {
        .period = period,
        .compare0 = (period * duty0_percent) / 100,
        .compare1 = (period * duty1_percent) / 100,
        .compare2 = (period * duty2_percent) / 100,
        .clockSelect = prescaler,
        .waveformMode = TCA_WGMODE_SINGLESLOPE,
        .interrupts = TCA_INT_NONE,
        .runInStandby = false,
        .autoLockUpdate = false,
        .enableCmp0 = true,
        .enableCmp1 = true,
        .enableCmp2 = true,
        .eventActionA = TCA_EVACT_NONE,
        .eventActionB = TCA_EVACT_NONE,
        .countOnEventA = false,
        .countOnEventB = false
    };
    
    TCA0_InitializeAdvanced(&config);
    return true;
}

bool TCA0_InitializeFrequencyGenerator(uint32_t frequency_hz, TCA_CmpChannel_t channel)
{
    TCA_ClkSel_t prescaler;
    uint16_t period;
    
    // For frequency generation, we need frequency * 2 since output toggles
    if (!TCA0_CalculateTimingParameters(frequency_hz * 2, &prescaler, &period)) {
        return false;
    }
    
    TCA_Config_t config = {
        .period = 0xFFFF,  // Max period for frequency mode
        .compare0 = (channel == TCA_CMP_CHANNEL0) ? period : 0,
        .compare1 = (channel == TCA_CMP_CHANNEL1) ? period : 0,
        .compare2 = (channel == TCA_CMP_CHANNEL2) ? period : 0,
        .clockSelect = prescaler,
        .waveformMode = TCA_WGMODE_FRQ,
        .interrupts = TCA_INT_NONE,
        .runInStandby = false,
        .autoLockUpdate = false,
        .enableCmp0 = (channel == TCA_CMP_CHANNEL0),
        .enableCmp1 = (channel == TCA_CMP_CHANNEL1),
        .enableCmp2 = (channel == TCA_CMP_CHANNEL2),
        .eventActionA = TCA_EVACT_NONE,
        .eventActionB = TCA_EVACT_NONE,
        .countOnEventA = false,
        .countOnEventB = false
    };
    
    TCA0_InitializeAdvanced(&config);
    return true;
}

// =============================================================================
// ENHANCED TIMER CONTROL FUNCTIONS  
// =============================================================================

void TCA0_Reset(void)
{
    // Stop timer
    TCA0.SINGLE.CTRLA = 0x0;
    
    // Reset all registers to default values
    TCA0.SINGLE.CTRLB = 0x0;
    TCA0.SINGLE.CTRLC = 0x0;
    TCA0.SINGLE.CTRLD = 0x0;
    TCA0.SINGLE.CTRLECLR = 0xFF;
    TCA0.SINGLE.CTRLFCLR = 0xFF;
    TCA0.SINGLE.EVCTRL = 0x0;
    TCA0.SINGLE.INTCTRL = 0x0;
    TCA0.SINGLE.INTFLAGS = 0xFF;
    TCA0.SINGLE.DBGCTRL = 0x0;
    TCA0.SINGLE.TEMP = 0x0;
    
    // Reset counter and compare values
    TCA0.SINGLE.CNT = 0x0;
    TCA0.SINGLE.PER = 0xFFFF;
    TCA0.SINGLE.CMP0 = 0x0;
    TCA0.SINGLE.CMP1 = 0x0;
    TCA0.SINGLE.CMP2 = 0x0;
}

bool TCA0_IsEnabled(void)
{
    return (TCA0.SINGLE.CTRLA & TCA_SINGLE_ENABLE_bm) != 0;
}

// =============================================================================
// ENHANCED PERIOD AND FREQUENCY FUNCTIONS
// =============================================================================

void TCA0_SetPeriod(uint16_t period)
{
    TCA0.SINGLE.PER = period;
}

uint16_t TCA0_GetPeriod(void)
{
    return TCA0.SINGLE.PER;
}

bool TCA0_SetFrequency(uint32_t frequency_hz)
{
    TCA_ClkSel_t prescaler;
    uint16_t period;
    
    if (!TCA0_CalculateTimingParameters(frequency_hz, &prescaler, &period)) {
        return false;
    }
    
    // Update prescaler and period
    TCA0_SetClockSelect(prescaler);
    TCA0_SetPeriod(period);
    
    return true;
}

uint32_t TCA0_GetFrequency(void)
{
    uint32_t system_freq = TCA0_GetSystemClockFreq();
    TCA_ClkSel_t prescaler = TCA0_GetClockSelect();
    uint16_t period = TCA0_GetPeriod();
    
    if (prescaler == 0 || prescaler > 8 || period == 0) {
        return 0;
    }
    
    uint16_t prescaler_div = prescaler_values[prescaler - 1];
    return system_freq / (prescaler_div * (period + 1));
}

// =============================================================================
// COMPARE CHANNEL FUNCTIONS
// =============================================================================

void TCA0_SetCompare(TCA_CmpChannel_t channel, uint16_t value)
{
    switch (channel) {
        case TCA_CMP_CHANNEL0:
            TCA0.SINGLE.CMP0 = value;
            break;
        case TCA_CMP_CHANNEL1:
            TCA0.SINGLE.CMP1 = value;
            break;
        case TCA_CMP_CHANNEL2:
            TCA0.SINGLE.CMP2 = value;
            break;
    }
}

uint16_t TCA0_GetCompare(TCA_CmpChannel_t channel)
{
    switch (channel) {
        case TCA_CMP_CHANNEL0:
            return TCA0.SINGLE.CMP0;
        case TCA_CMP_CHANNEL1:
            return TCA0.SINGLE.CMP1;
        case TCA_CMP_CHANNEL2:
            return TCA0.SINGLE.CMP2;
        default:
            return 0;
    }
}

void TCA0_EnableCompareOutput(TCA_CmpChannel_t channel)
{
    switch (channel) {
        case TCA_CMP_CHANNEL0:
            TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
            break;
        case TCA_CMP_CHANNEL1:
            TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP1EN_bm;
            break;
        case TCA_CMP_CHANNEL2:
            TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm;
            break;
    }
}

void TCA0_DisableCompareOutput(TCA_CmpChannel_t channel)
{
    switch (channel) {
        case TCA_CMP_CHANNEL0:
            TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;
            break;
        case TCA_CMP_CHANNEL1:
            TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP1EN_bm;
            break;
        case TCA_CMP_CHANNEL2:
            TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP2EN_bm;
            break;
    }
}

bool TCA0_IsCompareOutputEnabled(TCA_CmpChannel_t channel)
{
    switch (channel) {
        case TCA_CMP_CHANNEL0:
            return (TCA0.SINGLE.CTRLB & TCA_SINGLE_CMP0EN_bm) != 0;
        case TCA_CMP_CHANNEL1:
            return (TCA0.SINGLE.CTRLB & TCA_SINGLE_CMP1EN_bm) != 0;
        case TCA_CMP_CHANNEL2:
            return (TCA0.SINGLE.CTRLB & TCA_SINGLE_CMP2EN_bm) != 0;
        default:
            return false;
    }
}

// =============================================================================
// PWM FUNCTIONS
// =============================================================================

void TCA0_SetPWMDutyCycle(TCA_CmpChannel_t channel, uint8_t duty_percent)
{
    if (duty_percent > 100) duty_percent = 100;
    
    uint16_t period = TCA0_GetPeriod();
    uint16_t compare_value = (period * duty_percent) / 100;
    
    TCA0_SetCompare(channel, compare_value);
}

uint8_t TCA0_GetPWMDutyCycle(TCA_CmpChannel_t channel)
{
    uint16_t compare_value = TCA0_GetCompare(channel);
    uint16_t period = TCA0_GetPeriod();
    
    if (period == 0) return 0;
    
    return (compare_value * 100) / period;
}

bool TCA0_SetPWMFrequency(uint32_t frequency_hz)
{
    return TCA0_SetFrequency(frequency_hz);
}

void TCA0_EnablePWM(TCA_CmpChannel_t channel)
{
    // Set to single-slope PWM mode if not already
    uint8_t ctrlb = TCA0.SINGLE.CTRLB;
    ctrlb = (ctrlb & ~TCA_SINGLE_WGMODE_gm) | (TCA_WGMODE_SINGLESLOPE << TCA_SINGLE_WGMODE_gp);
    TCA0.SINGLE.CTRLB = ctrlb;
    
    // Enable compare output
    TCA0_EnableCompareOutput(channel);
}

void TCA0_DisablePWM(TCA_CmpChannel_t channel)
{
    TCA0_DisableCompareOutput(channel);
}

// =============================================================================
// ADVANCED CONFIGURATION FUNCTIONS
// =============================================================================

void TCA0_SetClockSelect(TCA_ClkSel_t clockSelect)
{
    uint8_t ctrla = TCA0.SINGLE.CTRLA;
    ctrla = (ctrla & ~TCA_SINGLE_CLKSEL_gm) | (clockSelect & TCA_SINGLE_CLKSEL_gm);
    TCA0.SINGLE.CTRLA = ctrla;
}

TCA_ClkSel_t TCA0_GetClockSelect(void)
{
    return (TCA_ClkSel_t)(TCA0.SINGLE.CTRLA & TCA_SINGLE_CLKSEL_gm);
}

void TCA0_SetWaveformMode(TCA_WgMode_t mode)
{
    uint8_t ctrlb = TCA0.SINGLE.CTRLB;
    ctrlb = (ctrlb & ~TCA_SINGLE_WGMODE_gm) | ((mode & 0x07) << TCA_SINGLE_WGMODE_gp);
    TCA0.SINGLE.CTRLB = ctrlb;
}

TCA_WgMode_t TCA0_GetWaveformMode(void)
{
    return (TCA_WgMode_t)((TCA0.SINGLE.CTRLB & TCA_SINGLE_WGMODE_gm) >> TCA_SINGLE_WGMODE_gp);
}

void TCA0_SetRunInStandby(bool enable)
{
    if (enable) {
        TCA0.SINGLE.CTRLA |= TCA_SINGLE_RUNSTDBY_bm;
    } else {
        TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_RUNSTDBY_bm;
    }
}

bool TCA0_IsRunInStandbyEnabled(void)
{
    return (TCA0.SINGLE.CTRLA & TCA_SINGLE_RUNSTDBY_bm) != 0;
}

void TCA0_SetCountDirection(bool up)
{
    if (up) {
        TCA0.SINGLE.CTRLESET = TCA_SINGLE_DIR_bm;
    } else {
        TCA0.SINGLE.CTRLECLR = TCA_SINGLE_DIR_bm;
    }
}

bool TCA0_GetCountDirection(void)
{
    return (TCA0.SINGLE.CTRLE & TCA_SINGLE_DIR_bm) != 0;
}

// =============================================================================
// EVENT SYSTEM FUNCTIONS
// =============================================================================

void TCA0_ConfigureEvents(TCA_EvAct_t eventA, TCA_EvAct_t eventB)
{
    uint8_t evctrl = TCA0.SINGLE.EVCTRL;
    evctrl = (evctrl & ~(TCA_SINGLE_EVACTA_gm | TCA_SINGLE_EVACTB_gm));
    evctrl |= ((eventA & 0x03) << TCA_SINGLE_EVACTA_gp);
    evctrl |= ((eventB & 0x03) << TCA_SINGLE_EVACTB_gp);
    TCA0.SINGLE.EVCTRL = evctrl;
}

void TCA0_EnableEventCounting(bool enableA, bool enableB)
{
    uint8_t evctrl = TCA0.SINGLE.EVCTRL;
    
    if (enableA) {
        evctrl |= TCA_SINGLE_CNTAEI_bm;
    } else {
        evctrl &= ~TCA_SINGLE_CNTAEI_bm;
    }
    
    if (enableB) {
        evctrl |= TCA_SINGLE_CNTBEI_bm;
    } else {
        evctrl &= ~TCA_SINGLE_CNTBEI_bm;
    }
    
    TCA0.SINGLE.EVCTRL = evctrl;
}

// =============================================================================
// ENHANCED INTERRUPT FUNCTIONS
// =============================================================================

void TCA0_EnableSpecificInterrupt(TCA_IntConfig_t interrupt)
{
    TCA0.SINGLE.INTCTRL |= (interrupt & 0x7F);
}

void TCA0_DisableSpecificInterrupt(TCA_IntConfig_t interrupt)
{
    TCA0.SINGLE.INTCTRL &= ~(interrupt & 0x7F);
}

void TCA0_ClearInterruptFlags(TCA_IntConfig_t flags)
{
    TCA0.SINGLE.INTFLAGS = (flags & 0x7F);
}

uint8_t TCA0_GetInterruptFlags(void)
{
    return TCA0.SINGLE.INTFLAGS;
}

// =============================================================================
// STATUS AND DIAGNOSTIC FUNCTIONS
// =============================================================================

uint8_t TCA0_GetStatus(void)
{
    return TCA0.SINGLE.INTFLAGS;
}

bool TCA0_IsAtTop(void)
{
    return TCA0.SINGLE.CNT >= TCA0.SINGLE.PER;
}

bool TCA0_IsAtBottom(void)
{
    return TCA0.SINGLE.CNT == 0;
}

// =============================================================================
// UTILITY AND CALCULATION FUNCTIONS
// =============================================================================

bool TCA0_CalculateTimingParameters(uint32_t frequency_hz, TCA_ClkSel_t *prescaler, uint16_t *period)
{
    if (prescaler == NULL || period == NULL || frequency_hz == 0) {
        return false;
    }
    
    uint32_t system_freq = TCA0_GetSystemClockFreq();
    
    // Try each prescaler value to find the best fit
    for (int i = 0; i < 8; i++) {
        uint32_t divided_freq = system_freq / prescaler_values[i];
        uint32_t calculated_period = (divided_freq / frequency_hz) - 1;
        
        // Check if period fits in 16-bit register
        if (calculated_period <= 0xFFFF && calculated_period >= 1) {
            *prescaler = (TCA_ClkSel_t)(i + 1);
            *period = (uint16_t)calculated_period;
            return true;
        }
    }
    
    return false;  // Cannot achieve the requested frequency
}

uint32_t TCA0_GetSystemClockFreq(void)
{
    return F_CPU;  // Return configured CPU frequency
}

uint16_t TCA0_DutyCycleToCompareValue(uint8_t duty_percent)
{
    if (duty_percent > 100) duty_percent = 100;
    uint16_t period = TCA0_GetPeriod();
    return (period * duty_percent) / 100;
}

uint8_t TCA0_CompareValueToDutyCycle(uint16_t compare_value)
{
    uint16_t period = TCA0_GetPeriod();
    if (period == 0) return 0;
    
    uint32_t duty = (compare_value * 100) / period;
    return (duty > 100) ? 100 : (uint8_t)duty;
}