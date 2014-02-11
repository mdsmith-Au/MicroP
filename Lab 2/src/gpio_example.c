#include "GPIO_example.h"

void GPIO_example_config(void){
	GPIO_InitTypeDef GPIO_InitStructure,GPIO_InitStructure2, GPIO_InitStructure3;
	
	// Enable clock to power GPIOD and GPIOE bank
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
	
	/* Pin 14: set red led to PWM*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //Alternate function -> PWM
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Drive Pin 12 (Green Led), normal output */
	GPIO_InitStructure2.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure2);
	
	/* Pins 3 - 5: select
	* Pins 6 - 13 : segment
  * Let them be used for powering display	*/
	GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure3.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure3.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure3.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure3);
	
	// Connect GPIOD Pin 14 to TIM 4 for PWM (TIM4 because that pin is connected
	// to it, see board manual)
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);

}




