#include <stdint.h>
void Delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 1108; i++)
    {
        __asm__("nop");
    }
}

int main(void){
    //Запуск тактирования GPIOA
    *(uint32_t*)(0x40021000UL + 0x018UL) |= 0x04; // Запуск тактирования IOPAEN в регистре RCC_APB2ENR


    //Конфигурация пинов светодиодов
    // Конфигурация пина PA8 - LED_R
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<0); //   Нулевой бит MODE8
    *(uint32_t*)(0x40010800UL + 0x04UL) |= (1<<1); //   Первый бит MODE8 ('10' - output low speed up to 2MHz)
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<2); //   Нулевой бит CNF8
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<3); //   Первый бит CNF8 ('00' - General purpose output push-pull при MODE - output)
    *(uint32_t*)(0x40010800UL + 0x0CUL) &= ~(1<<8);  //    Четвертый бит ODR5 (Включение pull-up)

    // Конфигурация пина PA9 - LED_Y
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<4); //   Нулевой бит MODE9
    *(uint32_t*)(0x40010800UL + 0x04UL) |= (1<<5); //   Первый бит MODE9 ('10' - output low speed up to 2MHz)
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<6); //   Нулевой бит CNF9
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<7); //   Первый бит CNF9 ('00'- General purpose output push-pull при MODE - output)
    *(uint32_t*)(0x40010800UL + 0x0CUL) &= ~(1<<9);  //    Четвертый бит ODR5 (Включение pull-up)

    // Конфигурация пина PA10 - LED_W
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<8); //   Нулевой бит MODE10
    *(uint32_t*)(0x40010800UL + 0x04UL) |= (1<<9); //   Первый бит MODE10 ('10' - output low speed up to 2MHz)
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<10); //   Нулевой бит CNF10
    *(uint32_t*)(0x40010800UL+ 0x04UL) &= ~(1<<11); //   Первый бит CNF10 ('00' - General purpose output push-pull при MODE - output)
    *(uint32_t*)(0x40010800UL + 0x0CUL) &= ~(1<<10);  //    Четвертый бит ODR5 (Включение pull-up)

    //Конфигурация пинов кнопок
    // Конфигурация пина PA4 - BUTT_1
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<16); //   Нулевой бит MODE4
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<17); //   Первый бит MODE4 ('00' - Input mode (reset state))
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<18); //   Нулевой бит CNF4
    *(uint32_t*)(0x40010800UL + 0x04UL) |= (1<<19); //   Первый бит CNF4 ('10' - Input with pull-up / pull-down при MODE - input)
    *(uint32_t*)(0x40010800UL + 0x0CUL) |= (1<<4); //    Четвертый бит ODR4 (Включение pull-up)
    // Конфигурация пина PA5 - BUTT_2
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<20); //   Нулевой бит MODE4
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<21); //   Первый бит MODE4 ('00' - Input mode (reset state))
    *(uint32_t*)(0x40010800UL + 0x04UL) &= ~(1<<22); //   Нулевой бит CNF4
    *(uint32_t*)(0x40010800UL + 0x04UL) |= (1<<23); //   Первый бит CNF4 ('10' - Input with pull-up / pull-down при MODE - input)
    *(uint32_t*)(0x40010800UL + 0x0CUL) |= (1<<5);  //    Четвертый бит ODR5 (Включение pull-up)

    while (1){
        *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<24); // Сбрасываем RESET бит
        *(uint32_t*)(0x40010800 + 0x10) |= (1<<8);  // Зажигаем красный свтодиод, устанавливая 1 в бит BS8 регистра GPIO_BSRR
        Delay(1000);
        *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<8);  // Сбрасываем SET бит
        *(uint32_t*)(0x40010800 + 0x10) |= (1<<24); // Тушим красный свтодиод, устанавливая 1 в бит BR8 регистра GPIO_BSRR
        Delay(1000);
    }
}