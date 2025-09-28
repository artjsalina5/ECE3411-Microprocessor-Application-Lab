# ECE 3411 USART: Universal Synchronous & Asynchronous Receiver & Transmitter

## Objectives

- Understand USART
- Configure The uC
- Implement *interrupt-driven* serial communication
- debug serial communication issues
- Create practical applications using USART
- Apply flow control and error handling techniques

## USART

- This device implements serial communications
- Asynchronous mode only needs Tx, Rx, Gnd
- Originally designed for MODEMS

## Two Levels

- RS232 is the 5V level
  - RS232 required more capacitors and handling
- RS485 is the 3.3V level version

| feature | RS232 | RS 485 |
| --- | -- | -- |
| Interface IC | MAX232 | MAX485 or SN75176 |
| AVR Pins | USART TX/RC | USART TX/RX + GPIO for DE?RE control |
| Voltage level | TTL | TTL Converted to differential +/5V |
| Mode | Point-to-Point full duplex mode | Multi-drop bus half duplex mode |

## FTDI Interface

- Previously, the FTDI interface was dedicated microprocessor that
allowed the conversion from UART to USB.

## Frame Format

- To transmit a byte (or one character) we need at least one
start bit (receiving clock starts when
falling edge is received), 8 data bits, and one stop bit.
- Total 10 bits

## USART

- Baud Rate: Bits per second transmission rate
- Each byte takes 10 bits (1 start bit, 8 data bits, 1 stop bit)
- 9600 baud = 960 bytes/s (approximately 1ms per byte)
- This is slow: Therefore, in SW start transmitting a character, then do something else!
- Baud rate can go up to 1Mbit per second
- The cable limits the maximum possible Baud rate.

### AVRDx USART

- USART = Universal Synchronous and Asynchronous serial Receiver and Transmitter
- Clock generator, Transmitter, Receiver
- AVR128DB48 has five USARTs

| Signal | type | Description |
| -- | -- | --- |
| XCK | output/input | Clock for synchronous operation |
| XDIR | output | Transmit enable for RS-485. |
| TxD | output/input | transmit line and reciving in one line mode |
| RxD | input | receiving line |

### UBRR0H and UBRR0L

- Baud rate is translated relative to the system oscillator clock frequency $f_{CLK_PER}$ to the BAUD register

## Curiosity Nano Board Hardware

- USART3 utilizes PB0 (Tx) and PB1 (Rx)

- There is a high side and a low side register to allow 9 bits

```c
#define F_CPU 4000000UL

#define BAUDE_RATE 9600

void USART3_Init(uint32_t baud) {
  uint16_t baud_setting = (F_CPU * 64) / (16 * baud);

  USART3.BAUD = baud_setting;
}
```

### CTRLA

- Contains
  - RXCIE - Receive Character Complete interrupt enable
  - TXCIE Enables interrupt for both members in TX queue being empty
  - DREIE - Data Register Empty, Enables interrupt if the first
          of the output pipeline is empty. Ready to Transmit
  - RXSIE - Receive start frame interrupt enable
  - LBME - Loop back mode enable
  - ABEIE - Auto Baud error Interrupt Enable
  - RS485 - Enable RS-485 Mode

### CTRLB

- Contains
  - RXEN - RX Enable
  - TXEN - Tx enable must still enable the corresponding pin as an output
  - SFDEN - Enable start of frame detection
  - ODME - Open Drain Mode Enable
  - RXMODE - Receiver Mode Selection
  - MPCM - Multiple Processor Address mode

```c
// Enable transmitter and receiver
USART3.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
```

### CTRLC

- CMODE - Communication mode
- PMODE - Parity Mode
- SBMODE - Stop Bit Mode
- CHSIZE - Character Size

```c
USART3.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_CMODE_ASYNCHRONOUS_gc | 
    USART_SBMODE_1BIT_gc;
```

### STATUS

- RXCIF - Receive character complete flag
- TXCIF - Transmit queue empty
- DREIF - First of the output pipeline is empty. Ready to transmit
- RXSIF - Receive start frame interrupt enable
- ISFIF - Inconsistent synchronization field
- BDF - Break Detected Flag
- WFB - Enable Wait-for-Break

```c
void USART3_SendChar(char data){
  while (!(USART3.STATUS & USART_DREIF_bm));

  USART3.TXDATAL = data;
}
```

```c
void USART3_ReceiveChar(char data){
  while (!(USART3.STATUS & USART_RXCIF_bm));

  // Return received data
  return USART3.RXDATAL;
}
```
