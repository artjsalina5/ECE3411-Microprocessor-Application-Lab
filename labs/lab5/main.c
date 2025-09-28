/*
 * UART Interrupts
 * Author: A. Salinas
 * Date: 09/17/25
 */

#define F_CPU 16000000UL
#include <avr/io.h>
// #include <avr/ioavr128db48.h>

#include "include/uart.h"
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
// #include <uart.h>
#include <util/delay.h>

// prep for 9600 8N1 operation
#define BAUD_RATE 9600
#define BUFFER_SIZE 64

// Circular buffer structure
typedef struct {
  char buffer[BUFFER_SIZE];
  volatile uint8_t head;
  volatile uint8_t tail;
  volatile uint8_t count;
} circular_buffer_t;

volatile circular_buffer_t tx_buffer = {0};
volatile circular_buffer_t rx_buffer = {0};
void buffer_put(volatile circular_buffer_t *buf, char data) {
  if (buf->count < BUFFER_SIZE) {
    buf->buffer[buf->head] = data;
    buf->head = (buf->head + 1) % BUFFER_SIZE;
    buf->count++;
  }
}
bool buffer_get(volatile circular_buffer_t *buf, char *data) {
  if (buf->count > 0) {
    *data = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1) % BUFFER_SIZE;
    buf->count--;
    return true;
  }
  return false;
}

