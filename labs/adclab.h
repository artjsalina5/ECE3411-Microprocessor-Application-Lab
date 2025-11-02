#ifndef ADCLAB_H_
#define ADCLAB_H_
#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

// Initalizes the 16M External clock and enables it
// Sets the main clock controller to the external clock
void init_clock();
// Initalizes the DAC0 to be set as an output
// Sets the reference voltage as the VDD from the USB JTAG interface
void DAC_init(void);

/**
 @brief Initializes the timer to a Period given by the global @param freq
 Sets the timer to a normal overflow operation
  Calculates period based on proper formula
 Sets the Interrupt Control mode to Interupt mode
  Sets the divisor to 1, and enables the peripheral
*/
void InitTimerTCA0();

void frequency_update(uint32_t new_freq);
/**
 * @brief Subroutine to update the amplitude to accomplish amplitude scaling
 * of the Sine Wave
 */
void amplitude_update(int new_amplitude_percent);

/**
 * @brief Sends current Status to UART for debug
 */
void status_printing();

/**
 * @brief UI for the UART Printing
 */
void question_handler();
/**
 * @brief Performs the adclab test
 */
int adclab(void);

#endif // ADCLAB_H_
