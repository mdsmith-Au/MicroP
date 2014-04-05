/*!
 @file mems_controller.h
 @author Nicholas Destounis
 @author Nikolaos Bukas
 @author Michael Smith
 @author Kevin Dam
 @brief This is an implementation of a MEMS controller including functionality for initializing and getting accelerometer readings.
 */
 
/*! @addtogroup Microp Project Group 1
 *  @{
 */

#include "stm32f4_discovery_lis302dl.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

/*!
 Get acceleration readings.
 @param[in,out] acc A pointer to the array to store acceleration values
 */
void mems_get_acceleration(int* acc);

/*!
 Initialize the motor. 
 */
void mems_init(void);

//! @}
