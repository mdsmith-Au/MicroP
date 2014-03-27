#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "../common/motors_driver.h"

#define MOTOR_MOVE_SIGNAL 0x01


void TIM10_IRQHandler(void);

void motor_thread(const void* arg);
void interpolator_thread(const void* arg);
void wireless_thread(const void* arg);

osThreadDef(motor_thread, osPriorityNormal, 1, 0);
osThreadDef(interpolator_thread, osPriorityNormal, 1, 0);
osThreadDef(wireless_thread, osPriorityNormal, 1, 0);

osThreadId tid_motor, tid_interpolator, tid_wireless;

/*!
 @brief Program entry point
 */
int main (void)
{
	
	tid_motor = osThreadCreate(osThread(motor_thread), NULL);
	
	tid_interpolator = osThreadCreate(osThread(interpolator_thread), NULL);
	/*tid_wireless = osThreadCreate(osThread(wireless_thread), NULL);
	*/
	
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

void motor_thread(const void* arg)
{
	init_motors();
	
	int i = 0;
	
	while(1)
	{
		osSignalWait(MOTOR_MOVE_SIGNAL, osWaitForever);
		
		i = (i + 1) % 10;
	}
}
void interpolator_thread(const void* arg)
{
	while(1)
	{
		
	}
}
void wireless_thread(const void* arg)
{
	while(1)
	{
		
	}
}

// Timer 2 interrupt for motor movement
void TIM2_IRQHandler() {
    osSignalSet(tid_motor, MOTOR_MOVE_SIGNAL);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}