void USART3_Init(uint32_t baud) {
  // Set baud rate to 9600
  // USART3.BAUD = (F_CPU * 64) / (16 * 9600);
  uint16_t baud_setting = (F_CPU * 64) / (16 * baud);

  // Set baud rate
  USART3.BAUD = baud_setting;
  // Enable interrupts for RX complete and
  data register empty USART3.CTRLA = USART_RXCIE_bm | USART_DREIE_bm;
  // Set frame format: 8N1
  USART3.CTRLC = USART_CHSIZE_8BIT_gc;
  // For USART3 (already configured on Curiosity Nano)
  PORTB.DIRSET = PIN0_bm; // PB0 as output  (TX)
  PORTB.DIRCLR = PIN1_bm; // PB1 as input (RX)
  // Enable transmitter and receiver
  USART3.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

// RX Complete Interrupt
ISR(USART3_RXC_vect) {
  char received_data = USART3.RXDATAL;
  buffer_put(&rx_buffer, received_data);
}

// Data Register Empty Interrupt (TX)
ISR(USART3_DRE_vect) {
  char data_to_send;
  if (buffer_get(&tx_buffer, &data_to_send)) {
    USART3.TXDATAL = data_to_send;
  } else {
    // Disable DRE interrupt when buffer is
    empty USART3.CTRLA &= ~USART_DREIE_bm;
  }
}

void USART3_SendChar(char data) {
  buffer_put(&tx_buffer, data);
  // Enable DRE interrupt to start
  transmission USART3.CTRLA |= USART_DREIE_bm;
}
void USART3_SendString(const char *str) {
  while (*str) {
    USART3_SendChar(*str);
    str++;
  }
}
bool USART3_ReceiveChar(char *data) {
  return buffer_get(&rx_buffer, data);

  void usart_transmit_data(void *ptr, char c) {
    USART_t *usart = (USART_t *)ptr;
    while (!(usart->STATUS & USART_DREIF_bm))
      ;
    usart->TXDATAL = c;
  }

  void usart_transmit_string(void *ptr, const char *str) {
    USART_t *usart = (USART_t *)ptr;
    while (*str) {
      while (!(usart->STATUS & USART_DREIF_bm))
        ;
      usart->TXDATAL = *str;
      str++;
    }
  }

  void usart_wait_until_transmit_ready(void *ptr) {
    USART_t *usart = (USART_t *)ptr;
    while (!(usart->STATUS & USART_DREIF_bm))
      ;
  }

  int usart_receive_data(void *ptr) {
    USART_t *usart = (USART_t *)ptr;
    while (!(usart->STATUS & USART_RXCIF_bm))
      ;
    return usart->RXDATAL;
  }

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


  int main(void) {
    PORTB.DIRSET = PIN3_bm; // PB3 as Output
    USART3_Init(BAUD_RATE);
    sei(); // Enable global interrupts
    USART3_SendString("Interrupt-driven USART Demo\r\n");
    USART3_SendString("Commands: LED_ON, LED_OFF, STATUS\r\n");
    char command_buffer[32];
    uint8_t cmd_index = 0;
    while (1) {
      char received_char;
      if (USART3_ReceiveChar(&received_char)) {
        if (received_char == '\r' || received_char == '\n') {
          command_buffer[cmd_index] = '\0';
          // Process commands
          if (strcmp(command_buffer, "LED_ON") == 0) {
            PORTB.OUTCLR = PIN3_bm; // Turn on LED
            USART3_SendString("LED turned ON\r\n");
          } else if (strcmp(command_buffer, "LED_OFF") == 0) {
            PORTB.OUTSET = PIN3_bm; // Turn off LED
            USART3_SendString("LED turned OFF\r\n");
          } else if (strcmp(command_buffer, "STATUS") == 0) {
            USART3_SendString("System Status: OK\r\n");
          } else {
            USART3_SendString("Unknown command\r\n");
          }

          cmd_index = 0;
          USART3_SendString("> ");
        }

        else if (cmd_index < sizeof(command_buffer) - 1) {
          command_buffer[cmd_index++] = received_char;
          USART3_SendChar(received_char); // Echo
          character
        }
      }
    }
  }
  */

  void init_led() {
    PORTD.DIRSET = 0xFF; // Connect 8 LEDs to PD 0 ~ 7 and set up as output.
    PORTD.OUTCLR = 0xFF; // all off
    PORTC.DIRSET = PIN6_bm | PIN7_bm; // PC6 & PC7 as output
    PORTC.OUTCLR = PIN6_bm | PIN7_bm; // all off
  }

  static inline void init_cpu(void) {
    CPU_CCP = CCP_IOREG_gc;                     // unlock protected IO regs
    CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_16M_gc; // internal HF osc 16 MHz
  }
  /*
  ISR(USART3_RXC_vect)
  {
      USART3_ReceiveISR();
  }
  #define USART3_RX_BUFFER_SIZE 64
  #define USART3_RX_BUFFER_MASK (USART3_RX_BUFFER_SIZE - 1)

  void USART3_ReceiveISR(void)
  {
      uint8_t regValue;
      uint8_t tempRxHead;

      usart3RxStatusBuffer[usart3RxHead].status = 0;


      regValue = USART3.RXDATAL;

      tempRxHead = (usart3RxHead + 1) & USART3_RX_BUFFER_MASK;// Buffer size of
  RX should be in the 2^n if (tempRxHead == usart3RxTail) {
                  // ERROR! Receive buffer overflow
          }
      else
      {
          // Store received data in buffer
                  usart3RxBuffer[usart3RxHead] = regValue;
                  usart3RxHead = tempRxHead;

                  usart3RxCount++;
          }
      if (USART3_RxCompleteInterruptHandler != NULL)
      {
          (*USART3_RxCompleteInterruptHandler)();
      }

  }
      */

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

  static inline uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi) {
    if (v < lo) {
      return lo;
    } else if (v > hi) {
      return hi;
    } else {
      return v;
    }
  }

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
    init_cpu();
    init_led();
    /* Initialize UART stdio on USART3 @ 9600 8N1 */
    uart_init(3, 9600, NULL);
    // USART3.CTRLA |= USART_DREIE_bm; // enable UART Tx interrupt
    // USART3.CTRLA |= USART_RXCIE_bm;
    // USART3.CTRLA |= USART_TXCIE_bm;
    // sei();
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
      // Use interrupts to do this every 5 seconds

      /* Every ~5 seconds: prompt user and handle potential change */
      if (prompt_ms >= 5000U) {
        prompt_ms = 0U;
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
