#include "keypad_driver.h"

// Global variable used to define whether or not NVIC prio. group has already been set
uint8_t NVIC_PRIORITY_SET = 0;

void Keypad_GPIO_setup(void);
void Keypad_Interrupt_configure(void);
void set_nvic_priority(void);
void Keypad_Reverse_GPIO(void);

void Keypad_configure() {
    Keypad_GPIO_setup();
		Keypad_Interrupt_configure();
}

void Keypad_GPIO_setup() {
    GPIO_InitTypeDef Keypad_Row_GPIO_InitStruct, Keypad_Col_GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    Keypad_Col_GPIO_InitStruct.GPIO_Pin = KEYPAD_COL;
    Keypad_Col_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    Keypad_Col_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    Keypad_Col_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    
    GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Col_GPIO_InitStruct);

    Keypad_Row_GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW;
    Keypad_Row_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    Keypad_Row_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    Keypad_Row_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Row_GPIO_InitStruct);
  
    GPIO_SetBits(KEYPAD_GPIO_BANK, KEYPAD_ROW);
}

void Keypad_Reverse_GPIO() {
  GPIO_InitTypeDef Keypad_Row_GPIO_InitStruct, Keypad_Col_GPIO_InitStruct;
  
  Keypad_Col_GPIO_InitStruct.GPIO_Pin = KEYPAD_COL;
  Keypad_Col_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  Keypad_Col_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  Keypad_Col_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Col_GPIO_InitStruct);
  
  Keypad_Row_GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW;
  Keypad_Row_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  Keypad_Row_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  Keypad_Row_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Row_GPIO_InitStruct);
  
  GPIO_SetBits(KEYPAD_GPIO_BANK, KEYPAD_COL);
  
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
  // Use external interrupt channels as in header file
  NVIC_InitStruct.NVIC_IRQChannel = KEYPAD_EXTI_IRQ ;
  // Priority settings. Give 6 .. not the highest, but fair
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;   // arbitrary
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


char Keypad_Get_Character(uint32_t EXTI_Line_Source, uint16_t rows) {
  int row = 0;
  // Figure out row
  if ((rows & KEYPAD_PIN_5) == KEYPAD_PIN_5) {
    row = 1;
  }
  else if ((rows & KEYPAD_PIN_6) == KEYPAD_PIN_6) {
    row = 2;
  }
  else if ((rows & KEYPAD_PIN_7) == KEYPAD_PIN_7) {
    row = 3;
  }
  else if ((rows & KEYPAD_PIN_8) == KEYPAD_PIN_8) {
    row = 4;
  }
  
  int col = 0;
  // Figure out column
  switch (EXTI_Line_Source) {
    case KEYPAD_INT_LINE_5: col = 1;
    case KEYPAD_INT_LINE_6: col = 2;
    case KEYPAD_INT_LINE_7: col = 3;
    case KEYPAD_INT_LINE_8: col = 4;
  }
  
  char key;
  switch(row) {
    case 1:
      switch (col) {
      case 1: key = '1';
      case 2: key = '2';
      case 3: key = '3';
      case 4: key = 'C';
    };
    case 2:
      switch (col) {
      case 1: key = '4';
      case 2: key = '5';
      case 3: key = '6';
      case 4: key = 'D';
        };
    case 3:
      switch (col) {
      case 1: key = '7';
      case 2: key = '8';
      case 3: key = '9';
      case 4: key = 'E';
        };
    case 4:
      switch (col) {
      case 1: key = 'A';
      case 2: key = '0';
      case 3: key = 'B';
      case 4: key = 'F';
        };
  }
  return key;
}

uint16_t Keypad_Handle_Interrupt(void) {
  Keypad_Reverse_GPIO();
  
  // Poll rows for source; get whole register since time is of the essense
  uint16_t rows = GPIO_ReadInputData(KEYPAD_GPIO_BANK);
  
  // Reset GPIO
  Keypad_GPIO_setup();
  
  return rows;
}

// Disables a given external interrupt
// ANDs the interrupt register with the NOT of the value of the interrupt line (masking)
// Ref: doc 018909 r1 sec 9.3.1 interrupt mask register
void disable_interrupt(uint32_t EXTI_Line) {
  uint32_t interr_reg = (uint32_t)EXTI_BASE;
  
  // Note: Use __IO here for volatile follwing ST driver convention
  *(__IO uint32_t *) interr_reg &= ~EXTI_Line;
}

// Enables the interrupt.  
// Same as disable, except the interrupt register is ORed with the value of the interrupt line
void enable_interrupt(uint32_t EXTI_Line) {
  uint32_t interr_reg = (uint32_t)EXTI_BASE;
  *(__IO uint32_t *) interr_reg |= EXTI_Line;
}
