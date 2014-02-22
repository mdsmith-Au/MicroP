#include "motor.h"

/*
 * From the PWM Ouput example from ST,
 * we know in this case 
 * TIM1CLK = 2 * PCLK2  (PCLK1 = APB2 clock -> clock of bus TIM1 is on)
 * but  PCLK1 = HCLK / 2
 * thus TIM1CLK = HCLK = SystemCoreClock
 * There is also the counter clock (CC), output clock and prescaler
 
 * Prescaler = (TIM4CLK / TIM4 counter clock) - 1
 * Prescaler = ((SystemCoreClock) / TIM4 counter clock) - 1

 * Output clock period ARR = (TIM4 counter clock / TIM4 output clock) - 1
 
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
void PWM_configure() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	// Enable clock to TIM1
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	// Prescaler set dynamically based on Clock Freq, useful if we change it later
    // According to Doc 018909 rev 6 (p. 627), sec 18.4.11:
    // Counter clock frequency CK_CNT = fCK_PSC / (PSC[15:0] + 1).
    // Hence, we must subtract 1 so that our prescaler value is simply fCK_PSC / PSC[15:0]
	uint16_t PrescalerValue                   = (uint16_t)((SystemCoreClock)/1000000) - 1;
	
	TIM_TimeBaseInitStruct.TIM_Period         = ARR;
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

void motor_move_to_angle(int angle) {
  // Calculate duty cycle
  uint16_t duty = NEUTRAL + (angle*DEGREE_INCREMENT);
  // Write to register
  TIM1->CCR4 = duty;
  
}
