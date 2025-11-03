// OPAMP Lab UI integration for AOS
#define F_CPU 16000000UL
#define __AVR_AVR128DB48__

#include "ui_opamp.h"
#include "aos_timer.h"
#include "opamp.h"
#include "uart.h"
#include "ui.h"
#include <avr/io.h>
#include <stdlib.h>

// Internal state
static volatile bool opamp_active = false;
static volatile uint32_t opamp_countdown = 0; // ms

// 2 seconds at ~1ms ticks
#define OPAMP_READ_INTERVAL_MS 2000UL

// ADC configuration for OPAMP2 output (connected to ADC0 AIN10 on AVR128DB48)
static void opamp_adc_init(void) {
  // Use VDD (3.3V) reference, no oversampling, prescaler DIV8
  VREF.ADC0REF = VREF_REFSEL_VDD_gc; // Vref = VDD
  ADC0.CTRLC = ADC_PRESC_DIV8_gc;    // clk/8
  ADC0.CTRLD = ADC_INITDLY_DLY16_gc; // small init delay
  ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc; // enable, 10-bit

  // Route positive input to OP2 OUT = AIN10
  ADC0.MUXPOS = ADC_MUXPOS_AIN10_gc;

  // Start first conversion and wait complete
  ADC0.COMMAND = ADC_STCONV_bm;
  while (ADC0.COMMAND & ADC_STCONV_bm) {
  }
}

// TCA1 1ms ISR callback for countdown
static void opamp_tca1_tick(void) {
  if (opamp_countdown > 0) {
    opamp_countdown--;
  }
}

void opamp_lab_init(void) {
  opamp_active = true;

  // Configure GPIO pads used by OPAMPs
  GPIO_opamp_init();

  // Initialize OPAMP peripheral (3-op-amp instrumentation amplifier)
  OPAMP_init();

  // Configure ADC to read OP2 output
  opamp_adc_init();

  // Start 1ms tick on TCA1 for periodic sampling
  opamp_countdown = OPAMP_READ_INTERVAL_MS;
  aos_tca1_register(opamp_tca1_tick);
  aos_tca1_start_1ms();
}

void opamp_lab_process(void) {
  // Exit handling
  char ch;
  if (uart_receive_char(&ch)) {
    uart_send_char(ch);
    if (ch == 'e' || ch == 'E') {
      opamp_lab_exit();
      return;
    }
  }

  if (opamp_countdown == 0) {
    // Read last ADC result and start a new one
    uint16_t adc_raw = ADC0.RES; // 10-bit (0..1023)
    ADC0.COMMAND = ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm) {
    }

    // Convert to voltage (assuming Vref = 3.3V, 10-bit ADC)
    const float vref = 3.3f;
    const float adc_max = 1023.0f;
    float v_out = (float)adc_raw * (vref / adc_max);

    // Format voltage and print only Vop2 (no compile-time defines needed)
    char buf_v[16];
    dtostrf(v_out, 4, 3, buf_v);
    if (adc_raw >= 1015) {
      aos_printf("Vop2 = %s V [SAT]\r\n", buf_v);
    } else if (adc_raw <= 2) {
      aos_printf("Vop2 = %s V [LOW]\r\n", buf_v);
    } else {
      aos_printf("Vop2 = %s V\r\n", buf_v);
    }

    // Reset interval
    opamp_countdown = OPAMP_READ_INTERVAL_MS;
  }
}

void opamp_lab_show_welcome(void) {
  aos_send("\r\n");
  aos_send("+-----------------------------------------------------------+\r\n");
  aos_send("|                        OPAMP LAB                          |\r\n");
  aos_send("+-----------------------------------------------------------+\r\n\r\n");
  aos_send("3-op-amp instrumentation amplifier (external resistor network):\r\n");
  aos_send("  - OP0+ on PD1 (buffer/gain stage using external resistors)\r\n");
  aos_send("  - OP1+ on PD7 (buffer/gain stage using external resistors)\r\n");
  aos_send("  - OP2 configured as differential stage; OP2 OUT -> ADC0 (AIN10)\r\n");
  aos_send("\r\nMeasurements:\r\n");
  aos_send("  - Prints every 2 seconds via USART\r\n");
  aos_send("  - Default output: Vop2 only. To compute current: I = Vout / (G * Rshunt)\r\n");
  aos_send("    Define INA_GAIN and RSHUNT_OHMS at build time if desired.\r\n\r\n");
  aos_send("(Type EXIT to return to AOS)\r\n\r\n");
}

bool opamp_lab_is_active(void) { return opamp_active; }

void opamp_lab_exit(void) {
  // Stop 1ms tick and release timer
  aos_tca1_disable();
  aos_tca1_unregister();

  // Disable ADC to free resource for other labs
  ADC0.CTRLA &= ~ADC_ENABLE_bm;

  // Disable OPAMPs if desired (keep powered down)
  OPAMP_DisableSystem();

  opamp_active = false;
  aos_send("\r\nExiting OPAMPLab, returning to AOS...\r\n\r\n");
}
