
/**
 * @file cpu.c
 * @author Arturo Salinas
 * @date 2025-09-24
 * @brief CPU Clock Configuration Module
 * 
 * @defgroup cpu CPU Configuration
 * @{
 * 
 * This module provides functions for configuring the CPU clock and external crystal oscillator
 * for AVR128DB48 microcontroller. It includes initialization routines for both internal
 * and external clock sources with proper protected register writes.
 */

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/ioavr128db48.h>
#include <stdint.h>
#include "cpu.h"

/**
 * @brief Initialize CPU with basic clock configuration
 * @ingroup cpu
 * 
 * This function configures the CPU to use external clock source.
 * It performs protected writes to clock control registers to enable
 * the external high-frequency oscillator and switch the main clock.
 * 
 * @note This is a basic initialization. For more advanced configuration,
 *       use External_Crystal_init() function.
 * 
 * @return void
 */
void init_cpu(void) {
  // Use CCP to write to protected registers  
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_ENABLE_bm;
  
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_EXTCLK_gc;
}

/**
 * @brief Initialize external crystal oscillator
 * @ingroup cpu
 * 
 * This function provides detailed configuration of the external high-frequency
 * crystal oscillator. It properly disables the oscillator, configures it to use
 * external crystal (not external clock), and then re-enables it.
 * 
 * The function uses protected I/O writes to ensure proper configuration of
 * clock control registers that are protected by the Configuration Change Protection.
 * 
 * @note This function should be called before switching to external clock source
 *       as the main system clock.
 * 
 * @return void
 */
void External_Crystal_init(void) {
  uint8_t buffer;

  /** 
   * Initialize High-Frequency Crystal Oscillator:
   * Step 1: Disable oscillator
   */
  buffer = CLKCTRL.XOSCHFCTRLA;
  buffer &= ~CLKCTRL_ENABLE_bm;

  /* Writing to protected register */
  protected_write_io((void *)&CLKCTRL.XOSCHFCTRLA, CCP_IOREG_gc, buffer);

  /* Wait for oscillator to stop - check EXTS bit in status register */
  while (CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm) {
    ; /* Wait until external clock source is not active */
  }

  /**
   * Step 2: Configure to use External Crystal (SEL = 0)
   * SELHF_bm bit: 0 = External Crystal, 1 = External clock on XTALHF1 pin
   */
  buffer = CLKCTRL.XOSCHFCTRLA;
  buffer &= ~CLKCTRL_SELHF_bm;  /* Select External Crystal mode */

  /* Writing to protected register */
  protected_write_io((void *)&CLKCTRL.XOSCHFCTRLA, CCP_IOREG_gc, buffer);

  /**
   * Step 3: Enable oscillator
   */
  buffer = CLKCTRL.XOSCHFCTRLA;
  buffer |= CLKCTRL_ENABLE_bm;
  /* Writing to protected register */
  protected_write_io((void *)&CLKCTRL.XOSCHFCTRLA, CCP_IOREG_gc, buffer);
}


/**
 * @brief Configure main clock with specific source and prescaler  
 * @ingroup cpu
 * 
 * @param clock_source Clock source selection
 * @param prescaler Prescaler setting
 * @return 0 on success, -1 on invalid parameters
 */
int8_t CPU_ConfigureClock(uint8_t clock_source, uint8_t prescaler) {
    uint8_t config_value;
    
    // Validate parameters
    if (clock_source > 3) {
        return -1; // Invalid clock source
    }
    
    // Configure main clock control register
    config_value = clock_source | prescaler;
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA = config_value;
    
    return 0;
}

/**
 * @brief Get current main clock frequency in Hz
 * @ingroup cpu
 * 
 * @return Current main clock frequency in Hz
 */
uint32_t CPU_GetClockFrequency(void) {
    uint32_t base_freq;
    uint8_t prescaler_bits;
    uint16_t prescaler_value;
    
    // Get current clock source
    uint8_t clock_source = CLKCTRL.MCLKCTRLA & 0x03;
    
    // Determine base frequency based on clock source
    switch(clock_source) {
        case CLKCTRL_CLKSEL_OSCHF_gc:
            base_freq = 4000000UL;   // 4MHz default internal oscillator
            break;
        case CLKCTRL_CLKSEL_OSC32K_gc:
            base_freq = 32768UL;     // 32.768kHz
            break;
        case CLKCTRL_CLKSEL_XOSC32K_gc:
            base_freq = 32768UL;     // 32.768kHz crystal
            break;
        case CLKCTRL_CLKSEL_EXTCLK_gc:
            base_freq = 16000000UL;  // Assume 16MHz external clock
            break;
        default:
            base_freq = 4000000UL;   // Default to internal 4MHz
            break;
    }
    
    // Get prescaler setting  
    if (CLKCTRL.MCLKCTRLB & CLKCTRL_PEN_bm) {
        // Prescaler is enabled
        prescaler_bits = (CLKCTRL.MCLKCTRLB >> 1) & 0x0F;
        switch(prescaler_bits) {
            case 0x00: prescaler_value = 2; break;   // DIV2
            case 0x01: prescaler_value = 4; break;   // DIV4  
            case 0x02: prescaler_value = 8; break;   // DIV8
            case 0x03: prescaler_value = 16; break;  // DIV16
            case 0x04: prescaler_value = 32; break;  // DIV32
            case 0x05: prescaler_value = 64; break;  // DIV64
            case 0x08: prescaler_value = 6; break;   // DIV6
            case 0x09: prescaler_value = 10; break;  // DIV10
            case 0x0A: prescaler_value = 12; break;  // DIV12
            case 0x0B: prescaler_value = 24; break;  // DIV24
            case 0x0C: prescaler_value = 48; break;  // DIV48
            default: prescaler_value = 1; break;     // No division
        }
    } else {
        prescaler_value = 1;  // No prescaling
    }
    
    return base_freq / prescaler_value;
}

/** @} */

/** @} */
