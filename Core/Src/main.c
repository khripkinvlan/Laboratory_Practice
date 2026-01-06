#include <stdint.h>

#define GPIOA_BSRR (*(uint32_t*)(0x40010800 + 0x10))

#define LED_R_SET *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<24);*(uint32_t*)(0x40010800 + 0x10) |= (1<<8)
#define LED_R_RESET   *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<8);*(uint32_t*)(0x40010800 + 0x10) |= (1<<24)

#define LED_Y_SET *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<25);*(uint32_t*)(0x40010800 + 0x10) |= (1<<9)
#define LED_Y_RESET   *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<9);*(uint32_t*)(0x40010800 + 0x10) |= (1<<25)

#define LED_W_SET *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<26);*(uint32_t*)(0x40010800 + 0x10) |= (1<<10)
#define LED_W_RESET   *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<10);*(uint32_t*)(0x40010800 + 0x10) |= (1<<26)

#define BUTT1_State_Check !((*(volatile uint32_t*)(0x40010800UL + 0x08UL)) & (1 << 4)) //GPIOA_IDR
#define BUTT2_State_Check !((*(volatile uint32_t*)(0x40010800UL + 0x08UL)) & (1 << 5)) //GPIOA_IDR

void Delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 1108; i++)
    {
        __asm__("nop");
    }
}

uint32_t BUTT1_State = 0;
uint32_t BUTT1_Prev_State = 0;
uint32_t BUTT1_is_Clicked = 0;

uint32_t BUTT2_State = 0;
uint32_t BUTT2_Prev_State = 0;
uint32_t BUTT2_is_Clicked = 0;
uint32_t mode = 0;


uint32_t LED_QUANTITY = 0;

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
    *(uint32_t*)(0x40010800UL + 0x00UL) &= ~(1<<16); //   Нулевой бит MODE4
    *(uint32_t*)(0x40010800UL + 0x00UL) &= ~(1<<17); //   Первый бит MODE4 ('00' - Input mode (reset state))
    *(uint32_t*)(0x40010800UL + 0x00UL) &= ~(1<<18); //   Нулевой бит CNF4
    *(uint32_t*)(0x40010800UL + 0x00UL) |= (1<<19); //   Первый бит CNF4 ('10' - Input with pull-up / pull-down при MODE - input)
    *(uint32_t*)(0x40010800UL + 0x0CUL) &= ~(1<<4); //    Четвертый бит ODR4 (Включение pull-down)
    // Конфигурация пина PA5 - BUTT_2
    *(uint32_t*)(0x40010800UL + 0x00UL) &= ~(1<<20); //   Нулевой бит MODE5
    *(uint32_t*)(0x40010800UL + 0x00UL) &= ~(1<<21); //   Первый бит MODE5 ('00' - Input mode (reset state))
    *(uint32_t*)(0x40010800UL + 0x00UL) &= ~(1<<22); //   Нулевой бит CNF5
    *(uint32_t*)(0x40010800UL + 0x00UL) |= (1<<23); //   Первый бит CNF5 ('10' - Input with pull-up / pull-down при MODE - input)
    *(uint32_t*)(0x40010800UL + 0x0CUL) &= ~(1<<5);  //    Четвертый бит ODR5 (Включение pull-down)

    while (1){
        //LED_Y_RESET;
        //LED_R_RESET;
        //LED_W_RESET;
        // Delay(100);
        //LED_W_SET;
        //LED_Y_SET;



        //Обработчик BUTTON1 - пин PA4
        BUTT1_State = BUTT1_State_Check; //Считывание состояния пина PA4 в регистре GPIO_IDR
        if (BUTT1_State)
        {
            if (BUTT1_State != BUTT1_Prev_State)
            {
                BUTT1_is_Clicked = 1;
            }
            else
            {
                BUTT1_is_Clicked = 0;
            }
                BUTT1_Prev_State = 1;

        }
        else
        {
            BUTT1_is_Clicked = 0;
                BUTT1_Prev_State = 0;
            if (BUTT1_State != BUTT1_Prev_State)
            {
                BUTT1_is_Clicked = 0;
            }
            else
            {
                BUTT1_is_Clicked = 0;
            }
        }




        //Обработчик BUTTON2 - PA5
        BUTT2_State = BUTT2_State_Check; //Считывание состояния пина PA4 в регистре GPIO_IDR
        if (BUTT2_State)
        {
            if (BUTT2_State != BUTT2_Prev_State)
            {
                BUTT2_is_Clicked = 1;
            }
            else
            {
                BUTT2_is_Clicked = 0;
            }
                BUTT2_Prev_State = 1;

        }
        else
        {
            BUTT2_is_Clicked = 0;
                BUTT2_Prev_State = 0;
            if (BUTT2_State != BUTT2_Prev_State)
            {
                BUTT2_is_Clicked = 0;
            }
            else
            {
                BUTT2_is_Clicked = 0;
            }
        }




        if (BUTT1_is_Clicked)
        {
            if (!mode)
            {
                LED_QUANTITY += 1 ;
                LED_QUANTITY >3? LED_QUANTITY = 3:0;
                Delay(100);
            }
            else
            {
                LED_QUANTITY -= 1 ;
                LED_QUANTITY >3? LED_QUANTITY = 0:0;
                Delay(100);
            }

        }

        if (BUTT2_is_Clicked)
        {
            // LED_QUANTITY = 0;
            mode = !mode;
        }

        switch (LED_QUANTITY) {
            case 0:
                // Тушим все светодиоды
                LED_R_RESET;
                LED_Y_RESET;
                LED_W_RESET;
                break;

            case 1:
                // Зажигаем красный, остальные тушим
                LED_R_SET;
                LED_Y_RESET;
                LED_W_RESET;
                break;
            
            case 2:
                // Зажигаем красный и желтый, белый тушим
                LED_R_SET;
                LED_Y_SET;
                LED_W_RESET;
                break;
            case 3:
                // Зажигаем все светодиоды
                LED_R_SET;
                LED_Y_SET;
                LED_W_SET;
                break;
        }
    //     default:
    //      // Код, если ни одно из значений не совпало
    //      break;
    //}
        // Delay(1000);

        // Delay(1000);
    }
}