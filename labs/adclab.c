
#define __AVR_AVR128DB48__
#define F_CPU 16000000UL
#include "adclab.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

/*
void init_adc_clock() {
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_ENABLE_bm;
  CPU_CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_EXTCLK_gc;
}

const uint16_t sine_table[64] = {
    512, 562, 612, 660, 707, 752, 794, 833, 868, 900, 927, 950, 968,
    982, 991, 995, 995, 991, 982, 968, 950, 927, 900, 868, 833, 794,
    752, 707, 660, 612, 562, 512, 461, 411, 363, 316, 271, 229, 190,
    155, 123, 95,  72,  53,  39,  30,  25,  24,  28,  37,  50,  68,
    90,  116, 146, 180, 218, 259, 303, 350, 399, 449, 500, 551};

uint16_t sine_table_scaled[64] = {
    512, 562, 612, 660, 707, 752, 794, 833, 868, 900, 927, 950, 968,
    982, 991, 995, 995, 991, 982, 968, 950, 927, 900, 868, 833, 794,
    752, 707, 660, 612, 562, 512, 461, 411, 363, 316, 271, 229, 190,
    155, 123, 95,  72,  53,  39,  30,  25,  24,  28,  37,  50,  68,
    90,  116, 146, 180, 218, 259, 303, 350, 399, 449, 500, 551};
/*
// implemented in dac.h
void DAC_init(void) {
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;
  VREF.DAC0REF = VREF_REFSEL_VDD_gc;
}
*/
/*
volatile uint32_t freq = 500;
volatile int index = 0;

volatile int amplitude_percent = 100;
volatile float amplitude_scaler = .50;

volatile uint32_t sample_counter = 0;
volatile uint32_t samples_needed_for_10s = 0;

volatile int mode = 0;
volatile int dac_update = 0;
volatile uint32_t temp_freq = 0;
volatile int temp_amp_percent = 0;

// timer with adjustable per for different freq for both DAC updates and status
// printing
void init_adc_tca0() {
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
  uint32_t per_value = F_CPU / (64UL * freq) - 1;
  TCA0.SINGLE.PER = per_value;
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;

  samples_needed_for_10s =
      10UL * freq * 64; // Used to print status every ten seconds adjusting from
                        // different freq this is trash.
}

void frequency_adc_update(uint32_t new_freq) {
  freq = new_freq;
  uint32_t per_value = F_CPU / (64UL * freq) - 1;
  TCA0.SINGLE.PER = per_value;

  samples_needed_for_10s =
      10UL * freq * 64; // Used to print status every ten seconds adjusting from
                        // different freq
}

void amplitude_adc_update(int new_amplitude_percent) {
  amplitude_percent = new_amplitude_percent;
  amplitude_scaler = (float)(amplitude_percent) / 100;

  for (int i = 0; i < 64; i++) {
    int raw = sine_table[i];
    sine_table_scaled[i] = (uint16_t)(raw * amplitude_scaler);
  }
}
*/
/*
  * Implemented in ui.c
  * Requires @params mode, ampltude_percent, temp_amp_percent, temp_freq
void status_printing() {
  printf("\n\nCurrent Freq %lu Hz \nAmplitude Percent: %d \n", freq,
         amplitude_percent);
  switch (mode) {
  case 0:
    printf("\nDo you want to change frequency or amplitude (F/A)? ");
    break;
  case 1:
    printf("\nNew Frequency Value %lu press Enter to confirm ", temp_freq);
    break;
  case 2:
    printf("\nEnter Amplitude Percent (10-100): %d Press enter to confirm ",
           temp_amp_percent);
    break;
  }
}
*/
/*
// rewrite
void question_handler() {
  switch (mode) {
  case 0: // Wait for initial question answer
    if (keypress_val == 'f' || keypress_val == 'F') {
      mode = 1;
      printf("\nFrequency (10-1k Hz): ");
    } else if (keypress_val == 'a' || keypress_val == 'A') {
      mode = 2;
      printf("\nEnter Amplitude Percent (10-100): ");
    } else {
      printf("\nEnter valid value\nDo you want to change frequency or "
             "amplitude (F/A)? ");
    }
    break;

  case 1: // Update frequency waits for enter to leave
    if (keypress_int >= 0 && keypress_int <= 9) { // Valid Input
      temp_freq = (temp_freq * 10) + keypress_int;
      printf("\nNew value %lu Press enter to confirm ", temp_freq);

    } else if (keypress_val == '\r') { // New Value Confirmed
      // Check value is in range
      if (temp_freq >= 10 && temp_freq <= 1000) {

        frequency_update(temp_freq); // UPDATE FREQUENCY FUNCTION

        printf("\nFrequency set to %lu \nDo you want to change frequency or "
               "amplitude (F/A)? ",
               freq);

      } else {
        printf("\n%lu out of range start over \nDo you want to change "
               "frequency or amplitude (F/A)? ",
               temp_freq);
      }

      temp_freq = 0;
      mode = 0;

    } else { // Invalid Input
      temp_freq = 0;
      mode = 0;
      printf("Invalid Value \nDo you want to change frequency or amplitude "
             "(F/A)? ");
    }
    break;

  case 2: // Amplitude Level shifter press enter to confirm
    if (keypress_int >= 0 && keypress_int <= 9) { // Valid Value
      temp_amp_percent = (temp_amp_percent * 10) + keypress_int;
      printf("\nNew Amplitude Percent %d Press enter to confirm ",
             temp_amp_percent);

    } else if (keypress_val == '\r') { // Confirm Value Update

      if (temp_amp_percent >= 10 &&
          temp_amp_percent <= 100) { // Check if value in range

        // AMPLITUDE CHANGE FUNCTION
        amplitude_update(temp_amp_percent);
        printf("\nAmplitude percent set to %d \nDo you want to change "
               "frequency or amplitude (F/A)? ",
               amplitude_percent);

      } else {
        printf("\n%d out of range start over \nDo you want to change frequency "
               "or amplitude (F/A)? ",
               temp_amp_percent);
      }
      temp_amp_percent = 0;
      mode = 0;
    } else { // Invalid Value
      temp_amp_percent = 0;
      mode = 0;
      printf("Invalid Value \nDo you want to change frequency or amplitude "
             "(F/A)? ");
    }
    break;
  }
}
*/
/*
int adclab(void) {
  init_clock();
  uart_init(3, BAUD_RATE, F_CLK_PER, NULL);
  InitTimerTCA0();
  USART3.CTRLA |= USART_RXCIE_bm;
  DAC_init();

  sei();

  printf("\nDo you want to change frequency or amplitude (F/A)? ");

  while (1) {
    if (dac_update) {

      int sample = sine_table_scaled[index];
      DAC0_setVal = sample;
      index++;
      if (index >= 64) {
        index = 0;
      }

      dac_update = 0;
    }

    if (keypress_flag == 1) {

      question_handler();

      keypress_flag = 0;
    }

    if (sample_counter >= samples_needed_for_10s) {
      status_printing();
      sample_counter = 0;
    }
  }
}
*/
