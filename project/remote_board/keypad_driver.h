#ifndef _KEYPAD_DRIVER_H_
#define _KEYPAD_DRIVER_H_

#include "stm32f4xx_conf.h"
#include "LCD_driver.h"


/* Keypad pin mapping (GPIO Pins <-> Keypad pins)
 *  Rows (1, 4, 7, *)
 *   PD2  = Pin 5
 *   PD3  = Pin 6
 *   PD6  = Pin 7
 *   PD7  = Pin 8
 *  Columns (1, 2, 3, A)
 *   PD8  = Pin 1
 *   PD9  = Pin 2
 *   PD10 = Pin 3
 *   PD11 = Pin 4
 */
void Keypad_configure(void);
uint16_t Keypad_Handle_Interrupt(void);
char Keypad_Get_Character(uint32_t EXTI_Line_Source, uint16_t rows);

void enable_interrupt(uint32_t EXTI_Line);
void disable_interrupt(uint32_t EXTI_Line);
 
#define KEYPAD_GPIO_BANK GPIOD
// Columns
#define KEYPAD_PIN_1 GPIO_Pin_8
#define KEYPAD_PIN_2 GPIO_Pin_6
#define KEYPAD_PIN_3 GPIO_Pin_10
#define KEYPAD_PIN_4 GPIO_Pin_11
// Rows
#define KEYPAD_PIN_5 GPIO_Pin_2
#define KEYPAD_PIN_6 GPIO_Pin_3
#define KEYPAD_PIN_7 GPIO_Pin_6
#define KEYPAD_PIN_8 GPIO_Pin_7

#define KEYPAD_COL KEYPAD_PIN_1 | KEYPAD_PIN_2 | KEYPAD_PIN_3 | KEYPAD_PIN_4
#define KEYPAD_ROW KEYPAD_PIN_5 | KEYPAD_PIN_6 | KEYPAD_PIN_7 | KEYPAD_PIN_8

#define KEPAD_INT_SRC_1 EXTI_PinSource8
#define KEPAD_INT_SRC_2	EXTI_PinSource6
#define KEPAD_INT_SRC_3	EXTI_PinSource10
#define KEPAD_INT_SRC_4 EXTI_PinSource11

#define KEYPAD_INT_SRC KEPAD_INT_SRC_1 | KEPAD_INT_SRC_2 | KEPAD_INT_SRC_3 | KEPAD_INT_SRC_4

#define KEYPAD_INT_LINE_1 EXTI_Line8
#define KEYPAD_INT_LINE_2 EXTI_Line6
#define KEYPAD_INT_LINE_3 EXTI_Line10
#define KEYPAD_INT_LINE_4 EXTI_Line11

#define KEYPAD_INT_LINE KEYPAD_INT_LINE_1 | KEYPAD_INT_LINE_2 | KEYPAD_INT_LINE_3 | KEYPAD_INT_LINE_4

#define KEYPAD_EXTI_IRQ_1 EXTI9_5_IRQn
#define KEYPAD_EXTI_IRQ_2 EXTI9_5_IRQn
#define KEYPAD_EXTI_IRQ_3 EXTI15_10_IRQn
#define KEYPAD_EXTI_IRQ_4 EXTI15_10_IRQn

#define KEYPAD_EXTI_IRQ KEYPAD_EXTI_IRQ_1 | KEYPAD_EXTI_IRQ_2 | KEYPAD_EXTI_IRQ_3 | KEYPAD_EXTI_IRQ_4

#endif
