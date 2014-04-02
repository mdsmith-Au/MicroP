#include "keypad_driver.h"

// Global variable used to define whether or not NVIC prio. group has already been set
uint8_t NVIC_PRIORITY_SET = 0;

void Keypad_GPIO_setup(void);
void set_nvic_priority(void);

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

void Keypad_Interrupt_configure() {
	// Enable SYSCFG
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  // Enable interrupt for the columns
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, KEYPAD_INT_SRC);
  
  // Configure interrupts
  EXTI_InitTypeDef EXTI_InitStruct;
  
  EXTI_InitStruct.EXTI_Line = KEYPAD_INT_LINE;
  // Use interrupts (as opposed to events)
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  // Trigger interrupt on rising edge -> data ready.
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  // Enable interrupt
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
  
  // Configure NVIC
  set_nvic_priority();
  NVIC_InitTypeDef NVIC_InitStruct;
  // Use external interrupt channel 1 -> for pin 1
  NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
  // Priority settings. Give 4 .. not the highest, but fair
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;   // arbitrary
  // Don't need sub priority
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  // Enable it
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}

/* NVIC priority bits should be set before using NVIC, but we don't want to set
 * them manually if different parts of the code use the NVIC, since we could have one
 * piece of code setting it to one value and another setting it to another value.
 * Hence, this method which sets it once and only once. */
void set_nvic_priority() {

  if (!NVIC_PRIORITY_SET) {
    // Priority group: 3 bits for pre-emption priority, 1 bit for subpriority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); // chosen arbitrarily
    NVIC_PRIORITY_SET = 1;
  }
}
