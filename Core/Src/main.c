#include <stdint.h>

volatile uint32_t g_milliseconds = 0;
volatile uint32_t g_iteration_counter = 0;

// Калибровка: сколько итераций главного цикла = 1 мс
// Подбирается экспериментально
#define ITERATIONS_PER_MS 35

void millis_init(void) {
    g_milliseconds = 0;
    g_iteration_counter = 0;
}

// Вызывать в КАЖДОЙ итерации while(1)
static inline void millis_update(void) {
    g_iteration_counter++;
    if (g_iteration_counter >= ITERATIONS_PER_MS) {
        g_milliseconds++;
        g_iteration_counter = 0;
    }
}

uint32_t millis(void) {
    return g_milliseconds;
}


#define GPIOA_BSRR (*(uint32_t*)(0x40010800 + 0x10))

#define LED_R_SET *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<24);*(uint32_t*)(0x40010800 + 0x10) |= (1<<8)
#define LED_R_RESET   *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<8);*(uint32_t*)(0x40010800 + 0x10) |= (1<<24)

#define LED_Y_SET *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<25);*(uint32_t*)(0x40010800 + 0x10) |= (1<<9)
#define LED_Y_RESET   *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<9);*(uint32_t*)(0x40010800 + 0x10) |= (1<<25)

#define LED_W_SET *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<26);*(uint32_t*)(0x40010800 + 0x10) |= (1<<10)
#define LED_W_RESET   *(uint32_t*)(0x40010800 + 0x10) &= ~(1<<10);*(uint32_t*)(0x40010800 + 0x10) |= (1<<26)

#define BUTT1_State_Check !((*(volatile uint32_t*)(0x40010800UL + 0x08UL)) & (1 << 4)) //GPIOA_IDR
#define BUTT2_State_Check !((*(volatile uint32_t*)(0x40010800UL + 0x08UL)) & (1 << 5)) //GPIOA_IDR

uint32_t LED_R_State = 0;
uint32_t LED_Y_State = 0;
uint32_t LED_W_State = 0;

uint8_t a1 = 0;
uint8_t a2 = 0;
uint8_t a3 = 0;
uint8_t a4 = 0;

// freq - частота мигания светодиода в Гц
void LED_R_Blink(float freq) {
    static uint32_t last_toggle = 0;
    static uint8_t led_state = 0;
    
    // Защита от деления на ноль
    if (freq == 0) return;
    
    // Период в миллисекундах = 1000 / freq
    // Полупериод (время включения/выключения) = период / 2
    float half_period_ms = 500 / freq;
    a1 = 1;
    // Проверяем, прошло ли нужное время
    if ((millis() - last_toggle) >= half_period_ms) {
        led_state = !led_state;
        a2 = 1;
        if (led_state) {
            LED_R_SET;
            a3 = 1;
        } else {
            LED_R_RESET;
            a4 = 1;
        }

        last_toggle = millis();
    }
}

void LED_Y_Blink(float freq) {
    static uint32_t last_toggle = 0;
    static uint8_t led_state = 0;
    
    // Защита от деления на ноль
    if (freq == 0) return;
    
    // Период в миллисекундах = 1000 / freq
    // Полупериод (время включения/выключения) = период / 2
    float half_period_ms = 500 / freq;
    
    // Проверяем, прошло ли нужное время
    if ((millis() - last_toggle) >= half_period_ms) {
        led_state = !led_state;

        if (led_state) {
            LED_Y_SET;
        } else {
            LED_Y_RESET;
        }
        
        last_toggle = millis();
    }
}

void LED_W_Blink(float freq) {
    static uint32_t last_toggle = 0;
    static uint8_t led_state = 0;
    
    // Защита от деления на ноль
    if (freq == 0) return;
    
    // Период в миллисекундах = 1000 / freq
    // Полупериод (время включения/выключения) = период / 2
    float half_period_ms = 500 / freq;
    
    // Проверяем, прошло ли нужное время
    if ((millis() - last_toggle) >= half_period_ms) {
        led_state = !led_state;

        if (led_state) {
            LED_W_SET;
        } else {
            LED_W_RESET;
        }
        
        last_toggle = millis();
    }
}

void LED_Blink(uint8_t Number, float freq[3]) {
    switch (Number)
    {
    case 0:
        LED_R_Blink(freq[0]);
        break;
    
    case 1:
        LED_Y_Blink(freq[1]);
        break;

    case 2:
        LED_W_Blink(freq[2]);
        break;
    
    default:
        break;
    }
}

void LED_Sparkle(uint8_t Number) {
    switch (Number)
    {
    case 0:
        LED_R_SET;
        Delay(20);
        LED_R_RESET;
        Delay(20);
        LED_R_SET;
        Delay(20);
        LED_R_RESET;
        break;
    
    case 1:
        LED_Y_SET;
        Delay(20);
        LED_Y_RESET;
        Delay(20);
        LED_Y_SET;
        Delay(20);
        LED_Y_RESET;
        break;

    case 2:
        LED_W_SET;
        Delay(20);
        LED_W_RESET;
        Delay(20);
        LED_W_SET;
        Delay(20);
        LED_W_RESET;
        break;
    
    default:
        break;
    }
}

void Delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * ITERATIONS_PER_MS; i++)
    {
        __asm__("nop");
    }
}

uint32_t BUTT1_State = 0;
uint32_t BUTT1_is_Clicked = 0;
uint32_t BUTT1_is_Pressed = 0;


uint32_t BUTT2_State = 0;
uint32_t BUTT2_is_Clicked = 0;
uint32_t BUTT2_is_Pressed = 0;
uint32_t mode = 0;

uint32_t LEDS_isActive[3] = {0, 0, 0};
float LEDS_freq [3] = {0.5, 0.5, 0.5}; 
int32_t LEDS_number = 0;
uint8_t prev_LEDS_number = 0;

uint8_t BUTT1_Handler(void) {
    static uint32_t press_start_time = 0;
    static uint8_t prev_state = 0;

    
    uint8_t current_state = BUTT1_State_Check;
    
    // Кнопка только что нажата
    if (current_state && !prev_state) {
        press_start_time = millis();
        BUTT1_is_Clicked = 0;
        BUTT1_is_Pressed = 0;
        prev_state = 1;
    }
    
    // Кнопка удерживается
    if (current_state && prev_state) {
        // Проверяем, прошла ли 1 секунда
        if ((millis() - press_start_time) >= 1000 && !BUTT1_is_Pressed) {
            BUTT1_is_Pressed = 1;
            return 1;  // Длинное нажатие обнаружено!
        }
    }
    
        // Кнопка отпущена
    if (!current_state) {

        if (prev_state && (millis() - press_start_time) < 1000) {
            BUTT1_is_Clicked = 1;
            prev_state = 0;
            return 1;  // Короткое нажатие обнаружено!
        }
        prev_state = 0;
        BUTT1_is_Pressed = 0;
        BUTT1_is_Clicked = 0;
    }
    return 0;
}

uint8_t BUTT2_Handler(void) {
    static uint32_t press_start_time = 0;
    static uint8_t prev_state = 0;
    static uint8_t was_pressed = 0;
    
    uint8_t current_state = BUTT2_State_Check;
    
    // Кнопка только что нажата
    if (current_state && !prev_state) {
        press_start_time = millis();
        BUTT2_is_Clicked = 0;
        BUTT2_is_Pressed = 0;
        prev_state = 1;
    }
    
    // Кнопка удерживается
    if (current_state && prev_state) {
        // Проверяем, прошла ли 1 секунда
        if (was_pressed){
            BUTT2_is_Pressed = 0;
            return 0;
        }

        if ((millis() - press_start_time) >= 1000 && !BUTT2_is_Pressed) {
            BUTT2_is_Pressed = 1;
            was_pressed = 1;
            return 1;  // Длинное нажатие обнаружено!
        }
    }
    
        // Кнопка отпущена
    if (!current_state) {

        if (prev_state && (millis() - press_start_time) < 1000) {
            BUTT2_is_Clicked = 1;
            prev_state = 0;
            return 1;  // Короткое нажатие обнаружено!
        }
        prev_state = 0;
        was_pressed = 0;
        BUTT2_is_Pressed = 0;
        BUTT2_is_Clicked = 0;
    }
    return 0;
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
    millis_init();

    while (1){ 
        millis_update();
        //Обработчик BUTTON1 - пин PA4
        BUTT1_State = BUTT1_State_Check; //Считывание состояния пина PA4 в регистре GPIO_IDR
        BUTT1_Handler();
        BUTT2_Handler();



        if (BUTT1_is_Clicked)
        {
            if (LEDS_freq[LEDS_number] >= 4.5) {
                LEDS_freq[LEDS_number] = 0.5;
            }
            else {
            LEDS_freq[LEDS_number] +=2;
            }

        }

        if (BUTT1_is_Pressed){
        }

        if (BUTT2_is_Clicked)
        {
            switch (LEDS_number)
            {
            case 0:
                LEDS_isActive[0] = !LEDS_isActive[0];
                break;
            case 1:
                LEDS_isActive[1] = !LEDS_isActive[1];
                break;
            case 2:
                LEDS_isActive[2] = !LEDS_isActive[2];
                break;
            default:
                break;
            }
        }

        if (BUTT2_is_Pressed){
            if (!mode) {
                LEDS_number++;
            }
            else {
                LEDS_number--;
            }
            if (LEDS_number >= 3) {
                LEDS_number = 1;
                mode = !mode;
            }
            if (LEDS_number < 0) {
                LEDS_number = 1;
                mode = !mode;
            }
        }

        if (prev_LEDS_number != LEDS_number)
        {
            LED_Sparkle(LEDS_number);
            prev_LEDS_number = LEDS_number;
        }
        

        for (uint8_t i = 0; i < 3; i++)
        {
            if (LEDS_isActive[i]){
                LED_Blink(i,LEDS_freq);
            }
            else{
                switch (i)
                {
                case 0:
                    LED_R_RESET;
                    break;
                case 1:
                    LED_Y_RESET;
                    break;
                case 2:
                    LED_W_RESET;
                    break;
                default:
                    break;
                }
            }
        }
        
    }
}