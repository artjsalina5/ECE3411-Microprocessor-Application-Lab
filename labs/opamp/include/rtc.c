

#include "rtc.h"

void RTC_init(void) {
  /* Initialize RTC: */
  while (RTC.STATUS > 0) {
    ; /* Wait for all register to be synchronized */
  }

  /* Set period */
  RTC.PER = RTC_PERIOD;

  /* 32.768kHz Internal Crystal Oscillator */
  RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;

  RTC.CTRLA = RTC_PRESCALER_DIV32_gc /* 32 */
              | RTC_RUNSTDBY_bm;     /* Run In Standby: enabled */

  /* clear Interrupt flags */
  RTC.INTFLAGS = RTC_OVF_bm | RTC_CMP_bm;

  /* Enable Overflow Interrupt */
  RTC.INTCTRL |= RTC_OVF_bm;
}

void RTC_enable(void) { RTC.CTRLA |= RTC_RTCEN_bm; /* Enable */ }

ISR(RTC_CNT_vect) {
  /* Clear flag by writing '1': */
  RTC.INTFLAGS = RTC_OVF_bm;

  LED0_toggle();
}

void LED0_init(void) {
  // Set (PB3) as output
  PORTB.DIR |= PIN3_bm;
}

void LED0_toggle(void) {
  // Toggle LED0 (PB3)
  PORTB.OUTTGL = PIN3_bm;
}
