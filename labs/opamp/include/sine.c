
#include "sine.h"

volatile uint16_t sine_wave_index = 0;
// Define the sine wave buffer declared in sine.h
uint16_t sine_wave[SINE_WAVE_STEPS];

void sine_wave_table_init0(void) {
  for (uint16_t i = 0; i < SINE_WAVE_STEPS; i++) {
    sine_wave[i] = SINE_DC_OFFSET +
                   ((float)SINE_AMPLITUDE) * sin(i * M_2PI / SINE_WAVE_STEPS);
  }
}

void DAC0_sine_init(void) {
  /*Create sinusoidal wave table of values*/
  sine_wave_table_init0();

  /* DAC output pin */
  /* Disable digital input buffer */
  PORTD.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;

  /* Select DAC Voltage reference and always on */
  VREF.DAC0REF |= VREF_REFSEL_2V048_gc | VREF_ALWAYSON_bm;

  /* Wait VREF start-up time */
  _delay_us(VREF_STARTUP_MICROS);

  DAC0_sine_setVal(SINE_DC_OFFSET);

  /* Enable DAC output to pin */
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;

  DAC0_SineWaveTimer_init();
}

void DAC0_sine_setVal(uint16_t val) { DAC0.DATA = val; }

void DAC0_SineWaveTimer_init(void) {
  /** Timer configured to overflow at ... **/
  SINE_WAVE_TIMER.CCMP = (F_CPU / SINE_WAVE_STEPS) / OUTPUT_FREQ;
  /* Clock prescaler */
  SINE_WAVE_TIMER.CTRLA = TCB_CLKSEL_DIV1_gc;
  SINE_WAVE_TIMER.INTCTRL = TCB_CAPT_bm;
}

void DAC0_SineEaveTimer_enable(void) { SINE_WAVE_TIMER.CTRLA |= TCB_ENABLE_bm; }

// Note: SINE_WAVE_TIMER ISR removed to avoid conflicts with centralized timer module.
