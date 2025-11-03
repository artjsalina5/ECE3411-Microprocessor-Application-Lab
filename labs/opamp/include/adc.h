#ifndef ADC_H_
#define ADC_H_

#define F_CPU 16000000UL
#define __AVR_AVR128DB48__

#include "sine.h"
#include "uart.h"

#define ADC_SAMPLE_TIMER TCB1

#define ADC_SAMPLE_TIMER_vect TCB1_INT_vect

#define ADC_SAMPLES ADC_SAMPNUM_ACC8_gc

#define ADC_MILI_VOLTAGE_REF (3300)

/*Amount of samples to acquire*/
#define SAMPLE_NUM (32)

void ADC0_init(void);
void ADC0_StartConversion(void);

void ADC0_SampleTimer_init(void);

void ADC0_SampleTimer_enable(void);

#endif
