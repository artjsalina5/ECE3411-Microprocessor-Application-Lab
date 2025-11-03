/**
 * @file opamp.c
 * @author Arturo Salinas
 * @date 2025-11-02
 * @brief OPAMP Lab
 * Students will use the AVR128DB48’s internal op-amps to build a 3-op-amp
 instrumentation
 * amplifier to measure the voltage drop across a shunt resistor, which
 corresponds to the current flowing through a load.
 ▪ Connect OP2 output to ADC0 and display the current value every 2 sec using
 USART.
 */

#include "opamp.h"

void OPAMP_init(void) {
  /* Configure the Timebase */
  OPAMP.TIMEBASE = OPAMP_TIMEBASE_US;
  /* Configure the voltage input range */
  OPAMP.PWRCTRL = OPAMP_PWRCTRL_IRSEL_FULL_gc;
  /* Configure OP0 as voltage follower for external network input (PD1) */
  OPAMP.OP0CTRLA = OPAMP_OP0CTRLA_OUTMODE_NORMAL_gc | OPAMP_ALWAYSON_bm;
  OPAMP.OP0INMUX = OPAMP_OP0INMUX_MUXNEG_OUT_gc | OPAMP_OP0INMUX_MUXPOS_INP_gc;
  OPAMP.OP0RESMUX = OPAMP_OP0RESMUX_MUXBOT_OFF_gc |
                    OPAMP_OP0RESMUX_MUXWIP_WIP0_gc |
                    OPAMP_OP0RESMUX_MUXTOP_OFF_gc;
  OPAMP.OP0SETTLE = 0x7F;
  /* Configure OP1 as voltage follower for external network input (PD7) */
  OPAMP.OP1CTRLA = OPAMP_OP1CTRLA_OUTMODE_NORMAL_gc | OPAMP_ALWAYSON_bm;
  OPAMP.OP1INMUX = OPAMP_OP1INMUX_MUXNEG_OUT_gc | OPAMP_OP1INMUX_MUXPOS_INP_gc;
  OPAMP.OP1RESMUX = OPAMP_OP1RESMUX_MUXBOT_OFF_gc |
                    OPAMP_OP1RESMUX_MUXWIP_WIP0_gc |
                    OPAMP_OP1RESMUX_MUXTOP_OFF_gc;
  OPAMP.OP1SETTLE = 0x7F;
  /* Configure OP2 for external difference amplifier stage */
  OPAMP.OP2CTRLA = OPAMP_OP2CTRLA_OUTMODE_NORMAL_gc | OPAMP_ALWAYSON_bm;
  OPAMP.OP2INMUX = OPAMP_OP2INMUX_MUXNEG_INN_gc | OPAMP_OP2INMUX_MUXPOS_INP_gc;
  OPAMP.OP2RESMUX = OPAMP_OP2RESMUX_MUXBOT_OFF_gc |
                    OPAMP_OP2RESMUX_MUXWIP_WIP0_gc |
                    OPAMP_OP2RESMUX_MUXTOP_OFF_gc;
  OPAMP.OP2SETTLE = 0x7F;
  /* Enable OPAMP peripheral */
  OPAMP.CTRLA = OPAMP_ENABLE_bm;
  /* Wait for the operational amplifiers to settle */
  while (((OPAMP.OP0STATUS & OPAMP_SETTLED_bm) == 0) ||
         ((OPAMP.OP1STATUS & OPAMP_SETTLED_bm) == 0) ||
         ((OPAMP.OP2STATUS & OPAMP_SETTLED_bm) == 0)) {
    ; // wait until all three op-amps report settled
  }
}
void GPIO_opamp_init(void) {
  /* Disable digital input buffer*/
  /*OP0*/
  PORTD.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTD.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  /*OP1*/
  PORTD.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTD.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
  /*OP2*/
  PORTE.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTE.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
}

void OPAMP_EnableSystem(void) { OPAMP.CTRLA |= OPAMP_ENABLE_bm; }

void OPAMP_DisableSystem(void) { OPAMP.CTRLA &= ~OPAMP_ENABLE_bm; }

// OP0 Custom APIs

void OPAMP_SetOP0PositiveInMux(OPAMP_OP0INMUX_MUXPOS_t value) {
  OPAMP.OP0INMUX = (OPAMP.OP0INMUX & ~OPAMP_MUXPOS_gm) | value;
}

void OPAMP_SetOP0NegativeInMux(OPAMP_OP0INMUX_MUXNEG_t value) {
  OPAMP.OP0INMUX = (OPAMP.OP0INMUX & ~OPAMP_MUXNEG_gm) | value;
}

void OPAMP_SetOP0TopResMux(OPAMP_OP0RESMUX_MUXTOP_t value) {
  OPAMP.OP0RESMUX = (OPAMP.OP0RESMUX & ~OPAMP_MUXTOP_gm) | value;
}

void OPAMP_SetOP0BottomResMux(OPAMP_OP0RESMUX_MUXBOT_t value) {
  OPAMP.OP0RESMUX = (OPAMP.OP0RESMUX & ~OPAMP_MUXBOT_gm) | value;
}

void OPAMP_SetOP0WiperResMux(OPAMP_OP0RESMUX_MUXWIP_t value) {
  OPAMP.OP0RESMUX = (OPAMP.OP0RESMUX & ~OPAMP_MUXWIP_gm) | value;
}

void OPAMP_SetOP0SettleTime(uint8_t settleTime) {
  if (settleTime > 127) {
    settleTime = 127;
  }
  OPAMP.OP0SETTLE = settleTime;
}

bool OPAMP_IsOP0Settled(void) { return (OPAMP.OP0STATUS & OPAMP_SETTLED_bm) != 0; }

void OPAMP_SetOP0OffsetCalibration(uint8_t calValue) {
  OPAMP.OP0CAL = calValue;
}
