#include "adc.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// Note: Original integration referenced a data_visualizer stream.
// This stub removes that dependency to keep ADC module buildable.

void ADC0_init(void) {
  /* Disable digital input buffer and pull up resistor*/
  PORTD.PIN2CTRL &= PORT_ISC_INPUT_DISABLE_gc;
  PORTD.PIN5CTRL &= PORT_ISC_INPUT_DISABLE_gc;
  PORTD.PIN6CTRL &= PORT_ISC_INPUT_DISABLE_gc;

  ADC0.CTRLC = ADC_PRESC_DIV12_gc; /* CLK_PER divided by 12 */

  VREF.ADC0REF = VREF_ALWAYSON_bm | VREF_REFSEL_VDD_gc;

  ADC0.CTRLA = ADC_ENABLE_bm          /* ADC Enable: enabled */
               | ADC_RESSEL_10BIT_gc; /* 10-bit mode */

  ADC0.MUXPOS = ADC_MUXPOS_AIN10_gc; // OPAMP2 Output

  /* Result Ready Interrupt Enable */
  ADC0.INTCTRL = ADC_RESRDY_bm;

  /* Set the accumulator mode to accumulate 8 samples */
  ADC0.CTRLB = ADC_SAMPLES;

  ADC0_SampleTimer_init();
}

// Optional: If needed, an ADC0 result-ready ISR can be added by the consumer module.

void ADC0_StartConversion(void) {
  /* Start ADC conversion */
  ADC0.COMMAND = ADC_STCONV_bm;
}

void ADC0_SampleTimer_init(void) {
  /* Timer configured to overflow at ...Hz */
  ADC_SAMPLE_TIMER.CCMP = (F_CPU / SAMPLE_NUM) / OUTPUT_FREQ;
  /* Clock prescaler */
  ADC_SAMPLE_TIMER.CTRLA = TCB_CLKSEL_DIV1_gc;
  ADC_SAMPLE_TIMER.INTCTRL = TCB_CAPT_bm;
}

void ADC0_SampleTimer_enable(void) { ADC_SAMPLE_TIMER.CTRLA |= TCB_ENABLE_bm; }

// Optional: If needed, a TCB-based sampling ISR can be added by the consumer module.
