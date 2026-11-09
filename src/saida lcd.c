#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <stdio.h>

const int D4 = 2;
const int D5 = 3;
const int D6 = 4;
const int D7 = 5;

const int RS = 7;
const int EN = 6;

// Função usleep calibrada para 72MHz (18 iterações x ~4 ciclos = 72 ciclos = 1 us)
void usleep(uint32_t us)
{
    for (volatile uint32_t i = 0; i < us * 18; i++) {
        __asm__("nop");
    }
}

void send_nibble(void)
{
    gpio_set(GPIOA, 1 << EN);
    usleep(1); // Mantido o valor original
    gpio_clear(GPIOA, 1 << EN);
    usleep(100); // Mantido o valor original
}

void nibble(int bits, int rs)
{
    if (rs)
        gpio_set(GPIOA, 1 << RS);
    else
        gpio_clear(GPIOA, 1 << RS);

    if (bits & 0x01) gpio_set(GPIOA, 1 << D4); else gpio_clear(GPIOA, 1 << D4);
    if (bits & 0x02) gpio_set(GPIOA, 1 << D5); else gpio_clear(GPIOA, 1 << D5);
    if (bits & 0x04) gpio_set(GPIOA, 1 << D6); else gpio_clear(GPIOA, 1 << D6);
    if (bits & 0x08) gpio_set(GPIOA, 1 << D7); else gpio_clear(GPIOA, 1 << D7);

    send_nibble();
}

void iniciar_lcd(void)
{
    usleep(15000); // Mantido valor original (agora dura 15ms reais)
    nibble(0x03, 0);
    usleep(5000);  // Mantido valor original (5ms reais)
    nibble(0x03, 0);
    usleep(100);   // Mantido valor original (100us reais)
    nibble(0x03, 0);

    nibble(0x02, 0); // Modo 4 bits

    nibble(0x02, 0); // Function Set: 4-bits, 2 linhas
    nibble(0x08, 0);

    nibble(0x00, 0); // Display ON (0x0C)
    nibble(0x0C, 0);

    nibble(0x00, 0); // Clear Display (0x01)
    nibble(0x01, 0);
    usleep(2000);  // Mantido valor original (2ms reais)

    nibble(0x00, 0); // Entry Mode Set (0x06)
    nibble(0x06, 0);
}

void new_line(void)
{
    nibble(0x0C, 0); // Move para a 2ª linha (0xC0 em nibbles)
    nibble(0x00, 0);
}

void palavra(const char* texto)
{
    for (int i = 0; texto[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)texto[i];
        nibble(c >> 4, 1);    // High nibble
        nibble(c & 0x0F, 1);  // Low nibble
    }
}

#include <math.h>

void numeroFloat(float val, int casasDecimais)
{
    int parteInteira = (int)val;
    float fracao = val - parteInteira;
    if (fracao < 0) fracao = -fracao;

    // Calcula 10^casasDecimais
    int multiplicador = 1;
    for (int i = 0; i < casasDecimais; i++) {
        multiplicador *= 10;
    }

    // +0.5f garante o arredondamento correto ao converter para int
    int parteDecimal = (int)(fracao * multiplicador + 0.5f);

    char buffer[17];
    // O '*' em '%0*d' recebe 'casasDecimais' para ajustar os zeros dinamicamente
    sprintf(buffer, "%d.%0*d", parteInteira, casasDecimais, parteDecimal);
    
    palavra(buffer);
}

void lcd_set_cursor(uint8_t linha, uint8_t coluna)
{
    // Linha 0 (1ª linha): Endereço base 0x80
    // Linha 1 (2ª linha): Endereço base 0xC0
    uint8_t endereco = (linha == 0) ? (0x80 + coluna) : (0xC0 + coluna);

    nibble(endereco >> 4, 0);   // Nibble alto
    nibble(endereco & 0x0F, 0); // Nibble baixo
}

volatile uint64_t ms = 0;

void sys_tick_handler(void)
{
    ms++;
}

int main(void)
{
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    rcc_periph_clock_enable(RCC_GPIOA);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO7);

    systick_set_frequency(1000, rcc_ahb_frequency);
    systick_counter_enable();
    systick_interrupt_enable();

    float temperatura = 25.5;

    iniciar_lcd();
    palavra("Distancia:");
    new_line();

    while (1) {
        // Loop principal
        lcd_set_cursor(1, 0); // Move para a segunda linha
        numeroFloat(temperatura, 2);
        usleep(200000);
    }
}
