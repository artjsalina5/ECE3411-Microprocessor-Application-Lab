# PWM

- Characterized by a fixed frequency and a varying duty cycle

## Modulation

- If each duty cycle d, the width of each pulse $T_d$, where $T$ is the length of each period. $T$ is the inverse of the switching frequency.

- Resolution of each pulse is determined by the f_clk.
- If you want 8 bit resolution, that means you need 256 possible pulse widths. Thus, the smallest $T$
 is $\frac{256}{f_{clk}}$

### AVR128DB48

- Cennect the PWM signal to one of the output pins
- How do you alter the square wave in code?
  = With a timer, TCA0
- 10KHz at 8-bit means the pulse width resolution is ~400ns

## PWM Modes

- PWM is gernerated with timers
- The PER register controls the PWM period/frequency.
- CMPn controls the duty cycle.

-- PWM can have higher frequencies with Single Slope. Power conversion applications
-- Dual Slope is phase accurate. Motor drive applications
