#include "LCD_driver.h"


// Private method definitions
void LCD_GPIO_setup(void);
void printToAddress(char* string, int length, uint8_t address);
void sendASCII(char data);
void sendCommand(uint8_t command);

void LCD_configure(void) {
	LCD_GPIO_setup();
  
  
}

void printLCDString(char* string, int length, LCD_Row row) {

  printLCDToPos(string, length, row, 1);

}


void printLCDToPos(char* string, int length, LCD_Row row, int col) {
	
  // Addresses start from 0, columns from 1
  // First row is from 0, second is from position 40
	printToAddress(string, length, (col-1) + row*40);
	
}



void printToAddress(char* string, int length, uint8_t address) {
	
  // Address commands have a 1 in the 8th bit, followed by the address
  // This sets the cursor to that position
  sendCommand(0x80 | address);
	
  
  // Send string one char at a time
  for (int i = 0; i < length; i++) {
    sendASCII(string[i]);
  }
}


void sendCommand(uint8_t command) {
	
	// Place command on data lines, one bit at a time
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_0, (BitAction)(command & BIT1));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_1, (BitAction)(command & BIT2));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_2, (BitAction)(command & BIT3));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_3, (BitAction)(command & BIT4));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_4, (BitAction)(command & BIT5));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_5, (BitAction)(command & BIT6));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_6, (BitAction)(command & BIT7));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_7, (BitAction)(command & BIT8));
	
  // Command mode has REG_SEL = 0, Read/Write = 0, Enable 0
	GPIO_ResetBits(LCD_GPIO_BANK, LCD_REG_SEL | LCD_RW | LCD_EN);
  
  // Bring enable up/down -> trig on falling edge
  GPIO_SetBits(LCD_GPIO_BANK, LCD_EN);
  // Give LCD time to process
  osDelay(1);
  GPIO_ResetBits(LCD_GPIO_BANK, LCD_EN);

}

void sendASCII(char data) {
	
	// Place command on data lines, one bit at a time
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_0, (BitAction)(data & BIT1));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_1, (BitAction)(data & BIT2));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_2, (BitAction)(data & BIT3));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_3, (BitAction)(data & BIT4));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_4, (BitAction)(data & BIT5));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_5, (BitAction)(data & BIT6));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_6, (BitAction)(data & BIT7));
	GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_7, (BitAction)(data & BIT8));
  
  	// ASCII write has REG_SEL = 1, Read/Write = 0, Enable 0
	GPIO_ResetBits(LCD_GPIO_BANK, LCD_RW |  LCD_EN);
	GPIO_SetBits(LCD_GPIO_BANK, LCD_REG_SEL);
	
  // Bring enable up/down -> trig on falling edge
  GPIO_SetBits(LCD_GPIO_BANK, LCD_EN);
  // Give LCD time to process
  osDelay(1);
  GPIO_ResetBits(LCD_GPIO_BANK, LCD_EN);
}


void LCD_GPIO_setup() {

	GPIO_InitTypeDef LCD_GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	LCD_GPIO_InitStruct.GPIO_Pin   = LCD_DATA_0 | LCD_DATA_1 | LCD_DATA_2 | LCD_DATA_3 | LCD_DATA_4 | LCD_DATA_5 | LCD_DATA_6 | LCD_DATA_7 | LCD_EN | LCD_RW | LCD_REG_SEL;
	LCD_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT; 
	LCD_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	LCD_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; 
	LCD_GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_GPIO_BANK, &LCD_GPIO_InitStruct);
	
	sendCommand(functionSet);
	sendCommand(displayOn);
	sendCommand(clearDisplay);
	sendCommand(displayCursorHome);
	
}
