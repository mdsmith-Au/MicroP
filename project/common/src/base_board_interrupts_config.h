/*!
 @file base_board_interrupts_config.h
 @author Nicholas Destounis
 @author Nikolaos Bukas
 @author Michael Smith
 @author Kevin Dam
 @brief This deals with all hardware interrupt configuration for the base board
 */
 
/*! @addtogroup Microp Project Group 1
 *  @{
 */

#include "stm32f4xx_tim.h"

#define TIM2_PERIOD 5000	/*!< TIM2 period in us */

/*!
 Initialize interrupts for timer 2
 */
void tim2_interrupt_config(void);

//! @} 
