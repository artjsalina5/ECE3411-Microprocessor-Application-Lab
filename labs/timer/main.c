#define __AVR_AVR128DB48__
#define F_CPU 16000000
#include <avr/io.h>
// #include <avr/ioavr128db48.h>
#include <avr/interrupt.h>
#include <avr/xmega.h>
#include <util/delay.h>

void init_cpu() {
  _PROTECTED_WRITE(CLKCTRL.XOSCHFCTRLA, CLKCTRL.XOSCHFCTRLA |
                                            CLKCTRL_FRQRANGE_16M_gc |
                                            CLKCTRL_ENABLE_bm);
}
void init_pins() { PORTD_DIRSET = 0xFF; }

void init_tca0() {

  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;

  TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTAEI_bm & TCA_SINGLE_CNTBEI_bm);

  TCA0.SINGLE.PER = 0x3d08; // 15625

  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

ISR(TCA0_OVF_vect) {
  PORTD.OUTTGL = 0b1010010;

  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

int main() {
  init_cpu();
  init_pins();
  init_tca0();
  sei();
  while (1) {
  }
}
