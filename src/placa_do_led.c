#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <stdio.h>
#include <unistd.h>
#include "led_rgb.h"

volatile uint64_t ticks = 0;

void sys_tick_handler(void)
{
  ticks++;
}

int main(void)
{
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART2);
gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO3);
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO10);
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO11);
  systick_set_frequency(1000, rcc_ahb_frequency);
  systick_counter_enable();
  systick_interrupt_enable();
  setup_leds();
  setup_pinos();

 

    usart_set_baudrate(USART2, 9600);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_enable(USART2);

  gpio_clear(GPIOA, GPIO9);
  gpio_set(GPIOA, GPIO10);
  gpio_set(GPIOA, GPIO11);
  unsigned int start = ticks;

  while (1)
  {
    now = ticks;
        if ((ticks - start) > 1000 && (ticks - start) < 2000)
        {
            float buffer = usart_recv_blocking(USART2);
            if(buffer <=2)
            {
                ascender_vermelho();
            }

            if(buffer>2 && buffer <=4 )
            {
                ascender_roxo();
            }

            if(buffer>4 && buffer <=6)
            {
                ascender_azul();
            }

            if(buffer>6 && buffer<=8)
            {
                ascender_ciano();
            }

            if(buffer>8 && buffer<=10)
            {
                ascender_verde();
            }
            if(buffer>10 && buffer <=12)
            {
                ascender_amarelo();
            }

            if(buffer>12 && buffer <=14)
            {
                ascender_branco();
            }
        
            if(buffer>14)
            {
                apagar_led();
            }
            
            start = ticks;
        }
  }
}
