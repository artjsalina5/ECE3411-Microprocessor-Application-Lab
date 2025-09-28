/**
 * @file rtc.c
 * @author Microchip Technology Inc.  
 * @date 2025-09-24
 * @brief RTC (Real-Time Counter) Driver Implementation
 *
 * @defgroup rtc_impl RTC Driver Implementation
 * @ingroup rtc
 * @{
 *
 * This file contains the complete driver implementation for the RTC module,
 * providing both configurable and default initialization functions along
 * with comprehensive control and status functions.
 *
 * @version RTC Driver Version 2.0.2
 */


#include "rtc.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

/** @defgroup rtc_callbacks Callback Function Pointers
 *  @ingroup rtc_impl
 *  @{
 *  Global callback function pointers for RTC interrupts
 */
void (*RTC_OVF_isr_cb)(void) = NULL;  /**< Overflow interrupt callback */
void (*RTC_CMP_isr_cb)(void) = NULL;  /**< Compare match interrupt callback */
void (*RTC_PIT_isr_cb)(void) = NULL;  /**< PIT interrupt callback */
/** @} */

int8_t RTC_Initialize(uint16_t compare, uint16_t count, uint16_t period,
                      uint8_t clk, uint8_t interrupt, uint8_t config,
                      uint8_t PI) {
  while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */
  }
  // Compare
  RTC.CMP = compare;

  // Count
  RTC.CNT = count;

  // Period
  RTC.PER = period;

  // Clock selection
  RTC.CLKSEL = clk;

  // CMP enabled; OVF disabled;
  RTC.INTCTRL = interrupt;

  // CORREN disabled; PRESCALER RTC Clock / 32768; RTCEN enabled; RUNSTDBY
  // disabled;
  RTC.CTRLA = config;

  while (RTC.PITSTATUS > 0) { /* Wait for all register to be synchronized */
  }
  // PI disabled;
  RTC.PITINTCTRL = PI;

  return 0;
}

/**
 * @brief Initialize RTC with default safe configuration
 * @ingroup rtc_impl
 * 
 * Provides a simple initialization for common use cases with sensible defaults.
 * 
 * @retval 0 Initialization successful
 */
int8_t RTC_Initialize_Default(void) {
    return RTC_Initialize(
        0xFFFF,                    // Compare: Max value (disabled)
        0,                         // Count: Start at 0
        0xFFFF,                    // Period: Max value for maximum range
        RTC_CLK_OSC32K,           // Clock: Internal 32kHz
        RTC_INT_OVF,              // Interrupt: Only overflow
        RTC_PRESCALER_DIV1 | RTC_RTCEN_bm, // Config: No prescaler, RTC enabled
        0x00                       // PIT: Disabled
    );
}

/**
 * @brief Enable RTC counter
 * @ingroup rtc_impl
 * 
 * Starts the RTC counter by setting the enable bit in the control register.
 */
void RTC_Start(void) { RTC.CTRLA |= RTC_RTCEN_bm; }

/**
 * @brief Disable RTC counter  
 * @ingroup rtc_impl
 * 
 * Stops the RTC counter by clearing the enable bit in the control register.
 */
void RTC_Stop(void) { RTC.CTRLA &= ~RTC_RTCEN_bm; }

/**
 * @brief Set overflow interrupt callback function
 * @ingroup rtc_impl
 * 
 * @param cb Function pointer to callback function for overflow interrupt
 */
void RTC_SetOVFIsrCallback(RTC_cb_t cb) { RTC_OVF_isr_cb = cb; }

/**
 * @brief Set compare match interrupt callback function
 * @ingroup rtc_impl
 * 
 * @param cb Function pointer to callback function for compare match interrupt
 */
void RTC_SetCMPIsrCallback(RTC_cb_t cb) { RTC_CMP_isr_cb = cb; }

/**
 * @brief Set PIT interrupt callback function
 * @ingroup rtc_impl
 * 
 * @param cb Function pointer to callback function for PIT interrupt
 */
void RTC_SetPITIsrCallback(RTC_cb_t cb) { RTC_PIT_isr_cb = cb; }

/**
 * @brief RTC counter interrupt service routine
 * @ingroup rtc_impl
 * 
 * Handles both overflow and compare match interrupts, calling the appropriate
 * callback functions if they have been registered.
 */
ISR(RTC_CNT_vect) {
  if (RTC.INTFLAGS & RTC_OVF_bm) {
    if (RTC_OVF_isr_cb != NULL) {
      (*RTC_OVF_isr_cb)();
    }
  }

  if (RTC.INTFLAGS & RTC_CMP_bm) {
    if (RTC_CMP_isr_cb != NULL) {
      (*RTC_CMP_isr_cb)();
    }
  }
  RTC.INTFLAGS = (RTC_OVF_bm | RTC_CMP_bm);
}

