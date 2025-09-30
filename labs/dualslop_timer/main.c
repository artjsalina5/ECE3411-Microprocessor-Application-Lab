#define __AVR_AVR128DB48__
#define F_CPU 16000000UL
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/xmega.h>
#include <stdbool.h>

#define PER_VALUE 249 // For 1 kHz PWM with F_CPU=16 MHz, prescaler=64
#define CMP_MIN ((PER_VALUE + 1) * 5 / 100)  // 5%
#define CMP_MAX ((PER_VALUE + 1) * 95 / 100) // 95%

volatile uint16_t duty_cmp = (PER_VALUE + 1) * 50 / 100; // start at 50%

void init_cpu(void) {
  /* Enable crystal oscillator
   * with frequency range 16Mhz and 4K cycles start-up time
   */
  ccp_write_io((uint8_t *)&CLKCTRL.XOSCHFCTRLA,
               CLKCTRL_RUNSTDBY_bm | CLKCTRL_CSUTHF_4K_gc |
                   CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_SELHF_XTAL_gc |
                   CLKCTRL_ENABLE_bm);

  // Confirm the crystal oscillator start-up
  while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm)) {
    ; // Do nothing
  }

  // set the main clock to use XOSCH as source and enable the CLKOUT pin
  ccp_write_io((uint8_t *)&CLKCTRL.MCLKCTRLA,
               CLKCTRL_CLKSEL_EXTCLK_gc | CLKCTRL_CLKOUT_bm);

  // Wait for system oscillator chaning to complete
  while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
    ; // Do nothing
  }

  // Clear RUNSTDBY for power save when not in use
  ccp_write_io((uint8_t *)&CLKCTRL.XOSCHFCTRLA,
               CLKCTRL.XOSCHFCTRLA & ~CLKCTRL_RUNSTDBY_bm);
}
void CLOCK_CFD_CLKMAIN_init(void) {
  ccp_write_io((uint8_t *)&CLKCTRL.MCLKCTRLA,
               CLKCTRL_CFDSRC_CLKMAIN_gc | CLKCTRL_CFDEN_bm);

  ccp_write_io((uint8_t *)&CLKCTRL.MCLKINTCTRL,
               CLKCTRL_INTTYPE_bm | CLKCTRL_CFD_bm);
}

void init_button(void) {
  // PB5 = increase
  PORTB.DIRCLR = PIN5_bm;
  PORTB.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;

  // PB2 = decrease
  PORTB.DIRCLR = PIN2_bm;
  PORTB.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}

void init_led() {
  PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
  PORTD.OUTCLR = 0xFF; // all off
  PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
  PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
}

void init_tca0(void) {
  // Route TCA0 to PORTD
  PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc;

  // Single-slope PWM, enable compare channel 0
  TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

  // Set period and starting compare
  TCA0.SINGLE.PER = PER_VALUE;
  TCA0.SINGLE.CMP0 = duty_cmp;

  // Enable TCA0 with prescaler 64
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc | TCA_SINGLE_ENABLE_bm;
}

void update_duty(bool increase) {
  if (increase) {
    if (duty_cmp < CMP_MAX) {
      duty_cmp += (PER_VALUE + 1) / 100; // +1%
    }
  } else {
    if (duty_cmp > CMP_MIN) {
      duty_cmp -= (PER_VALUE + 1) / 100; // -1%
    }
  }
  TCA0.SINGLE.CMP0 = duty_cmp;
}

ISR(PORTB_PORT_vect) {
  if (PORTB.INTFLAGS & PIN5_bm) {
    update_duty(true);        // Increase duty
    PORTB.INTFLAGS = PIN5_bm; // Clear flag
  }
  if (PORTB.INTFLAGS & PIN2_bm) {
    update_duty(false);       // Decrease duty
    PORTB.INTFLAGS = PIN2_bm; // Clear flag
  }
}

int main(void) {
  init_cpu();
  init_button();
  init_led();
  init_tca0();
  sei();

  while (1) {
  }
}
