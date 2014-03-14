#ifndef _ALARM_H_
#define _ALARM_H_

/**
 * @file alarm.h
 * Header file for the LED temperature alarm.
 * The temperature alarm is used to detect a potentially dangerous 
 * increase in internal temperature. Upon exceeding a certain threshold 
 * temperature, the red LED will begin fading in and out between off 
 * and maximum brightness. The alarm will only turn off once the measured 
 * temperature falls below a threshold value.
 */

#include <stdint.h>                 /* For things like uint16_t */
#include "stm32f4xx_tim.h"          /* PWM Timer */
#include "system_stm32f4xx.h"       /* Variables such as SystemCoreClock */

/**
 * The ARR register is used to store the desired period of the PWM.
 */
#define ARR 2500

/**
 * The temperature threshold value (in degrees Celsius) which will trigger 
 * alarm activation.
 */
#define TEMP_THRESHOLD 45

/**
 * Temperature threshold standard deviation value. Its main purpose is to 
 * prevent the alarm from triggering and detriggering too quickly when the 
 * temperature value fluctuates near the threshold.
 */
#define TEMP_ERROR 1

/**
 * Configures the Pulse Width Modulator.
 */
void PWM_configure(void);

/**
 * Checks the temperature and triggers the alarm if temp is over 
 * the temperature threshold, or deactivates the alarm if 
 * below the threshold.
 * @param temp the current temperature
 */
void alarmCheckTemp(float temp);

#endif
