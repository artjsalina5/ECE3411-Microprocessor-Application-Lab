/**
 * @file rtc.h
 * @author Microchip Technology Inc.
 * @date 2025-09-24
 * @brief RTC (Real-Time Counter) Driver API Header File
 *
 * @defgroup rtc RTC Driver
 * @{
 *
 * This header file provides comprehensive APIs for the RTC driver with configurable
 * initialization parameters. The RTC module provides accurate timekeeping and
 * periodic interrupt generation capabilities.
 *
 * @version RTC Driver Version 2.0.2
 */
/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip
    software and any derivatives exclusively with Microchip products.
    You are responsible for complying with 3rd party license terms
    applicable to your use of 3rd party software (including open source
    software) that may accompany Microchip software. SOFTWARE IS "AS IS."
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR
    THIS SOFTWARE.
*/

#ifndef RTCDRIVER_H
#define RTCDRIVER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup rtc_config RTC Configuration Constants
 * @ingroup rtc
 * @{
 * These constants define the configuration options for RTC initialization
 */

/** @defgroup rtc_clk_sel Clock Selection Options
 *  @brief Clock source selection for RTC
 *  @{
 */
#define RTC_CLK_OSC32K      RTC_CLKSEL_OSC32K_gc    /**< 32.768 kHz from OSC32K */
#define RTC_CLK_OSC1K       RTC_CLKSEL_OSC1K_gc     /**< 1.024 kHz from OSC32K */
#define RTC_CLK_XOSC32K     RTC_CLKSEL_XOSC32K_gc   /**< 32.768 kHz from XOSC32K */
#define RTC_CLK_EXTCLK      RTC_CLKSEL_EXTCLK_gc    /**< External Clock */
/** @} */

/** @defgroup rtc_prescaler Prescaler Options
 *  @brief Prescaler settings for RTC clock
 *  @{
 */
#define RTC_PRESCALER_DIV1      RTC_PRESCALER_DIV1_gc      /**< RTC Clock / 1 */
#define RTC_PRESCALER_DIV2      RTC_PRESCALER_DIV2_gc      /**< RTC Clock / 2 */
#define RTC_PRESCALER_DIV4      RTC_PRESCALER_DIV4_gc      /**< RTC Clock / 4 */
#define RTC_PRESCALER_DIV8      RTC_PRESCALER_DIV8_gc      /**< RTC Clock / 8 */
#define RTC_PRESCALER_DIV16     RTC_PRESCALER_DIV16_gc     /**< RTC Clock / 16 */
#define RTC_PRESCALER_DIV32     RTC_PRESCALER_DIV32_gc     /**< RTC Clock / 32 */
#define RTC_PRESCALER_DIV64     RTC_PRESCALER_DIV64_gc     /**< RTC Clock / 64 */
#define RTC_PRESCALER_DIV128    RTC_PRESCALER_DIV128_gc    /**< RTC Clock / 128 */
#define RTC_PRESCALER_DIV256    RTC_PRESCALER_DIV256_gc    /**< RTC Clock / 256 */
#define RTC_PRESCALER_DIV512    RTC_PRESCALER_DIV512_gc    /**< RTC Clock / 512 */
#define RTC_PRESCALER_DIV1024   RTC_PRESCALER_DIV1024_gc   /**< RTC Clock / 1024 */
#define RTC_PRESCALER_DIV2048   RTC_PRESCALER_DIV2048_gc   /**< RTC Clock / 2048 */
#define RTC_PRESCALER_DIV4096   RTC_PRESCALER_DIV4096_gc   /**< RTC Clock / 4096 */
#define RTC_PRESCALER_DIV8192   RTC_PRESCALER_DIV8192_gc   /**< RTC Clock / 8192 */
#define RTC_PRESCALER_DIV16384  RTC_PRESCALER_DIV16384_gc  /**< RTC Clock / 16384 */
#define RTC_PRESCALER_DIV32768  RTC_PRESCALER_DIV32768_gc  /**< RTC Clock / 32768 */
/** @} */

/** @defgroup rtc_interrupt Interrupt Configuration
 *  @brief Interrupt enable/disable options
 *  @{
 */
#define RTC_INT_NONE        (0x00)              /**< No interrupts enabled */
#define RTC_INT_OVF         RTC_OVF_bm          /**< Overflow interrupt enabled */
#define RTC_INT_CMP         RTC_CMP_bm          /**< Compare match interrupt enabled */
#define RTC_INT_BOTH        (RTC_OVF_bm | RTC_CMP_bm)  /**< Both OVF and CMP interrupts enabled */
/** @} */

/** @defgroup rtc_pit_period PIT Period Options  
 *  @brief PIT (Periodic Interrupt Timer) period settings
 *  @{
 */
#define RTC_PIT_PERIOD_OFF      RTC_PERIOD_OFF_gc      /**< PIT disabled */
#define RTC_PIT_PERIOD_CYC4     RTC_PERIOD_CYC4_gc     /**< 4 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC8     RTC_PERIOD_CYC8_gc     /**< 8 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC16    RTC_PERIOD_CYC16_gc    /**< 16 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC32    RTC_PERIOD_CYC32_gc    /**< 32 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC64    RTC_PERIOD_CYC64_gc    /**< 64 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC128   RTC_PERIOD_CYC128_gc   /**< 128 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC256   RTC_PERIOD_CYC256_gc   /**< 256 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC512   RTC_PERIOD_CYC512_gc   /**< 512 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC1024  RTC_PERIOD_CYC1024_gc  /**< 1024 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC2048  RTC_PERIOD_CYC2048_gc  /**< 2048 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC4096  RTC_PERIOD_CYC4096_gc  /**< 4096 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC8192  RTC_PERIOD_CYC8192_gc  /**< 8192 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC16384 RTC_PERIOD_CYC16384_gc /**< 16384 RTC clock cycles */
#define RTC_PIT_PERIOD_CYC32768 RTC_PERIOD_CYC32768_gc /**< 32768 RTC clock cycles */
/** @} */

/** @} */

/**
 * @ingroup rtc
 * @typedef RTC_cb_t
 * @brief Function pointer to callback function called by RTC
 * 
 * NULL = default value: No callback function is to be used.
 */
typedef void (*RTC_cb_t)(void);
/**
 * @ingroup rtc
 * @brief Isr callback function to be called if overflow interrupt flag is set.
 * @param RTC_cb_t cb - call back value for overflow.
 * @return none
 */
void RTC_SetOVFIsrCallback(RTC_cb_t cb);
/**
 * @ingroup rtc
 * @brief Isr callback function to be called if Compare match interrupt flag is
 * set.
 * @param RTC_cb_t cb - call back value for compare.
 * @return none
 */
void RTC_SetCMPIsrCallback(RTC_cb_t cb);
/**
 * @ingroup rtc
 * @brief Isr callback function to be called if PIT interrupt flag is set.
 * @param RTC_cb_t cb - call back value for PIT.
 * @return none
 */
void RTC_SetPITIsrCallback(RTC_cb_t cb);
/**
 * @ingroup rtc
 * @brief Initialize RTC interface with configurable parameters
 * 
 * This function provides complete configuration of the RTC module with all
 * parameters exposed for maximum flexibility. It allows configuration of:
 * - Compare value for match interrupts
 * - Initial counter value  
 * - Period value for overflow
 * - Clock source selection
 * - Interrupt enable settings
 * - Control register configuration
 * - PIT (Periodic Interrupt Timer) configuration
 * 
 * @param compare Compare match value (0-65535)
 * @param count Initial counter value (0-65535)  
 * @param period Period value for overflow detection (0-65535)
 * @param clk Clock source selection (use RTC_CLK_* constants)
 * @param interrupt Interrupt configuration (use RTC_INT_* constants)
 * @param config Control register configuration including prescaler
 * @param PI PIT interrupt control configuration
 * 
 * @retval 0 RTC initialization was successful
 * @retval -1 RTC initialization failed
 * 
 * @note Wait for synchronization is handled internally
 * 
 * Example usage:
 * @code
 * // Initialize RTC with 1Hz overflow, compare at 500ms, interrupts enabled
 * RTC_Initialize(16384,          // Compare at 0.5 seconds (32768/2)
 *                0,              // Start counter at 0
 *                32767,          // Period for ~1Hz overflow
 *                RTC_CLK_OSC32K, // Use internal 32kHz oscillator  
 *                RTC_INT_BOTH,   // Enable both interrupts
 *                RTC_PRESCALER_DIV1 | RTC_RTCEN_bm,  // No prescaler, enable RTC
 *                0x00);          // No PIT interrupts
 * @endcode
 */
int8_t RTC_Initialize(uint16_t compare, uint16_t count, uint16_t period,
                      uint8_t clk, uint8_t interrupt, uint8_t config,
                      uint8_t PI);

/**
 * @ingroup rtc
 * @brief Initialize RTC with default configuration
 * 
 * This function provides a simple initialization with commonly used defaults:
 * - Internal 32kHz clock source
 * - No prescaling  
 * - Period set to maximum (65535)
 * - Compare disabled
 * - Only overflow interrupt enabled
 * - PIT disabled
 * 
 * @retval 0 RTC initialization was successful
 * @retval -1 RTC initialization failed
 */
int8_t RTC_Initialize_Default(void);
/**
 * @ingroup rtc
 * @brief API to start the counter register for RTC interface
 * @param none
 * @return none
 */
void RTC_Start(void);
/**
 * @ingroup rtc
 * @brief API to stop the counter register for RTC interface
 * @param none
 * @return none
 */
void RTC_Stop(void);
/**
 * @ingroup rtc
 * @brief API to write the counter value for RTC.
 * @param uint16_t timerVal - Loading the counter value to write for RTC.
 * @return none
 */
void RTC_WriteCounter(uint16_t timerVal);
/**
 * @ingroup rtc
 * @brief API to write the counter value to load for RTC.
 * @param uint16_t timerVal - Loading the write period to determine overflow
 * period in RTC.
 * @return none
 */
void RTC_WritePeriod(uint16_t timerVal);
/**
 * @ingroup rtc
 * @brief API to read the counter clock cycle value from counter register.
 * @param none
 * @return uint16_t - Counter values returns from the RTC interface.
 */
uint16_t RTC_ReadCounter(void);
/**
 * @ingroup rtc
 * @brief API to read the overflow value in period register.
 * @param none
 * @return uint16_t - Period values returns from the RTC interface.
 */
uint16_t RTC_ReadPeriod(void);
/**
 * @ingroup rtc
 * @brief Enable the CMP interrupt to set the flag, if match value between
 * counter register and compare register.
 * @param none
 * @return none
 */
void RTC_EnableCMPInterrupt(void);
/**
 * @ingroup rtc
 * @brief Disable the CMP interrupt for RTC interface.
 * @param none
 * @return none
 */
void RTC_DisableCMPInterrupt(void);
/**
 * @ingroup rtc
 * @brief Enable the overflow interrupt set the OVF flag, if the counter reached
 * value from the period register and wrapped to zero.
 * @param none
 * @return none
 */
void RTC_EnableOVFInterrupt(void);
/**
 * @ingroup rtc
 * @brief Disable the Overflow interrupt for RTC interface.
 * @param none
 * @return none
 */
void RTC_DisableOVFInterrupt(void);
/**
 * @ingroup rtc
 * @brief Enable the Period Interrupt Timer to set the flag, if a time period
 * has passed as configured in period bit field.
 * @param none
 * @return none
 */
void RTC_EnablePITInterrupt(void);
/**
 * @ingroup rtc
 * @brief Disable the PIT interrupt for RTC interface.
 * @param none
 * @return none
 */
void RTC_DisablePITInterrupt(void);
/**
 * @ingroup rtc
 * @brief Clearing the Overflow interrupt flag after the overflow flag set.
 * @param none
 * @return none
 */
void RTC_ClearOVFInterruptFlag(void);
/**
 * @ingroup rtc
 * @brief Enable the Overflow interrupt to set overflow flag, when overflow
 * occur.
 * @param none
 * @return none
 */
bool RTC_IsOVFInterruptEnabled(void);

/**
 * @defgroup rtc_extended Extended RTC Functions
 * @ingroup rtc
 * @{
 * Advanced functions for comprehensive RTC control and status monitoring
 */

/**
 * @brief Write a value to the RTC compare register
 * @param compareVal 16-bit compare value to write (0-65535)
 */
void RTC_WriteCompare(uint16_t compareVal);

/**
 * @brief Read current value from RTC compare register
 * @return Current 16-bit compare value
 */
uint16_t RTC_ReadCompare(void);

/**
 * @brief Check if compare match interrupt is enabled
 * @return true if compare match interrupt is enabled, false otherwise
 */
bool RTC_IsCMPInterruptEnabled(void);

/**
 * @brief Clear the compare match interrupt flag
 */
void RTC_ClearCMPInterruptFlag(void);

/**
 * @brief Clear all RTC interrupt flags
 */
void RTC_ClearAllInterruptFlags(void);

/**
 * @brief Get current RTC interrupt flags
 * @return Current interrupt flags register value
 */
uint8_t RTC_GetInterruptFlags(void);

/**
 * @brief Get current RTC status
 * @return Current status register value with synchronization busy flags
 */
uint8_t RTC_GetStatus(void);

/**
 * @brief Check if RTC is busy with synchronization
 * @return true if any synchronization is in progress, false otherwise
 */
bool RTC_IsBusy(void);

/**
 * @brief Check if counter synchronization is busy
 * @return true if counter sync is busy, false otherwise
 */
bool RTC_IsCounterBusy(void);

/**
 * @brief Check if period synchronization is busy
 * @return true if period sync is busy, false otherwise
 */
bool RTC_IsPeriodBusy(void);

/**
 * @brief Check if compare synchronization is busy
 * @return true if compare sync is busy, false otherwise
 */
bool RTC_IsCompareBusy(void);

/**
 * @brief Check if RTC is enabled
 * @return true if RTC counter is enabled, false otherwise
 */
bool RTC_IsEnabled(void);

/**
 * @brief Get current clock source setting
 * @return Current clock source selection (RTC_CLKSEL_*_gc values)
 */
uint8_t RTC_GetClockSource(void);

/**
 * @brief Set clock source for RTC
 * @param clockSource Clock source selection (use RTC_CLK_* constants)
 */
void RTC_SetClockSource(uint8_t clockSource);

/**
 * @brief Get current prescaler setting from CTRLA register
 * @return Current prescaler bits from CTRLA register
 */
uint8_t RTC_GetPrescaler(void);

/**
 * @brief Set prescaler value in CTRLA register
 * @param prescaler Prescaler setting (use RTC_PRESCALER_* constants)
 * @note This preserves other bits in CTRLA register
 */
void RTC_SetPrescaler(uint8_t prescaler);

/**
 * @brief Enable/disable run in standby mode
 * @param enable true to enable run in standby, false to disable
 */
void RTC_SetRunInStandby(bool enable);

/**
 * @brief Check if run in standby is enabled
 * @return true if run in standby is enabled, false otherwise
 */
bool RTC_IsRunInStandbyEnabled(void);

/**
 * @brief Enable/disable correction
 * @param enable true to enable correction, false to disable
 */
void RTC_SetCorrection(bool enable);

/**
 * @brief Check if correction is enabled
 * @return true if correction is enabled, false otherwise
 */
bool RTC_IsCorrectionEnabled(void);

/**
 * @brief Set calibration value
 * @param calibValue 8-bit calibration value for frequency adjustment
 */
void RTC_SetCalibration(uint8_t calibValue);

/**
 * @brief Get current calibration value
 * @return Current 8-bit calibration value
 */
uint8_t RTC_GetCalibration(void);

/**
 * @brief Configure PIT (Periodic Interrupt Timer) period
 * @param period PIT period setting (use RTC_PERIOD_* constants)
 */
void RTC_ConfigurePIT(uint8_t period);

/**
 * @brief Enable PIT
 */
void RTC_EnablePIT(void);

/**
 * @brief Disable PIT
 */
void RTC_DisablePIT(void);

/**
 * @brief Check if PIT is enabled
 * @return true if PIT is enabled, false otherwise
 */
bool RTC_IsPITEnabled(void);

/**
 * @brief Get PIT status
 * @return Current PIT status register value
 */
uint8_t RTC_GetPITStatus(void);

/**
 * @brief Clear PIT interrupt flag
 */
void RTC_ClearPITInterruptFlag(void);

/**
 * @brief Check if PIT interrupt is enabled
 * @return true if PIT interrupt is enabled, false otherwise
 */
bool RTC_IsPITInterruptEnabled(void);

/**
 * @brief Perform a complete RTC reset and reconfiguration
 * 
 * This function stops the RTC, clears all settings, and allows for
 * complete reconfiguration from a known state.
 */
void RTC_Reset(void);

/** @} */

#endif /* RTCDRIVER_H */

/** @}*/
