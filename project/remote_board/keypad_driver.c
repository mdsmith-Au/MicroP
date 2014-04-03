#include "keypad_driver.h"

// Global variable used to define whether or not NVIC prio. group has already been set
uint8_t NVIC_PRIORITY_SET = 0;

void Keypad_GPIO_setup(void);
void Keypad_Reverse_GPIO(void);

void Keypad_configure() {
    Keypad_GPIO_setup();
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
  
	Keypad_Row_GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW;
  Keypad_Row_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  Keypad_Row_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  Keypad_Row_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Row_GPIO_InitStruct);
	
  Keypad_Col_GPIO_InitStruct.GPIO_Pin = KEYPAD_COL;
  Keypad_Col_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  Keypad_Col_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  Keypad_Col_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(KEYPAD_GPIO_BANK, &Keypad_Col_GPIO_InitStruct);
  
  GPIO_SetBits(KEYPAD_GPIO_BANK, KEYPAD_COL);
  
}


char Keypad_Get_Character(uint16_t data) {
  int row = 0;
  // Figure out row
  if ((data & KEYPAD_PIN_5) == KEYPAD_PIN_5) {
    row = 1;
  }
  else if ((data & KEYPAD_PIN_6) == KEYPAD_PIN_6) {
    row = 2;
  }
  else if ((data & KEYPAD_PIN_7) == KEYPAD_PIN_7) {
    row = 3;
  }
  else if ((data & KEYPAD_PIN_8) == KEYPAD_PIN_8) {
    row = 4;
  }
  
  int col = 0;
  // Figure out column
  if ((data & KEYPAD_PIN_1) == KEYPAD_PIN_1) {
    col = 1;
  }
  else if ((data & KEYPAD_PIN_2) == KEYPAD_PIN_2) {
    col = 2;
  }
  else if ((data & KEYPAD_PIN_3) == KEYPAD_PIN_3) {
    col = 3;
  }
  else if ((data & KEYPAD_PIN_4) == KEYPAD_PIN_4) {
    col = 4;
  }
  
  char key = 0;
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

uint16_t Keypad_poll(void) {
	
	uint16_t data = GPIO_ReadInputData(KEYPAD_GPIO_BANK);
	
	Keypad_Reverse_GPIO();
  
  // Poll rows for source; get whole register since time is of the essense
  data |= GPIO_ReadInputData(KEYPAD_GPIO_BANK);
  
  // Reset GPIO
  Keypad_GPIO_setup();
  
  return data;
}
