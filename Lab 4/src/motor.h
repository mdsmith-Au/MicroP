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
 * Note that calculations are based on an ARR of 
 * 20000 for simplicity, but the difference is negligible
 */
#define MOTOR_ARR 19999

#define NEUTRAL 1500
// Neutral -> +90 deg : 1500 -> 2500 duty cycle
// Checked with protractor
#define DEGREE_INCREMENT ((2500-1500)/(float)90)
/**
 * Configures the Pulse Width Modulator.
 */
void Motor_PWM_configure(void);
void motor_move_to_angle(int angle);
#endif
