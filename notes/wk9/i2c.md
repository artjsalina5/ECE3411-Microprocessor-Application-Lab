# I2C

## I2C Inter Integrated Circuit

- AKA Two Wire Serial Interface, I square C, I two C.

- Allows up to 128 different devices to be connected using only two
  bi-directional bus lines, one for clock, and one for data. SDA.

## I2C Terminology

- Host/Master initiates and terminates a transmission

- Host also generates the SCL clock

- Client/Slave interface receives

### Open Drain Mode

- Each line SDA and SDL is connected VCC or VDD via a pull up.

### Pull up resistor Mode

- I2c data line can change between high and low states only whiile the clock
  line is low.

- Data is to be read only while the clock line is high.

- A start signal is a falling edge while the clock line is held high.

- A stop signal is a rising edge while the clock line is held high.

## KEy Registers

- TWIx.MCTRLA
- TWI.MSTATUS
- TWIx.MBAUD
- TWIx.MADDR
- TWIx.MDATA

### Example Init

```c
void TWI_HOST_INITIALIZE (void) {
  TWI0.MBAUD = 95;
  TWI0.MCTRLA |= TWI_ENABLE_bm;
  TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}
```
