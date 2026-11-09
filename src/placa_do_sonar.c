#include <stdint.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "hz_clock.h"

#define TRIG GPIO10
#define ECHO GPIO13

volatile uint64_t ticks = 0;
volatile uint64_t time = 0;



void sys_tick_handler(void);
void sys_tick_handler(void) { ticks++;
                              time++; }
uint16_t estado;
int main(void) {
  setup_clock1000hz();

  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART1);

 gpio_set_mode(GPIOB,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,TRIG);
  gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, ECHO);

 usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_baudrate(USART1, 9600);
  usart_enable(USART1);
  int start =0 ;
  estado = gpio_get(GPIOB, ECHO);
    while(1){


   if(  ticks - start > 0    && ticks-start < 20){

    gpio_set(GPIOB, TRIG);
   
   }
   if(ticks - start > 20 && ticks-start < 60000){
    gpio_clear(GPIOB, TRIG);
   }
   if(ticks - start > 60000){
    start = ticks;
   }
   if(gpio_get(GPIOB, ECHO) != 0){
      time = 0;
    }else{
      if(time > 0){
        uint16_t distancia = (time * 0.01715);
        usart_send_blocking(USART1, distancia);
      }
    }




}}