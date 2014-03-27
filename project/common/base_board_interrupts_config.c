#include "base_board_interrupts_config.h"

void tim2_interrupt_config() {
  // Get bus clock
  RCC_ClocksTypeDef clock_data;
  RCC_GetClocksFreq(&clock_data);
  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	// Enable clock to TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// Prescaler set dynamically based on Clock Freq, useful if we change it later
  // According to Doc 018909 rev 6 (p. 627), sec 18.4.11:
  // Counter clock frequency CK_CNT = fCK_PSC / (PSC[15:0] + 1).
  // Hence, we must subtract 1 so that our prescaler value is simply fCK_PSC / PSC[15:0]
	uint16_t PrescalerValue                   = (uint16_t)((2*clock_data.PCLK1_Frequency)/1000000) - 1;
  
	TIM_TimeBaseInitStruct.TIM_Period         = TIM2_PERIOD;
  TIM_TimeBaseInitStruct.TIM_Prescaler      = PrescalerValue;
  
	/* No need to further divide the clock in our case */   
  TIM_TimeBaseInitStruct.TIM_ClockDivision  = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode    = TIM_CounterMode_Up;
	/* Note: TIM_RepetitionCounter does not apply to TIM3 */

	/* Send struct to be processed */
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	/* Enable the period preload just in case */
	TIM_ARRPreloadConfig(TIM2, ENABLE);
  	
	// Priority group: 3 bits for pre-emption priority, 1 bit for subpriority
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); // chosen arbitrarily
	
  NVIC_InitTypeDef NVIC_InitStruct;
  // Use TIM3 interrupt
  NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
  // Priority settings. Give 5 .. below accelerometer
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
  // Don't really need sub priority
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  // Enable it
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  // Now tell the timer to enable interrupts
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
	/* Enable counter -> start */
	TIM_Cmd(TIM2, ENABLE);
}
