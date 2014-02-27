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

/**
 * Controls the rate of how often the LED 7-segment displays are updated.
 */
static volatile uint_fast16_t delay;

/* Main entry point */
int main()
{
  
  // Clear interrupt flags (and 7-seg update delay)
  ext_interrupt = 0;
  tim3_interrupt = 0;
  delay = 0; 
  
  // Configure GPIO, peripherals and interrupts
	GPIO_configure();
	PWM_configure();
  Accelerometer_configure();
  Interrupts_configure();
  
  //Reset motor to 0 deg
  motor_move_to_angle(0);
	
	// Software delay run on reset button press
	displayNum(0);
	int i = 0;
	while(i < 10000000) {
		draw();
		i++;
	}
      
  while(1) {
    
    // Do nothing while waiting for an interrupt
    while(! (ext_interrupt || tim3_interrupt));
    
    // External interrupt happened
    if (ext_interrupt == 1) {
      
      // Reset the software interrupt flag after sensor interrupt is handled.
      ext_interrupt = 0;	
      
      // Get acclerometer data
      int x,y,z;
      Accelerometer_get_data( &x, &y, &z);
      
      // Convert acceleration to pitch & roll
      int roll = Accelerometer_get_roll(x,y,z);
      int pitch = Accelerometer_get_pitch(x,y,z);
      
      // Set motor based on roll
      motor_move_to_angle(roll);
      
      // Update the pitch shown on LED displays only if the delay time control has elapsed.
      if (delay == 0) {
        displayNum(pitch);
      }
		
      // Increment the delay time control (refresh rate of display)
      delay = (delay + 1) % 8;

      printf("Roll: %i, Pitch: %i\n", roll, pitch);
    }
    // Not external interrupt; must have been the timer
    else {
      // Reset flag
      tim3_interrupt = 0;
      // Power the displays
      draw();
    }
    
  }
}

// External interrupt on line 0 : INT1 on acclerometer
void EXTI0_IRQHandler()
{
  // Clear hardware flag; activate software flag
  ext_interrupt = 1;
  EXTI_ClearITPendingBit(EXTI_Line0);
  // Note that the interrupt on the accelerometer is cleared 
  // if, and ONLY if, data is read from it.  The status
  // registers that hold the interrupt line high cannot be modified manually
  
}

// Timer 3 interrupt
void TIM3_IRQHandler() {
  // Clear hardware flag; enable software one
  tim3_interrupt = 1;
  TIM_ClearITPendingBit( TIM3, TIM_IT_Update);
}
