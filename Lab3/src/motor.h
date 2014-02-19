#ifndef _MOTOR_H_
#define _MOTOR_H_

/**
 * @file motor.h
 * Header file for the PWM servo motor.
 * The motor is rotated according to tilt
 * data from the accelerometer.
 */

#include <stdint.h>                 /* For things like uint16_t */
#include "stm32f4xx_tim.h"          /* PWM Timer */
#include "system_stm32f4xx.h"       /* Variables such as SystemCoreClock */
#include "stm32f4xx_rcc.h"

/**
 * The ARR register is used to store the desired period of the PWM.
 */
#define ARR 19999

/**
 * Configures the Pulse Width Modulator.
 */
void PWM_configure(void);
#endif
