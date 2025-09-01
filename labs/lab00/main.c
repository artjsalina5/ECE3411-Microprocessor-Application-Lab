/*
 * @file   main.c
 * @author artjs
 *
 * @date Created on August 25, 2025, 4:03 PM
 */

#define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    /* Replace with your application code */
    PORTB_DIRSET = PIN3_bm;
    PORTD_DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm |
                   PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
    
    while (1) {
        PORTB_OUTTGL = PIN3_bm;
        PORTD_OUTTGL = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm |
                   PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;

        _delay_ms(1000);
        
    }
}

