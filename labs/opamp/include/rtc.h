#ifndef RTC_H_
#define RTC_H_

#include <avr/interrupt.h>
#include <avr/io.h>

#define F_CPU 16000000UL
#define __AVR_AVR128DB48__
/* RTC Period */
#define RTC_PERIOD (511)

void RTC_init(void);
void RTC_enable(void);

void LED0_init(void);
void LED0_toggle(void);

#endif /* RTC_H_ */
