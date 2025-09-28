/*
 * AVRDx specific UART code
 */

/* CPU frequency */
#define F_CPU 16000000UL

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "uart.h"

void *usart_init(uint8_t usartnum, uint32_t baud_rate) {
  USART_t *usart;

  if (usartnum == 0) {
    usart = &USART0;
    // enable USART0 TX pin
    PORTA.DIRSET = PIN0_bm;
  } else if (usartnum == 1) {
    usart = &USART1;
    // enable USART1 TX pin
    PORTC.DIRSET = PIN0_bm;
  } else if (usartnum == 2) {
    usart = &USART2;
    // enable USART2 TX pin
    PORTF.DIRSET = PIN0_bm;
  } else if (usartnum == 3) {
    usart = &USART3;
    // enable USART3 TX pin
    PORTB.DIRSET = PIN0_bm;
  } else {
    usart = NULL;
  }

  // set BAUD and CTRLB registers

  return usart;
}

void usart_transmit_data(void *ptr, char c) {
  USART_t *usart = (USART_t *)ptr;
  // TODO send data
}

void usart_wait_until_transmit_ready(void *ptr) {
  USART_t *usart = (USART_t *)ptr;
  // TODO wait until UART is ready to transmit
}

int usart_receive_data(void *ptr) {
  USART_t *usart = (USART_t *)ptr;
  // TODO wait until data has arrived and then return the data
}
