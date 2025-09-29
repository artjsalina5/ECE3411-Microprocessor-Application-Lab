#define __AVR_AVR128DB48__
#define F_CPU 8000000
#include <avr/io.h>
// #include <avr/ioavr128db48.h>
#include <avr/interrupt.h>
#include <avr/xmega.h>
#include <util/delay.h>

void init_cpu() {
  _PROTECTED_WRITE(CLKCTRL.XOSCHFCTRLA, CLKCTRL.XOSCHFCTRLA |
                                            CLKCTRL_FRQRANGE_8M_gc |
                                            CLKCTRL_ENABLE_bm);
}
void init_pins() { PORTD.DIR |= PIN0_bm | PIN3_bm; }

void init_tca0() {
  PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc;

  TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;

  TCA0_SPLIT_CTRLB = TCA_SPLIT_HCMP0OV_bm | TCA_SPLIT_LCMP0OV_bm;

  TCA0_SPLIT_LPER = 0xdf;
  TCA0_SPLIT_HPER = 0x9c;

  TCA0_SPLIT_LCMP0 = 0x70;
  TCA0_SPLIT_HCMP0 = 0x4e;

  TCA0_SPLIT_CTRLA = TCA_SPLIT_CLKSEL_DIV1024_gc | TCA_SPLIT_ENABLE_bm;
}

void reset_tca0() {
  TCA0_SINGLE_CTRLA &= ~(TCA_SINGLE_ENABLE_bm);
  TCA0_SINGLE_CTRLESET = TCA_SINGLE_CMD_RESET_gc;
}

ISR(TCA0_OVF_vect) {
  PORTD.OUTTGL = 0b1010010;

  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

int main() {
  init_cpu();
  init_pins();
  reset_tca0();
  init_tca0();
  // sei();
  while (1) {
  }
}
