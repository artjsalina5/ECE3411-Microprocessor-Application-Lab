/*
 * Intro to UART
 * Author: A. Salinas
 * Date: 09/15/25
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/ioavr128db48.h>

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <uart.h>
#include <util/delay.h>

// prep for 9600 8N1 operation
// #define BAUD_RATE 9600

volatile uint16_t global_freq = 2;

/*
static inline void USART3_INIT(uint32_t baud) {
  // Calculate baud rate register value
  // S = 16 in NORMAL mode or 8 in CLK2X mode
  uint16_t baud_setting = (F_CPU * 64) / (16 * baud);

  // Set baud rate
  USART3.BAUD = baud_setting;

  // Set frame format: 8 data bits, no parity, 1 stop bit
  USART3.CTRLC = USART_CHSIZE_8BIT_gc;

  PORTB.DIRSET = PIN0_bm; // USART3_TX
  PORTB.DIRCLR = PIN1_bm; // USART3_RX

  USART3.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

void USART3_SendChar(char data) {
  // Wait for the transmit buffer to be empty
  while (!(USART3.STATUS & USART_DREIF_bm))
    ;

  // send data
  USART3.TXDATAL = data;
}

void USART3_SendString(const char *str) {
  while (*str) {
    USART3_SendChar(*str);
    str++;
  }
}

char USART3_ReceiveChar(void) {
  // wait for receive complete
  while (!(USART3.STATUS & USART_RXCIF_bm))
    ;

  // Return received data
  return USART3.RXDATAL;
}
*/

/* --------- LED setup ---------
 * Use 8 LEDs on PORTD pins PD0..PD7.
 */
static inline void leds_init(void) {
  PORTD.DIRSET = 0xFF; /* PD0..PD7 as outputs */
  PORTD.OUTCLR = 0xFF; /* all off */
}

/* Drive exactly one LED (0..7). Any out-of-range value turns all off. */
static inline void leds_set_position(uint8_t pos) {
  if (pos < 8U) {
    PORTD.OUT = (uint8_t)(1U << pos);
  } else {
    PORTD.OUTCLR = 0xFF;
  }
}

/* Toggle the currently selected LED without affecting others. */
static inline void leds_toggle_position(uint8_t pos) {
  if (pos < 8U) {
    PORTD.OUTTGL = (uint8_t)(1U << pos);
  }
}

/* --------- Clock to 16 MHz HF internal --------- */
static inline void clock_init_16mhz(void) {
  CPU_CCP = CCP_IOREG_gc;                     /* unlock protected IO regs */
  CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_16M_gc; /* internal HF osc 16 MHz */
}

/* Clamp helpers */
static inline uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi) {
  if (v < lo) {
    return lo;
  } else if (v > hi) {
    return hi;
  } else {
    return v;
  }
}

/* Compute half-period in ms from frequency in Hz.
 * half_ms = round(500 / f), min 1 ms, max 500 ms (for 1 Hz).
 */
