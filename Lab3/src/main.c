#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "motor.h"
#include "gpio_init.h"
#include "accelerometer.h"
#include "interrupts_config.h"
#include "display_driver.h"

/** 
 * Software flag set by a timer or external device when an interrupt is generated.
 * @c uint_fast16_t is used so the compiler uses the max speed implementation 
 * dependent on processor architecture. The interrupt service routine should 
 * be as fast as possible, hence the use of this "fast" type.
 */
static volatile uint_fast16_t ext_interrupt;
static volatile uint_fast16_t tim3_interrupt;

int main()
{
	GPIO_configure();
	PWM_configure();
  
  ext_interrupt = 0;
  tim3_interrupt = 0;
	
  Accelerometer_configure();
  Interrupts_configure();
  
  while(1) {
    
    
    while(! (ext_interrupt || tim3_interrupt));
    
    if (ext_interrupt == 1) {
      
      // Reset the software interrupt flag after interrupt is handled.
      ext_interrupt = 0;	
      
      int x,y,z;
      Accelerometer_get_data( &x, &y, &z);
      
      
      int roll = Accelerometer_get_roll(x,y,z);
      int pitch = Accelerometer_get_pitch(x,y,z);
      // Set roll to control motor
      motor_move_to_angle(roll);
      
      // Display pitch
      displayNum(pitch);
      
      
      printf("Roll: %i, Pitch: %i\n", roll, pitch);
    }
    else {
      //TIM3 interrupt
      tim3_interrupt = 0;
      draw();
    }
    
  }
}

void EXTI0_IRQHandler()
{
  // Clear hardware flag; activate software flag
  ext_interrupt = 1;
  EXTI_ClearITPendingBit(EXTI_Line0);
  
}

void TIM3_IRQHandler() {
  // Clear hardware flag; enable software one
  tim3_interrupt = 1;
  TIM_ClearITPendingBit( TIM3, TIM_IT_Update);
}
