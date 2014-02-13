/**
 * @file alarm.c
 * Temperature alarm implementation file.
 * The alarm (which is simply a red LED light) will turn on 
 * once the circuit reaches a certain temperature threshold.
 * The LED light will fade in and out as determined by the 
 * board's Pulse Width Modulation component.
 */

#include "alarm.h"

// Keep track of whether we increasing or decreasing brightness
int8_t increaseBrightness = 1;
// Used to see if the alarm was already on when cooling down so that
// we turn off the LED gracefully
int8_t alarmOn = 0;

void PWM_configure() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	/*
	 * APB Max 142 Mhz
	 * From the PWM Ouput example from ST,
	 * we know in this case 
	 * TIM4CLK = 2 * PCLK1  (PCLK1 = APB1 clock -> clock of bus TIM4 is on)
   * but  PCLK1 = HCLK / 4
	 * thus TIM4CLK = HCLK / 2 = SystemCoreClock /2
	
	 * There is also the counter and output clock
	 * Counter clock depends on prescaler
	 * Prescaler = (TIM4CLK / TIM4 counter clock) - 1
   * Prescaler = ((SystemCoreClock /2) /DESIRED_MHZ) - 1
	
	 * Output clock period ARR = (TIM4 counter clock / TIM4 output clock) - 1
	 
	 * Duty cycle = CC register / ARR
	
	 * We choose 50 Mhz counter clock, since we don't need a very
	 * fast output for an LED.  Same goes for the output clock,
	 * which we set to 20Khz => ARR = 2500
	*/
	
	// Enable clock to TIM4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	// Prescaler set dynamically based on Clock Freq, useful if we change it later
	uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 50000000) - 1;
	
	/* Basic TIM Config */
	TIM_TimeBaseInitStruct.TIM_Period = ARR;
  TIM_TimeBaseInitStruct.TIM_Prescaler = PrescalerValue;
	/* No need to divide the clock in our case */
  TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	/* Note: TIM_RepetitionCounter does not apply  to TIM4 */
	
	/* Send struct to be processed */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	/* Now set up PWM4 for the specific channel we need - CH 3 for RED Led */
	
	// PWM mode 1 - Channel 1 is active as long as TIMx_CNT < TIMx_CCR1 else inactive
	// i.e. active as long as counter less than CC register
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	// By default, leave LED disabled -> no pulse (0% duty cycle)
  TIM_OCInitStruct.TIM_Pulse = 0;
	// Active High for our LED...want it to be on at 100%
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	/* Process struct for channel 3*/
	TIM_OC3Init(TIM4, &TIM_OCInitStruct);
	/* Enable OC preload so that behaviour is guaranteed per
	 * p. 450 of Doc ID 018909 Rev 1.  Update occurs when counter overflows */
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	/* Similar for ARR preload */
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	/* Enable counter -> start */
	TIM_Cmd(TIM4, ENABLE);
}

void alarmCheckTemp(float temp) {
	
	// Below threshold; reset
	if ((temp - TEMP_THRESHOLD) < -(TEMP_ERROR) ) {
		resetPWMDutyCycle();
		alarmOn = 0;
	}
	// Activate LED if temp > threshold
	else if ((temp - TEMP_THRESHOLD) > TEMP_ERROR) {
		triggerAlarm();
	}
	// Within margin of error (Threshold +- error): keep alarm on, but only if it was already on
	else if (alarmOn) {
		triggerAlarm();
	}
	
}

static void triggerAlarm(){
		
	alarmOn = 1;
	// Change between increasing/decreasing duty cycle -> fade in/out
	if (increaseBrightness) {
		increasePWMDutyCycle();
	}
	else {
		decreasePWMDutyCycle();
	}
}

/* Below methods change duty cycle reg. (CCR) for CH3 on TIM4 
 * Note: Increase/decrease of CCR3 must be a factor of ARR
 * Otherwise, the checks for max or min brighteness will fail*/
static void increasePWMDutyCycle(){
	TIM4->CCR3 = (TIM4->CCR3 + 100) ;
	
	// At max brightness -> decrease next time
	if (TIM4->CCR3 == ARR) {
		increaseBrightness = 0;
	}
}

static void decreasePWMDutyCycle() {
	TIM4->CCR3 = (TIM4->CCR3 - 100) ;
		
	// Min brightness (i.e. off) -> increase next time
	if (TIM4->CCR3 == 0) {
		increaseBrightness = 1;
	}
}

// Resets PWM Duty cycle to 0 if not already set
static void resetPWMDutyCycle() {
	if (TIM4->CCR3 != 0) {
		TIM4->CCR3 = 0;
		increaseBrightness = 1;
	}
}
