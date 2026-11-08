#include "hz_clock.h"
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

void setup_clock1000hz(void) {
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  systick_set_frequency(1000000, rcc_ahb_frequency);
  systick_counter_enable();
  systick_interrupt_enable();
}
