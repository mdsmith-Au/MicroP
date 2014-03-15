/**
 * @file alarm.c
 * Temperature alarm implementation file.
 * The alarm (which is simply a red LED light) will turn on 
 * once the circuit reaches a certain temperature threshold.
 * The LED light will fade in and out as determined by the 
 * board's Pulse Width Modulation component.
 */

#include "alarm.h"

void increasePWMDutyCycle(void);
void decreasePWMDutyCycle(void);
void resetPWMDutyCycle(void);
void triggerAlarm(void);
void Alarm_GPIO_configure(void);
void Alarm_PWM_configure(void);

/** Flag to track the LED brightness should increase or decrease. */
int8_t increaseBrightness = 1;

/** 
 * Flag to check if the alarm was already on when cooling down so that
 * the LED turns off gracefully.
 */
int8_t alarmOn = 0;


void Alarm_configure() {
  
  Alarm_GPIO_configure();
  Alarm_PWM_configure();
}

void Alarm_GPIO_configure() {
  
  GPIO_InitTypeDef Alarm_GPIO;
  
  // Enable clock to power GPIOD bank
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	/* Set red LED (Pin 14) to PWM */
	Alarm_GPIO.GPIO_Pin    = GPIO_Pin_14;
	Alarm_GPIO.GPIO_Mode   = GPIO_Mode_AF;            // Alternate Function (PWM)
	Alarm_GPIO.GPIO_OType  = GPIO_OType_PP;           // Operating output type: push-pull
	Alarm_GPIO.GPIO_Speed  = GPIO_Speed_100MHz;       
	Alarm_GPIO.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &Alarm_GPIO);
  
  // Connect GPIOD Pin 14 to TIM 4 for PWM (TIM4 because that pin for the red LED is connected to it
	// See STM32F4 Discovery manual, Hardware and Layout, Table 5 (MCU pin description versus board function)
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
}


/* APB Max 142 Mhz
 * From the PWM Ouput example from ST,
 * we know in this case 
 * TIM4CLK = 2 * PCLK1  (PCLK1 = APB1 clock -> clock of bus TIM4 is on)
 * but  PCLK1 = HCLK / 4
 * thus TIM4CLK = HCLK / 2 = SystemCoreClock /2
 * There is also the counter clock (CC), output clock and prescaler
 
 * Prescaler = (TIM4CLK / TIM4 counter clock) - 1
 * Prescaler = ((SystemCoreClock /2) / TIM4 counter clock) - 1

 * Output clock period ARR = (TIM4 counter clock / TIM4 output clock) - 1
 
 * Duty cycle = CC register / ARR

 * We choose 50 MHz counter clock, since we don't need a very
 * fast output for an LED.  Same goes for the output clock,
 * which we set to 20Khz => ARR = 50MHz / 20KHz - 1 = 2500
 * Values chosen by experimentation and need for large period
 * and thus a good degree of precision
 
 * For details, see Doc ID 018909 Rev 6 and Doc ID 022152 Rev 4
 */
void Alarm_PWM_configure() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	// Enable clock to TIM4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	// Prescaler set dynamically based on Clock Freq, useful if we change it later
    // According to Doc 018909 rev 6 (p. 627), sec 18.4.11:
    // Counter clock frequency CK_CNT = fCK_PSC / (PSC[15:0] + 1).
    // Hence, we must subtract 1 so that our prescaler value is simply fCK_PSC / PSC[15:0]
	uint16_t PrescalerValue                   = (uint16_t)((SystemCoreClock/2)/50000000) - 1;
	
	TIM_TimeBaseInitStruct.TIM_Period         = ALARM_ARR;
    TIM_TimeBaseInitStruct.TIM_Prescaler      = PrescalerValue;
  
	/* No need to further divide the clock in our case */   
    TIM_TimeBaseInitStruct.TIM_ClockDivision  = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode    = TIM_CounterMode_Up;
	/* Note: TIM_RepetitionCounter does not apply to TIM4 */
	
	/* Send struct to be processed */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	/* Now set up PWM4 for the specific channel we need - CH 3 for RED Led */
	
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
	
	/* Process struct for channel 3 */
	TIM_OC3Init(TIM4, &TIM_OCInitStruct);
	/* Enable OC preload so that behaviour is guaranteed per
	 * p. 450 of Doc ID 018909 Rev 1.  Update occurs automatically when counter overflows */
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	/* Enable the ARR preload just in case */
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

/* Activate alarm fade in/fade out effects */
void triggerAlarm(){
	alarmOn = 1;                        // Set alarm flag
	if (increaseBrightness) {
		increasePWMDutyCycle();         // Fade in
	}
	else {
		decreasePWMDutyCycle();         // Fade out
	}
}

/* Below methods change the duty cycle register (CCR) for CH3 on TIM4 
 * NOTE: Increase/decrease of CCR3 must be a factor of ARR
 * Otherwise, the checks for max or min brightness will fail */

// Increases PWM duty cycle for fade in
void increasePWMDutyCycle(){
	TIM4->CCR3 = (TIM4->CCR3 + 100);    // 100 determined by trial-and-error
	
	// At max brightness -> decrease next time
	if (TIM4->CCR3 == ALARM_ARR) {
		increaseBrightness = 0;
	}
}

// Decreases PWM duty cycle for fade out
void decreasePWMDutyCycle() {
	TIM4->CCR3 = (TIM4->CCR3 - 100);
		
	// Min brightness (i.e. off) -> increase next time
	if (TIM4->CCR3 == 0) {
		increaseBrightness = 1;
	}
}

// Resets PWM Duty cycle to 0 if not already set
void resetPWMDutyCycle() {
	if (TIM4->CCR3 != 0) {
		TIM4->CCR3 = 0;
		increaseBrightness = 1;
	}
}
