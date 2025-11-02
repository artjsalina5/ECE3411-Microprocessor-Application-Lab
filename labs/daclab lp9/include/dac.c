#include "dac.h"
#include <math.h>

// Global sine wave table (64 samples)
uint16_t sineWave[64];

// Pre-scaled sine wave table (applies amplitude scaling)
uint16_t sine_wave_scaled[64];

// Compute sine wave table dynamically
// sine_period_steps = 64 (samples per period)
// sine_amplitude = 511 (half of 1023 for 10-bit DAC)
// sine_dc_offset = 512 (mid-point of 1023)
void sine_wave_init(void) {
  uint8_t i;
  for (i = 0; i < 64; i++) {
    // sin(2*pi*i/64) produces values from -1 to 1
    // Multiply by amplitude (511) and add DC offset (512)
    sineWave[i] = 512 + 511 * sin(2.0 * M_PI * i / 64.0);
  }
  
  // Initialize scaled table to match original (100% amplitude)
  for (i = 0; i < 64; i++) {
    sine_wave_scaled[i] = sineWave[i];
  }
}

// Update the scaled sine wave table based on amplitude percentage
void update_sine_wave_scaled(int amplitude_percent) {
  float scaler = (float)amplitude_percent / 100.0f;
  for (uint8_t i = 0; i < 64; i++) {
    sine_wave_scaled[i] = (uint16_t)(sineWave[i] * scaler);
  }
}

void DAC_init(void) {
  // The DAC output pin needs to have the digital input buffer and the pull-up
  // resistor disabled in order to reduce its load.
  PORTD.PIN6CTRL &= ~PORT_ISC_gm;
  PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;

  // Configure DAC0 with output enable and run in standby
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm | DAC_RUNSTDBY_bm;
  // Set reference voltage to VDD (3.3V) with always on mode
  VREF.DAC0REF = VREF_REFSEL_VDD_gc | VREF_ALWAYSON_bm;

  // Wait for VREF startup time
  _delay_us(50);
}

void DAC0_setVal(uint16_t value) {
  DAC0.DATAL = (value & (0x03)) << 6;
  DAC0.DATAH = value >> 2;
}
