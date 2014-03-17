#include "motor.h"

void Motor_GPIO_setup(void);

/*
 * From the PWM Ouput example from ST,
 * we know in this case 
 * TIM1CLK = 2 * PCLK2  (PCLK1 = APB2 clock -> clock of bus TIM1 is on)
 * There is also the counter clock (CC), output clock and prescaler
 
 * Prescaler = (TIM1CLK / TIM1 counter clock) - 1

 * Output clock period ARR = (TIM1 counter clock / TIM1 output clock) - 1
 
 * Duty cycle = CC register / ARR

 * We choose 1 MHz counter clock and 50Hz output clock.
 * 50Hz output is expected by the motor; 1Mhz counter clock
 * gives a high period while being below the max value for a 
 * 16 bit number (65536), allowing for precise operation.
 * => ARR = 1MHz / 50Hz - 1 = 19999
 * Values chosen by experimentation and need for large period
 * and thus a good degree of precision
 
 * For details, see Doc ID 018909 Rev 6 and Doc ID 022152 Rev 4
 */
void Motor_PWM_configure() {
	Motor_GPIO_setup();
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
  
  // Get bus clock
  RCC_ClocksTypeDef clock_data;
  RCC_GetClocksFreq(&clock_data);
	
	// Enable clock to TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	// Prescaler set dynamically based on Clock Freq, useful if we change it later
  // According to Doc 018909 rev 6 (p. 627), sec 18.4.11:
  // Counter clock frequency CK_CNT = fCK_PSC / (PSC[15:0] + 1).
  // Hence, we must subtract 1 so that our prescaler value is simply fCK_PSC / PSC[15:0]
	uint16_t PrescalerValue                   = (uint16_t)((2*clock_data.PCLK2_Frequency)/1000000) - 1;
	
	TIM_TimeBaseInitStruct.TIM_Period         = MOTOR_ARR;
  TIM_TimeBaseInitStruct.TIM_Prescaler      = PrescalerValue;
  
	/* No need to further divide the clock in our case */   
  TIM_TimeBaseInitStruct.TIM_ClockDivision  = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode    = TIM_CounterMode_Up;
	/* Note: TIM_RepetitionCounter does not apply in our case */
	
	/* Send struct to be processed */
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	
	/* Now set up PWM1 for the specific channel we need - CH 4 */
	
  /* PWM has two modes:
   * Mode 1 - while the counter is TIMx_CNT < TIMx_CCRx, the PWM output is high.
   * Mode 2 - while the counter is TIMx_CNT < TIMx_CCRx, the PWM output is low.
   */
	TIM_OCInitStruct.TIM_OCMode               = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState          = TIM_OutputState_Enable;
	// By default, leave LED disabled -> no pulse (0% duty cycle)
  TIM_OCInitStruct.TIM_Pulse                = 0;
	// Polarity_high means active high on (3V). Polarity low would be off when 100% duty cycle
  TIM_OCInitStruct.TIM_OCPolarity           = TIM_OCPolarity_High;
	
	/* Process struct for channel 4 */
	TIM_OC4Init(TIM1, &TIM_OCInitStruct);
	/* Enable OC preload so that behaviour is guaranteed per
	 * p. 450 of Doc ID 018909 Rev 1.  Update occurs automatically when counter overflows */
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* Enable the ARR preload just in case */
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	/* Main TIM1 output enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	/* Enable counter -> start */
	TIM_Cmd(TIM1, ENABLE);
}

void Motor_GPIO_setup() {
	GPIO_InitTypeDef Motor_GPIO_InitStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	Motor_GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_14;
	Motor_GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;            // Alternate Function (PWM)
	Motor_GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;           // Operating output type: push-pull
	Motor_GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_100MHz;       
	Motor_GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &Motor_GPIO_InitStruct);
	
	// Connect GPIODE Pin 14 to TIM 1 for PWM (TIM1 because it (and only it) is wired to that pin
	// See STM32F4 Discovery manual, Hardware and Layout, Table 5 (MCU pin description versus board function)
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);
}

void motor_move_to_angle(int angle) {
  // Calculate duty cycle
  uint16_t duty = NEUTRAL + (angle*DEGREE_INCREMENT);
  // Write to register
  TIM1->CCR4 = duty;
  
}
