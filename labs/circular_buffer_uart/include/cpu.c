
#include <avr/cpufunc.h>

static inline void init_cpu(void) {
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_ENABLE_bm;
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_EXTCLK_gc;
}
void External_Crystal_init(void) {
  uint8_t buffer;

  /* Initialize 32.768kHz Oscillator: */
  /* Disable oscillator: */
  buffer = CLKCTRL.XOSCHFCTRLA;
  buffer &= ~CLKCTRL_ENABLE_bm;

  /* Writing to protected register */
  ccp_write_io((void *)&CLKCTRL.XOSCHFCTRLA, buffer);

  while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSCHFCTRLA_bm) {
    ; /* Wait until XOSCHFCTRLA becomes 0 */
  }

  /* SEL = 0 (Use External Crystal): */
  buffer = CLKCTRL.XOSCHFCTRLA;
  buffer &= ~CLKCTRL_SEL_bm;

  /* Writing to protected register */
  ccp_write_io((void *)&CLKCTRL.XOSCHFCTRLA, buffer);

  /* Enable oscillator: */
  buffer = CLKCTRL.XOSCHFCTRLA;
  buffer |= CLKCTRL_ENABLE_bm;
  /* Writing to protected register */
  ccp_write_io((void *)&CLKCTRL.XOSCHFCTRLA, buffer);
}
