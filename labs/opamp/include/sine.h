/*
 * \file sine.h
 *
 * \brief
 *
 */

#ifndef SINE_H_
#define SINE_H_
#define F_CPU 16000000UL
#define __AVR_AVR128DB48__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#include "adc.h"

#define DEGREE 0.0245 // 2pi /256 = 6.28/ 256.

#define SINE_WAVE_TIMER TCB0

#define SINE_WAVE_TIMER_vect TCB0_INT_vect

/* VREF Startup time */
#define VREF_STARTUP_MICROS (25)

/* Resolution of DAC */
#define DAC0_RESOLUTION (0x3FF)

/*Amplitude Volts*/
#define SINE_WAVE_AMPLITUDE_VOLTS (0.128)
/*Offset Volts*/
#define SINE_DC_OFFSET_VOLTS (0.825)
/*DAC Voltage Reference*/
#define DAC_VOLTAGE_REF (2.048)
/*Offset Volts*/
#define VDD_DIV_2_VOLTS (1650)

/* Number of steps for a sine wave period */
#define SINE_WAVE_STEPS (92)
/* Sine wave amplitude */
#define SINE_AMPLITUDE                                                         \
  (uint16_t)(SINE_WAVE_AMPLITUDE_VOLTS / DAC_VOLTAGE_REF * DAC0_RESOLUTION)
/* Sine wave DC offset */
#define SINE_DC_OFFSET                                                         \
  (uint16_t)(SINE_DC_OFFSET_VOLTS / DAC_VOLTAGE_REF * DAC0_RESOLUTION)
/* 2*PI */
#define M_2PI (2 * M_PI)
/* Frequency of the sine wave */
#define OUTPUT_FREQ (25)

/* Buffer to store the sine wave samples (defined in sine.c) */
extern uint16_t sine_wave[SINE_WAVE_STEPS];

void DAC0_sine_init(void);
void DAC0_sine_setVal(uint16_t val);

void sine_wave_table_init0(void);

void DAC0_SineWaveTimer_init(void);

void DAC0_SineEaveTimer_enable(void);

#endif /* SINE_H_ */
