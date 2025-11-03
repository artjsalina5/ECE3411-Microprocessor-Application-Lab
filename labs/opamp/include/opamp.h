#ifndef OPAMP_H_INCLUDED
#define OPAMP_H_INCLUDED

#define F_CPU 16000000UL
#define __AVR_AVR128DB48__
#include "aos_timer.h"
#include "cpu.h"
#include "dac.h"
#include "labtest3.h"
#include "uart.h"
#include "ui.h"
#include "ui_adc.h"
#include "ui_dac.h"
#include "ui_eeprom.h"
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

// OPAMP timebase in microseconds: number of CLK_PER cycles per 1us minus 1
// Use integer math to avoid <math.h> and floating point
#define OPAMP_TIMEBASE_US ((uint8_t)(((F_CPU + 999999UL) / 1000000UL) - 1UL))
/**
  @Summary
    Initializes the OPAMP_Initialize.
  @Description
    This routine initializes the OPAMP_Initialize.
    This routine should only be called once during system initialization.
  @Preconditions
    None
  @Param
    None
   @Example
    <code>
    OPAMP_Initialize();
*/
void OPAMP_init(void);

void GPIO_opamp_init(void);
/**
 * @brief Enables the OPAMP System
 * @return None
 * @param None
 */
void OPAMP_EnableSystem(void);

/**
 * @brief Disables the OPAMP System
 * @return None
 * @param None
 */
void OPAMP_DisableSystem(void);

// OP0 Custom APIs

/**
 * @brief Sets OP0's positive input as per user selection
 * @return None
 * @param [in] Desired positive input
          For the available positive inputs, refer to OPAMP_MUXPOS_t enum

 from


 the device header file
 */
void OPAMP_SetOP0PositiveInMux(OPAMP_OP0INMUX_MUXPOS_t value);

/**
 * @brief Sets OP0's negative input as per user selection
 * @return None
 * @param [in] Desired negative input
          For the available negative inputs, refer to OPAMP_MUXNEG_t enum

 from


 the device header file
 */
void OPAMP_SetOP0NegativeInMux(OPAMP_OP0INMUX_MUXNEG_t value);

/**
 * @brief Sets the top resistor connection of the OP0's internal resistor



 ladder
 as per user selection
 * @return None
 * @param [in] Desired top resistor connection
          For the available top resistor connections, refer to
 OPAMP_MUXTOP_t



 enum from the device header file
 */
void OPAMP_SetOP0TopResMux(OPAMP_OP0RESMUX_MUXTOP_t value);

/**
 * @brief Sets the bottom resistor connection of OP0's internal resistor
 * ladder as per user selection
 * @return None
 * @param [in] Desired bottom resistor connection
          For the available bottom resistor connections, refer to OPAMP_MUXBOT_t
          enum from the device header file
 */
void OPAMP_SetOP0BottomResMux(OPAMP_OP0RESMUX_MUXBOT_t value);

/**
 * @brief Sets the R1 and R2 values of OP0's internal resistor ladder as per
    user selection
 * @return None
 * @param [in] Desired resistor selection
               For the available resistor values, refer to OPAMP_MUXWIP_t

 enum


 from the device header file
 */
void OPAMP_SetOP0WiperResMux(OPAMP_OP0RESMUX_MUXWIP_t value);

/**
 * @brief Sets OP0's settle time
 * @return None
 * @param [in] Desired settle time of 0 to 127 in us
 */
void OPAMP_SetOP0SettleTime(uint8_t settleTime);

/**
 * @brief Checks if OP0's settling time is finished
 * @return boolean
 * @param None
 */
bool OPAMP_IsOP0Settled(void);

/**
 * @brief Sets OP0's offset calibration
 * @return None
 * @param [in] Desired offset
 */
void OPAMP_SetOP0OffsetCalibration(uint8_t calValue);

#endif /* OPAMP_H_INCLUDED */
