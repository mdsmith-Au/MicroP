#include "mems_controller.h"

void mems_init()
{
	LIS302DL_InitTypeDef memsInit;
	GPIO_InitTypeDef gpioInit;
	EXTI_InitTypeDef extiInit;
	NVIC_InitTypeDef nvicInit;
	
	/* Enable the GPIO E and SYSCFG clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Configure MEMS */
	memsInit.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;														// Turn on power
	memsInit.Output_DataRate = LIS302DL_DATARATE_100;															// Produce output at 100 Hz
	memsInit.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
	memsInit.Full_Scale = LIS302DL_FULLSCALE_2_3;																	// TODO
	memsInit.Self_Test = LIS302DL_SELFTEST_NORMAL;																// Disable self-test
	LIS302DL_Init(&memsInit);
	
	/* 
	* LIS302DL_CTRL_REG3_ADDR
	* Bit 7 = 0 - Active HIGH
	* Bit 6 = 0 - Push-pull
	* Bit 5,4,3 = 100 - Set INT2 to DATA READY (PE_1)
	* Bit 2,1,0 = 000 - Set INT1 to GND (PE_0)
  */
	
	/* Configure the MEMS interrupts */
	uint8_t config = 32;
	LIS302DL_Write(&config, LIS302DL_CTRL_REG3_ADDR, 1);
	
	/* Configure GPIO for external interrupt input */
	gpioInit.GPIO_Mode = GPIO_Mode_IN;																						// Set port as output
	gpioInit.GPIO_Pin = GPIO_Pin_1;																								// Initialize pins 0, 1, 2 only
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;																				// No pull-up/pull-down operation for pins
	GPIO_Init(GPIOE, &gpioInit);
	
	/* Configure external interrupt to port E*/
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);									// Map PE_0 to EXTI_0
	
	/* Configure the EXTI */
	extiInit.EXTI_Line = EXTI_Line1;																							// Configure for EXTI_0
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;																			// Interrupt mode
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;																	// Rising-edge
	extiInit.EXTI_LineCmd = ENABLE;																								// Enable interrupt line
	EXTI_Init(&extiInit);
	
	/* Configure NVIC */
	nvicInit.NVIC_IRQChannel = EXTI1_IRQn;																				// Configure for EXTI_0
	nvicInit.NVIC_IRQChannelCmd = ENABLE;																					// Enable channel
	nvicInit.NVIC_IRQChannelPreemptionPriority = 1;																// Set second-highest priority
	nvicInit.NVIC_IRQChannelSubPriority = 1;																			// Set second-highest priority
	NVIC_Init(&nvicInit);
	
	/* Generate a software interrupt on EXTI line 0 in order to clear the data already read */
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}

void mems_get_acceleration(int* acc)
{
	LIS302DL_ReadACC(acc);
}
