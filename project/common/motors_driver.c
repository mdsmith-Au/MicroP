#include "motors_driver.h"

void init_motors()
{
	init_roll_motor();
	init_pitch_motor();
}

void init_roll_motor()
{	
	int prescalerValue;
	int prescalarFreq = TIMER_CLOCK_FREQ;
	//structures
	TIM_TimeBaseInitTypeDef  timBase;
	TIM_OCInitTypeDef  timeInit;

	GPIO_InitTypeDef motorInit;

	//enable clock of timer and motor gpio
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//initialize motor
	motorInit.GPIO_Mode = GPIO_Mode_AF;												// Alternate function: Motor wired to port C										
	motorInit.GPIO_Pin = GPIO_Pin_8;
	motorInit.GPIO_OType = GPIO_OType_PP;											// Default push-pull output mode. Do not want open drain
	motorInit.GPIO_PuPd = GPIO_PuPd_NOPULL;										// No pull-up/pull-down operation for pins
	motorInit.GPIO_Speed = GPIO_Speed_100MHz;									// Set speed to 100 MHz

	GPIO_Init(GPIOC, &motorInit);

	//map timer output to motor
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);

	//configures timer clock to prescalar frequency
	prescalerValue = (uint16_t) ((SystemCoreClock /2) / prescalarFreq) - 1;

	//initialize timer
	timBase.TIM_Period = PWM_PERIOD;											// Set period (number of cycles of timer clock)
	timBase.TIM_Prescaler = prescalerValue;							// Set prescaler
	timBase.TIM_ClockDivision = TIM_CKD_DIV1;						// Don't divide clock	
	timBase.TIM_CounterMode = TIM_CounterMode_Up;				// Count up mode

	TIM_TimeBaseInit(TIM3, &timBase);

	//initialize PWM channel
	timeInit.TIM_OCMode = TIM_OCMode_PWM1;							// Set timer to PWM mode
	timeInit.TIM_OutputState = TIM_OutputState_Enable;			
	timeInit.TIM_Pulse = 0;
	timeInit.TIM_OCPolarity = TIM_OCPolarity_High;			//Active high output

	TIM_OC3Init(TIM3, &timeInit);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	//enable counter on TIM3
	TIM_Cmd(TIM3, ENABLE);
}

void init_pitch_motor()
{
	GPIO_InitTypeDef Motor_GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	Motor_GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_14;
	Motor_GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;            // Alternate Function (PWM)
	Motor_GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;           // Operating output type: push-pull
	Motor_GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_100MHz;       
	Motor_GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &Motor_GPIO_InitStruct);

	// Connect GPIOE Pin 14 to TIM 1 for PWM (TIM1 because it (and only it) is wired to that pin
	// See STM32F4 Discovery manual, Hardware and Layout, Table 5 (MCU pin description versus board function)
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);
	
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
	uint16_t PrescalerValue                   = (uint16_t)((2*clock_data.PCLK2_Frequency)/TIMER_CLOCK_FREQ) - 1;
	
	TIM_TimeBaseInitStruct.TIM_Period         = PWM_PERIOD;
  TIM_TimeBaseInitStruct.TIM_Prescaler      = PrescalerValue;
  
	/* No need to divide the clock in our case */   
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

void roll_move_to_angle(int angle)
{
	float dutyCycle = ((ROLL_DUTY_CYCLE_AT_90_DEG - ROLL_DUTY_CYCLE_AT_0_DEG)/90.0f)*angle + ROLL_DUTY_CYCLE_AT_0_DEG;
	TIM_SetCompare3(TIM3, (int)dutyCycle);
}


void pitch_move_to_angle(int angle)
{
	float dutyCycle = ((PITCH_DUTY_CYCLE_AT_90_DEG - PITCH_DUTY_CYCLE_AT_0_DEG)/90.0f)*angle + PITCH_DUTY_CYCLE_AT_0_DEG;
	TIM_SetCompare4(TIM1, (int)dutyCycle);
}
