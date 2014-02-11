/**
 * @file alarm.h
 * Header file for the LED temperature alarm.
 */

// @TODO I've definitely seen stdint.h somewhere else. Will probably need to #ifndef this later...
#include <stdint.h>                 /* For things like uint16_t */
#include "system_stm32f4xx.h"       /* Variables such as SystemCoreClock */
#include "stm32f4xx_tim.h"          /* PWM Timer */

/**
 * @TODO: Unfortunately, I have no idea what this is.
 */
#define ARR 2500
#define TEMP_THRESHOLD 33    //TODO: CHANGE TO REAL VALUE
#define TEMP_ERROR 1

/**
 * Configures the Pulse Width Modulator.
 */
void initPWM(void);
void alarmCheckTemp(float temp);
static void increasePWMDutyCycle(void);
static void decreasePWMDutyCycle(void);
static void resetPWMDutyCycle(void);