/**
 * @brief PIT interrupt service routine
 * @ingroup rtc_impl
 * 
 * Handles PIT interrupts, calling the registered callback function if available.
 */
ISR(RTC_PIT_vect) {
  if (RTC_PIT_isr_cb != NULL) {
    (*RTC_PIT_isr_cb)();
  }
  RTC.PITINTFLAGS = RTC_PI_bm;
}

/**
 * @brief Write a value to the RTC counter register
 * @ingroup rtc_impl
 * 
 * This function safely writes a new value to the counter register,
 * waiting for any ongoing synchronization to complete first.
 * 
 * @param timerVal 16-bit counter value to write (0-65535)
 */
inline void RTC_WriteCounter(uint16_t timerVal) {
  while (RTC.STATUS & RTC_CNTBUSY_bm)
    ; /* Wait for counter sync */
  RTC.CNT = timerVal;
}

/**
 * @brief Read current value from RTC counter register
 * @ingroup rtc_impl
 * 
 * @return Current 16-bit counter value
 */
inline uint16_t RTC_ReadCounter(void) { return RTC.CNT; }

/**
 * @brief Write a value to the RTC period register
 * @ingroup rtc_impl
 * 
 * This function safely writes a new period value, waiting for any
 * ongoing synchronization to complete first.
 * 
 * @param timerVal 16-bit period value to write (0-65535)
 */
inline void RTC_WritePeriod(uint16_t timerVal) {
  while (RTC.STATUS & RTC_PERBUSY_bm)
    ; /* Wait for period sync */
  RTC.PER = timerVal;
}

/**
 * @brief Read current value from RTC period register
 * @ingroup rtc_impl
 * 
 * @return Current 16-bit period value
 */
inline uint16_t RTC_ReadPeriod(void) { return RTC.PER; }

/**
 * @brief Enable compare match interrupt
 * @ingroup rtc_impl
 */
inline void RTC_EnableCMPInterrupt(void) { RTC.INTCTRL |= RTC_CMP_bm; }

/**
 * @brief Disable compare match interrupt
 * @ingroup rtc_impl  
 */
inline void RTC_DisableCMPInterrupt(void) { RTC.INTCTRL &= ~RTC_CMP_bm; }

/**
 * @brief Enable overflow interrupt
 * @ingroup rtc_impl
 */
inline void RTC_EnableOVFInterrupt(void) { RTC.INTCTRL |= RTC_OVF_bm; }

/**
 * @brief Disable overflow interrupt
 * @ingroup rtc_impl
 */
inline void RTC_DisableOVFInterrupt(void) { RTC.INTCTRL &= ~RTC_OVF_bm; }

/**
 * @brief Enable PIT (Periodic Interrupt Timer) interrupt
 * @ingroup rtc_impl
 */
inline void RTC_EnablePITInterrupt(void) { RTC.PITINTCTRL |= RTC_PI_bm; }

/**
 * @brief Disable PIT (Periodic Interrupt Timer) interrupt
 * @ingroup rtc_impl
 */
inline void RTC_DisablePITInterrupt(void) { RTC.PITINTCTRL &= ~RTC_PI_bm; }

/**
 * @brief Clear the overflow interrupt flag
 * @ingroup rtc_impl
 * 
 * Writing a '1' to the flag bit clears it.
 */
inline void RTC_ClearOVFInterruptFlag(void) { RTC.INTFLAGS = RTC_OVF_bm; }

/**
 * @brief Check if overflow interrupt is enabled
 * @ingroup rtc_impl
 * 
 * @return true if overflow interrupt is enabled, false otherwise
 */
inline bool RTC_IsOVFInterruptEnabled(void) {
  return ((RTC.INTCTRL & RTC_OVF_bm) > 0);
}

/**
 * @brief Write a value to the RTC compare register
 * @ingroup rtc_impl
 * 
 * This function safely writes a new compare value, waiting for any
 * ongoing synchronization to complete first.
 * 
 * @param compareVal 16-bit compare value to write (0-65535)
 */
inline void RTC_WriteCompare(uint16_t compareVal) {
  while (RTC.STATUS & RTC_CMPBUSY_bm)
    ; /* Wait for compare sync */
  RTC.CMP = compareVal;
}

/**
 * @brief Read current value from RTC compare register
 * @ingroup rtc_impl
 * 
 * @return Current 16-bit compare value
 */
