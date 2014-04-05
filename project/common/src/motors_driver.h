/*!
 @file motors_driver.h
 @author Nicholas Destounis
 @author Nikolaos Bukas
 @author Michael Smith
 @author Kevin Dam
 @brief This is an implementation of a motor driver for both motors including functionality for initializing and moving to specific angles.
 */
 
/*! @addtogroup Microp Project Group 1
 *  @{
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

#define TIMER_CLOCK_FREQ 1000000	/*!< Clock frequency to timer */
#define PWM_PERIOD 20000					/*!< PWM period in us */

#define ROLL_DUTY_CYCLE_AT_90_DEG 2480 /*!< Duty cycle of 90 degree roll motor angle in us */
#define ROLL_DUTY_CYCLE_AT_0_DEG 1100	/*!< Duty cycle of 0 degree roll motor angle in us */

#define PITCH_DUTY_CYCLE_AT_90_DEG 2500 /*!< Duty cycle of 90 degree pitch motor angle in us */
#define PITCH_DUTY_CYCLE_AT_0_DEG 1550	/*!< Duty cycle of 0 degree pitch motor angle in us */

/*!
 Initialize both motors. 
 */
void init_motors(void);

/*!
 Initialize roll the motor. 
 */
void init_roll_motor(void);

/*!
 Initialize the pitch motor. 
 */
void init_pitch_motor(void);

/*!
 Move roll motor to angle specified.
 @param[in] angle The angle to move to (between -90 and 90)
 */
 void roll_move_to_angle(int angle);
 
 /*!
 Move pitch motor to angle specified.
 @param[in] angle The angle to move to (between -90 and 90)
 */
 void pitch_move_to_angle(int angle);

/*!
 Move both motors to angles specified.
 @param[in] roll The roll angle to move to (between -90 and 90)
 @param[in] pitch The pitch angle to move to (between -90 and 90)
 */
void move_to_angles(int roll, int pitch);

//! @} 
