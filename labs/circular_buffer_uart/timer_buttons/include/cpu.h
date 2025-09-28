/**
 * @file cpu.h
 * @author Arturo Salinas
 * @date 2025-09-24
 * @brief CPU Clock Configuration Module Header
 * 
 * @defgroup cpu CPU Configuration
 * @{
 * 
 * This header file provides function declarations and configuration constants
 * for CPU clock initialization and external crystal oscillator setup on
 * AVR128DB48 microcontroller.
 */

#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cpu_config CPU Configuration Constants
 * @ingroup cpu
 * @{
 */

/** @defgroup cpu_clock_sources Clock Source Selection
 *  @brief Available clock sources for system clock
 *  @{
 */
#define CPU_CLK_INT_OSC20M      (0x0)  /**< Internal 20MHz Oscillator */
#define CPU_CLK_INT_OSC32K      (0x1)  /**< Internal 32.768kHz Ultra Low Power Oscillator */
#define CPU_CLK_XOSC32K         (0x2)  /**< 32.768kHz Crystal Oscillator */
#define CPU_CLK_EXTCLK          (0x3)  /**< External Clock */
/** @} */

/** @defgroup cpu_prescaler Main Clock Prescaler Options
 *  @brief Prescaler values for main system clock
 *  @{
 */
#define CPU_PRESCALER_DIV1      (0x0 << 1)  /**< No division */
#define CPU_PRESCALER_DIV2      (0x1 << 1)  /**< Divide by 2 */
#define CPU_PRESCALER_DIV4      (0x2 << 1)  /**< Divide by 4 */
#define CPU_PRESCALER_DIV8      (0x3 << 1)  /**< Divide by 8 */
#define CPU_PRESCALER_DIV16     (0x4 << 1)  /**< Divide by 16 */
#define CPU_PRESCALER_DIV32     (0x5 << 1)  /**< Divide by 32 */
#define CPU_PRESCALER_DIV64     (0x6 << 1)  /**< Divide by 64 */
/** @} */

/** @} */

/**
 * @brief Initialize CPU with basic external clock configuration
 * @ingroup cpu
 * 
 * This function configures the CPU to use external clock source.
 * It performs protected writes to clock control registers to enable
 * the external high-frequency oscillator and switch the main clock.
 * 
 * @note This is a basic initialization. For more advanced configuration,
 *       use External_Crystal_init() function first.
 * 
 * @return void
 */
void init_cpu(void);

/**
 * @brief Initialize external crystal oscillator with detailed configuration
 * @ingroup cpu
 * 
 * This function provides comprehensive configuration of the external high-frequency
 * crystal oscillator. It properly disables the oscillator, configures it to use
 * external crystal (not external clock), and then re-enables it.
 * 
 * The function uses protected I/O writes to ensure proper configuration of
 * clock control registers that are protected by the Configuration Change Protection.
 * 
 * Configuration sequence:
 * 1. Disable the external oscillator
 * 2. Wait for oscillator to be disabled
 * 3. Configure for external crystal mode (not external clock)
 * 4. Re-enable the oscillator
 * 
 * @note This function should be called before switching to external clock source
 *       as the main system clock. After calling this function, you can call
 *       init_cpu() to switch the main clock to the external source.
 * 
 * @warning Ensure external crystal is properly connected before calling this function
 * 
 * @return void
 */
void External_Crystal_init(void);

/**
 * @brief Configure main clock with specific source and prescaler
 * @ingroup cpu
 * 
 * This function allows detailed configuration of the main system clock
 * including source selection and prescaler setting.
 * 
 * @param clock_source Clock source selection (use CPU_CLK_* constants)
 * @param prescaler Prescaler setting (use CPU_PRESCALER_* constants)
 * 
 * @return 0 on success, -1 on invalid parameters
 * 
 * Example usage:
 * @code
 * // Initialize external crystal first
 * External_Crystal_init();
 * 
 * // Configure main clock to use external clock with no prescaling
 * CPU_ConfigureClock(CPU_CLK_EXTCLK, CPU_PRESCALER_DIV1);
 * @endcode
 */
int8_t CPU_ConfigureClock(uint8_t clock_source, uint8_t prescaler);

/**
 * @brief Get current main clock frequency in Hz
 * @ingroup cpu
 * 
 * This function calculates and returns the current main clock frequency
 * based on the selected clock source and prescaler settings.
 * 
 * @return Current main clock frequency in Hz
 * 
 * @note For external clock sources, this function assumes standard
 *       crystal frequencies (16MHz for external crystal, 32768Hz for 32kHz crystal)
 */
uint32_t CPU_GetClockFrequency(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CPU_H */