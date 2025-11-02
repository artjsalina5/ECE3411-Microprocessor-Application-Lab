#ifndef DAC_H_
#define DAC_H_
#define F_CPU 16000000UL // 16 MHz clock speed
#define __AVR_AVR128DB48__
#include <avr/io.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
/**
 *@file dac.h
 *@brief Contains functions to initialize the DAC0 peripheral and generate a
 * sine wave in memory
 *@author Arturo Salinas
 */
void sine_wave_init(void);

void DAC_init(void);

void DAC0_setVal(uint16_t value);

#endif // DAC_H_
