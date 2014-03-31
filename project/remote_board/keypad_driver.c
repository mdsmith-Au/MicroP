#include "keypad_driver.h"


void Keypad_GPIO_setup(void);

void Keypad_configure() {
    Keypad_GPIO_setup();
}

void Keypad_GPIO_setup() {
    GPIO_InitTypeDef Keypad_Row_GPIO_InitStruct, Keypad_Col_GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    Keypad_Col_GPIO_InitStruct.GPIO_Pin = KEYPAD_PIN_1 | KEYPAD_PIN_2 | KEYPAD_PIN_3 | KEYPAD_PIN_4;
    Keypad_Col_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    Keypad_Col_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    Keypad_Col_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    
    GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Col_GPIO_InitStruct);

    Keypad_Row_GPIO_InitStruct.GPIO_Pin = KEYPAD_PIN_5 | KEYPAD_PIN_6 | KEYPAD_PIN_7 | KEYPAD_PIN_8;
    Keypad_Row_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    Keypad_Row_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    Keypad_Row_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    
    GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Row_GPIO_InitStruct);
}