inline uint16_t RTC_ReadCompare(void) { return RTC.CMP; }

/**
 * @brief Check if compare match interrupt is enabled
 * @ingroup rtc_impl
 * 
 * @return true if compare match interrupt is enabled, false otherwise
 */
inline bool RTC_IsCMPInterruptEnabled(void) {
  return ((RTC.INTCTRL & RTC_CMP_bm) > 0);
}

/**
 * @brief Clear the compare match interrupt flag
 * @ingroup rtc_impl
 * 
 * Writing a '1' to the flag bit clears it.
 */
inline void RTC_ClearCMPInterruptFlag(void) { RTC.INTFLAGS = RTC_CMP_bm; }

/**
 * @brief Clear all RTC interrupt flags
 * @ingroup rtc_impl
 * 
 * Clears both overflow and compare match interrupt flags.
 */
inline void RTC_ClearAllInterruptFlags(void) { 
  RTC.INTFLAGS = (RTC_OVF_bm | RTC_CMP_bm); 
}

/**
 * @brief Get current RTC interrupt flags
 * @ingroup rtc_impl
 * 
 * @return Current interrupt flags register value
 */
inline uint8_t RTC_GetInterruptFlags(void) { return RTC.INTFLAGS; }

/**
 * @brief Get current RTC status
 * @ingroup rtc_impl
 * 
 * @return Current status register value with synchronization busy flags
 */
inline uint8_t RTC_GetStatus(void) { return RTC.STATUS; }

/**
 * @brief Check if RTC is busy with synchronization
 * @ingroup rtc_impl
 * 
 * @return true if any synchronization is in progress, false otherwise
 */
inline bool RTC_IsBusy(void) { return (RTC.STATUS != 0); }

/**
 * @brief Check if counter synchronization is busy
 * @ingroup rtc_impl
 * 
 * @return true if counter sync is busy, false otherwise
 */
inline bool RTC_IsCounterBusy(void) { return ((RTC.STATUS & RTC_CNTBUSY_bm) != 0); }

/**
 * @brief Check if period synchronization is busy
 * @ingroup rtc_impl
 * 
 * @return true if period sync is busy, false otherwise
 */
inline bool RTC_IsPeriodBusy(void) { return ((RTC.STATUS & RTC_PERBUSY_bm) != 0); }

/**
 * @brief Check if compare synchronization is busy
 * @ingroup rtc_impl
 * 
 * @return true if compare sync is busy, false otherwise
 */
inline bool RTC_IsCompareBusy(void) { return ((RTC.STATUS & RTC_CMPBUSY_bm) != 0); }

/**
 * @brief Check if RTC is enabled
 * @ingroup rtc_impl
 * 
 * @return true if RTC counter is enabled, false otherwise
 */
inline bool RTC_IsEnabled(void) { return ((RTC.CTRLA & RTC_RTCEN_bm) != 0); }

/**
 * @brief Get current clock source setting
 * @ingroup rtc_impl
 * 
 * @return Current clock source selection (RTC_CLKSEL_*_gc values)
 */
inline uint8_t RTC_GetClockSource(void) { return RTC.CLKSEL; }

/**
 * @brief Set clock source for RTC
 * @ingroup rtc_impl
 * 
 * @param clockSource Clock source selection (use RTC_CLK_* constants)
 */
inline void RTC_SetClockSource(uint8_t clockSource) { 
  RTC.CLKSEL = clockSource; 
}

/**
 * @brief Get current prescaler setting from CTRLA register
 * @ingroup rtc_impl
 * 
 * @return Current prescaler bits from CTRLA register
 */
inline uint8_t RTC_GetPrescaler(void) { 
  return (RTC.CTRLA & 0x78); /* Extract prescaler bits [6:3] */
}

/**
 * @brief Set prescaler value in CTRLA register
 * @ingroup rtc_impl
 * 
 * @param prescaler Prescaler setting (use RTC_PRESCALER_* constants)
 * @note This preserves other bits in CTRLA register
 */
inline void RTC_SetPrescaler(uint8_t prescaler) {
  uint8_t temp = RTC.CTRLA & ~0x78; /* Clear prescaler bits */
  RTC.CTRLA = temp | (prescaler & 0x78); /* Set new prescaler */
}

/**
 * @brief Enable/disable run in standby mode
 * @ingroup rtc_impl
 * 
 * @param enable true to enable run in standby, false to disable
 */
inline void RTC_SetRunInStandby(bool enable) {
  if (enable) {
    RTC.CTRLA |= 0x80; /* Set RUNSTDBY bit */
  } else {
    RTC.CTRLA &= ~0x80; /* Clear RUNSTDBY bit */
  }
}

