
#define __AVR_AVR128DB48__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <util/delay.h>

#define RTC_PERIOD (511)

volatile uint16_t adcVal;
void ADC0_init(void);
uint16_t ADC0_read(void);
void ADC0_init(void) {
  /* Disable digital input buffer */
  PORTE.PIN0CTRL &= ~PORT_ISC_gm;
  PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  /* Disable pull-up resistor */
  PORTE.PIN0CTRL &= ~PORT_PULLUPEN_bm;
  /* CLK_PER divided by 4 */
  /* Internal reference */
  ADC0.CTRLC = ADC_PRESC_DIV4_gc | ADC_REFSEL_INTREF_gc;
  /* ADC Enable: enabled */
  /* 12-bit mode */
  ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_12BIT_gc;
}
/* Select ADC channel */
ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
uint16_t ADC0_read(void) {
  /* Start ADC conversion */
  ADC0.COMMAND = ADC_STCONV_bm;
  /* Wait until ADC conversion done */
  while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) {
    ;
  }
  /* Clear the interrupt flag by writing 1: */
  ADC0.INTFLAGS = ADC_RESRDY_bm;
}
return ADC0.RES;
int { main(void) ADC0_init(); }
adcVal = ADC0_read();
while {
  ;
}
(1)
