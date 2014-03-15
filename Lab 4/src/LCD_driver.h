#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

// For GPIO Config
#include "stm32f4xx_conf.h"
// For OS functions like delay
#include "cmsis_os.h"


	/* Pin mapping:
  B0 = Reg select
	B1 = read/write
	B2 = enable
	B4 = data 0
	B5 = data 1
	B7 = data 2
	B8 = data 3
	B11 = data 4
	B12 = data 5
	B13 = data 6
	B14 = data 7
*/

#define LCD_GPIO_BANK GPIOB
#define LCD_REG_SEL GPIO_Pin_0
#define LCD_RW GPIO_Pin_1
#define LCD_EN GPIO_Pin_2
#define LCD_DATA_0 GPIO_Pin_4
#define LCD_DATA_1 GPIO_Pin_5
#define LCD_DATA_2 GPIO_Pin_7
#define LCD_DATA_3 GPIO_Pin_8
#define LCD_DATA_4 GPIO_Pin_11
#define LCD_DATA_5 GPIO_Pin_12
#define LCD_DATA_6 GPIO_Pin_13
#define LCD_DATA_7 GPIO_Pin_14


// Commands in Hex.  From "A generic keypad and LCD tutorial"
#define clearDisplay (uint8_t)0x01
#define displayCursorHome (uint8_t)0x02
#define displayOn (uint8_t)0xC
// 8 bit data lines, 2 LCD rows, 5x7 characters
#define functionSet (uint8_t)0x38


// For selecting specific bits of a number (i.e. ANDing)
#define BIT1 (uint8_t)0x1
#define BIT2 (uint8_t)0x2
#define BIT3 (uint8_t)0x4
#define BIT4 (uint8_t)0x8
#define BIT5 (uint8_t)0x10
#define BIT6 (uint8_t)0x20
#define BIT7 (uint8_t)0x40
#define BIT8 (uint8_t)0x80

void LCD_configure(void);
void printLCDString(char* string, int length, int row);
void printLCDToPos(char* string, int length, int row, int col);
void clearLCD(void);

#endif