/**
 * @brief Check if run in standby is enabled
 * @ingroup rtc_impl
 * 
 * @return true if run in standby is enabled, false otherwise
 */
inline bool RTC_IsRunInStandbyEnabled(void) {
  return ((RTC.CTRLA & 0x80) != 0);
}

/**
 * @brief Enable/disable correction
 * @ingroup rtc_impl
 * 
 * @param enable true to enable correction, false to disable
 */
inline void RTC_SetCorrection(bool enable) {
  if (enable) {
    RTC.CTRLA |= 0x04; /* Set CORREN bit */
  } else {
    RTC.CTRLA &= ~0x04; /* Clear CORREN bit */
  }
}

/**
 * @brief Check if correction is enabled
 * @ingroup rtc_impl
 * 
 * @return true if correction is enabled, false otherwise
 */
inline bool RTC_IsCorrectionEnabled(void) {
  return ((RTC.CTRLA & 0x04) != 0);
}

/**
 * @brief Set calibration value
 * @ingroup rtc_impl
 * 
 * @param calibValue 8-bit calibration value for frequency adjustment
 */
inline void RTC_SetCalibration(uint8_t calibValue) {
  RTC.CALIB = calibValue;
}

/**
 * @brief Get current calibration value
 * @ingroup rtc_impl
 * 
 * @return Current 8-bit calibration value
 */
inline uint8_t RTC_GetCalibration(void) {
  return RTC.CALIB;
}

/**
 * @brief Configure PIT (Periodic Interrupt Timer) period
 * @ingroup rtc_impl
 * 
 * @param period PIT period setting (use RTC_PERIOD_* constants)
 */
void RTC_ConfigurePIT(uint8_t period) {
  while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
    ; /* Wait for PIT sync */
  RTC.PITCTRLA = period;
}

/**
 * @brief Enable PIT
 * @ingroup rtc_impl
 */
void RTC_EnablePIT(void) {
  while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
    ; /* Wait for PIT sync */
  RTC.PITCTRLA |= RTC_PITEN_bm;
}

/**
 * @brief Disable PIT
 * @ingroup rtc_impl
 */
void RTC_DisablePIT(void) {
  while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
    ; /* Wait for PIT sync */
  RTC.PITCTRLA &= ~RTC_PITEN_bm;
}

/**
 * @brief Check if PIT is enabled
 * @ingroup rtc_impl
 * 
 * @return true if PIT is enabled, false otherwise
 */
inline bool RTC_IsPITEnabled(void) {
  return ((RTC.PITCTRLA & RTC_PITEN_bm) != 0);
}

/**
 * @brief Get PIT status
 * @ingroup rtc_impl
 * 
 * @return Current PIT status register value
 */
inline uint8_t RTC_GetPITStatus(void) {
  return RTC.PITSTATUS;
}

/**
 * @brief Clear PIT interrupt flag
 * @ingroup rtc_impl
 */
inline void RTC_ClearPITInterruptFlag(void) {
  RTC.PITINTFLAGS = RTC_PI_bm;
}

/**
 * @brief Check if PIT interrupt is enabled
 * @ingroup rtc_impl
 * 
 * @return true if PIT interrupt is enabled, false otherwise
 */
inline bool RTC_IsPITInterruptEnabled(void) {
  return ((RTC.PITINTCTRL & RTC_PI_bm) != 0);
}

/**
 * @brief Perform a complete RTC reset and reconfiguration
 * @ingroup rtc_impl
 * 
 * This function stops the RTC, clears all settings, and allows for
 * complete reconfiguration from a known state.
 */
void RTC_Reset(void) {
  /* Stop RTC */
  RTC.CTRLA &= ~RTC_RTCEN_bm;
  
  /* Wait for all synchronization to complete */
  while (RTC.STATUS != 0);
  
  /* Clear all interrupt flags */
  RTC.INTFLAGS = (RTC_OVF_bm | RTC_CMP_bm);
  
  /* Reset all registers to default values */
  RTC.CNT = 0;
  RTC.PER = 0xFFFF;
  RTC.CMP = 0xFFFF;
  RTC.INTCTRL = 0;
  RTC.CTRLA = 0;
  RTC.CLKSEL = 0;
  RTC.CALIB = 0;
  
  /* Reset PIT */
  RTC.PITCTRLA &= ~RTC_PITEN_bm;
  while (RTC.PITSTATUS != 0);
  RTC.PITINTFLAGS = RTC_PI_bm;
  RTC.PITINTCTRL = 0;
  RTC.PITCTRLA = 0;
}

/** @} */