static inline uint16_t half_ms_from_freq(uint8_t freq_hz) {
  if (freq_hz == 0U) {
    return 500U;
  }
  uint16_t half = (uint16_t)((500U + (freq_hz / 2U)) / freq_hz); /* rounded */
  if (half == 0U) {
    half = 1U;
  } else if (half > 500U) {
    half = 500U;
  }
  return half;
}
/*
ISR(PORTB_PORT_vect) {
  if (PORTB.INTFLAGS & PIN1_bm) {

    PORTB.INTFLAGS = PIN1_bm; // Clear the interrupt flag
  } else if (PORTB.INTFLAGS & PIN5_bm) {
    PORTB.INTFLAGS = PIN1_bm; // Clear the interrupt flag
  }
}

void Ext_Int_Init() {
  PORTB.DIRCLR = PIN1_bm; // Set USART3_RX PB1 as input to interrupt
  PORTB.PIN2CTRL = PORT_ISC_BOTHEDGES_gc; // Both Edge interrupt
  sei();                                  // Enable global interrupts
}
*/
static void prompt_and_handle_menu(uint8_t *freq_hz, uint8_t *pos) {
  /* Print the top-level menu and read F/P */
  printf("\nDo you want to change the frequency or position? (F/P)\n");
  printf("> ");
  fflush(stdout);

  char choice = 0;
  /* Use " %c" to skip whitespace/newlines */
  if (scanf(" %c", &choice) != 1) {
    printf("Input error.\n");
    return;
  }

  if (choice == 'F' || choice == 'f') {
    unsigned int newf = 0U;
    printf("Frequency (1-10 Hz):\n> ");
    fflush(stdout);
    if (scanf("%u", &newf) != 1) {
      printf("Input error.\n");
      return;
    }
    uint8_t nf = clamp_u8((uint8_t)newf, 1U, 10U);
    if (nf != newf) {
      printf("Out of range. Clamped to %u Hz.\n", (unsigned)nf);
    }
    *freq_hz = nf;
    printf("OK. Frequency set to %u Hz.\n", (unsigned)*freq_hz);
  } else if (choice == 'P' || choice == 'p') {
    unsigned int newp = 0U;
    printf("Position (0-7):\n> ");
    fflush(stdout);
    if (scanf("%u", &newp) != 1) {
      printf("Input error.\n");
      return;
    }
    uint8_t np = clamp_u8((uint8_t)newp, 0U, 7U);
    if (np != newp) {
      printf("Out of range. Clamped to %u.\n", (unsigned)np);
    }
    *pos = np;
    /* Immediately update solid ON state to the new position (off or on
     * preserved by next toggle) */
    leds_set_position(*pos);
    printf("OK. Position set to %u.\n", (unsigned)*pos);
  } else {
    printf("Unrecognized option '%c'. Please enter F or P next time.\n",
           choice);
  }
}
int main(void) {
  clock_init_16mhz();
  leds_init();

  /* Initialize UART stdio on USART3 @ 9600 8N1 */
  uart_init(3, 9600, NULL);
  printf("\n[UART READY] AVR128DB48 – LED control via UART. Starting at 2 Hz "
         "on PD0.\n");

  /* State */
  uint8_t freq_hz = 2U; /* starts at 2 Hz */
  uint8_t led_pos = 0U; /* starts at PD0  */
  uint16_t half_ms = half_ms_from_freq(freq_hz);

  /* Timekeeping using 10 ms ticks */
  uint16_t tick10_ms = 0U; /* accumulates to half_ms */
  uint16_t prompt_ms = 0U; /* accumulates to 5000 ms */

  /* Initialize LED output for the current position */
  leds_set_position(led_pos);
  bool led_on_phase = true; /* track phase for clarity */

  while (1) {
    /* 10 ms base tick */
    _delay_ms(10);
    tick10_ms += 10U;
    prompt_ms += 10U;

    /* Toggle at half-period boundary */
    if (tick10_ms >= half_ms) {
      leds_toggle_position(led_pos);
      led_on_phase = !led_on_phase;
      tick10_ms = 0U;
    }

    /* Every ~5 seconds: prompt user and handle potential change */
    if (prompt_ms >= 5000U) {
      prompt_ms = 0U;
      /* Blocking interaction; OK for lab */
      prompt_and_handle_menu(&freq_hz, &led_pos);
      /* Recompute derived timing in case frequency changed */
      half_ms = half_ms_from_freq(freq_hz);
      /* Make sure only the selected LED is driven after interaction */
      if (led_on_phase) {
        leds_set_position(led_pos);
      } else {
        /* If we were "off" in the current phase, keep it off but on the new
         * pin. */
        PORTD.OUTCLR = 0xFF;
      }
      printf("Now blinking PD%u at %u Hz.\n", (unsigned)led_pos,
             (unsigned)freq_hz);
    }
  }

  /* not reached */
  return 0;
}
