#ifndef CPU_H_
#define CPU_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
/**
*@brief Initializes the High Frequency Crystal Oscillator to 16Mhz
*@params None
*@return None
 */
void CLOCK_XOSCHF_16M_init(void);
/**
*@brief Initializes the High Frequency Crystal Oscillator to 24Mhz
*@params None
*@return None
 */
void CLOCK_XOSCHF_24M_init(void);
/**
*@brief Initializes Clock Failure Detection
*@params None
*@return None
 */
void CLOCK_CFD_CLKMAIN_init(void);
/**
*@brief Initializes the 16Mhz Internal Oscillator
*@params None
*@return None
 */
void CLOCK_OSC_16M_init(void);
/**
*@brief Initializes the 24Mhz Internal Oscillator
*@params None
*@return None
 */
void CLOCK_OSC_24M_init(void);
#endif /* CPU_H_ */