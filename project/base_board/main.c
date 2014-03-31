#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "motors_driver.h"
#include "base_board_interrupts_config.h"

#define MOTOR_MOVE_SIGNAL 0x01
#define MOTOR_MESSAGE_QUEUE_SIZE 1000
#define INTERPOLATOR_MESSAGE_QUEUE_SIZE 1000

int motorPeriod = BASEBOARD_TIM2_PERIOD;

typedef struct {                               
	int rollAngle;
	int pitchAngle;
} Motor_message;

osPoolDef(motor_pool, MOTOR_MESSAGE_QUEUE_SIZE, Motor_message);                    // Define memory pool queue size 16 for now
osPoolId motor_pool;
osMessageQDef(motor_message_box, MOTOR_MESSAGE_QUEUE_SIZE, Motor_message);   //queue size 16 arbitrarily for now (queue should theoretically never get full)
osMessageQId motor_message_box;

typedef struct {                               
	int rollAngle;
	int pitchAngle;
	int delta_t; //in seconds
	int realtime; //is it realtime mode or not
} Interpolator_message;

osPoolDef(interpolator_pool, INTERPOLATOR_MESSAGE_QUEUE_SIZE, Interpolator_message);                    // Define memory pool queue size 16 for now
osPoolId interpolator_pool;
osMessageQDef(interpolator_message_box, INTERPOLATOR_MESSAGE_QUEUE_SIZE, Interpolator_message);   //queue size 16 arbitrarily for now (queue should theoretically never get full)
osMessageQId interpolator_message_box;

void motor_thread(const void* arg);
void interpolator_thread(const void* arg);
void wireless_thread(const void* arg);

osThreadDef(motor_thread, osPriorityNormal, 1, 0);
osThreadDef(interpolator_thread, osPriorityNormal, 1, 0);
osThreadDef(wireless_thread, osPriorityNormal, 1, 0);

osThreadId tid_motor, tid_interpolator, tid_wireless;

void TIM2_IRQHandler(void);

/*!
 @brief Program entry point
 */
int main (void)
{
	init_motors();
	move_to_angles(0, 0);
	
	osDelay(2000);
	
	baseboard_tim2_interrupt_config();
	
	motor_pool = osPoolCreate(osPool(motor_pool));                 // create memory pool
  motor_message_box = osMessageCreate(osMessageQ(motor_message_box), NULL);  // create msg queue
	
	interpolator_pool = osPoolCreate(osPool(interpolator_pool));                 // create memory pool
  interpolator_message_box = osMessageCreate(osMessageQ(interpolator_message_box), NULL);  // create msg queue
	
	tid_motor = osThreadCreate(osThread(motor_thread), NULL);
	tid_interpolator = osThreadCreate(osThread(interpolator_thread), NULL);
	
	Interpolator_message *interpolator_m;
	interpolator_m = osPoolAlloc(interpolator_pool);                     // Allocate memory for the message
	interpolator_m->rollAngle = 45;
	interpolator_m->pitchAngle = 45;
	interpolator_m->delta_t = 2;
	interpolator_m->realtime = 0;
	osMessagePut(interpolator_message_box, (uint32_t)interpolator_m, osWaitForever);  // Send Message
	
	interpolator_m = osPoolAlloc(interpolator_pool);                     // Allocate memory for the message
	interpolator_m->rollAngle = 0;
	interpolator_m->pitchAngle = 0;
	interpolator_m->delta_t = 2;
	interpolator_m->realtime = 0;
	osMessagePut(interpolator_message_box, (uint32_t)interpolator_m, osWaitForever);  // Send Message
	
	interpolator_m = osPoolAlloc(interpolator_pool);                     // Allocate memory for the message
	interpolator_m->rollAngle = 90;
	interpolator_m->pitchAngle = -90;
	interpolator_m->delta_t = 1;
	interpolator_m->realtime = 0;
	osMessagePut(interpolator_message_box, (uint32_t)interpolator_m, osWaitForever);  // Send Message
	
	//tid_wireless = osThreadCreate(osThread(wireless_thread), NULL);
	
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

void motor_thread(const void* arg)
{
	Motor_message  *motor_m;
  osEvent event;
	
	while(1)
	{
		osSignalWait(MOTOR_MOVE_SIGNAL, osWaitForever); //wait until it is time to move motor
		
		event = osMessageGet(motor_message_box, osWaitForever);  // wait for message
		
    if (event.status == osEventMessage)
		{
      motor_m = event.value.p;
			
			//move motors according to message received
			move_to_angles(motor_m->rollAngle, motor_m->pitchAngle);
			
      osPoolFree(motor_pool, motor_m);                  // free memory allocated for message
    }
	}
}
void interpolator_thread(const void* arg)
{
	Interpolator_message *interpolator_m;
	Motor_message *motor_m;
	osEvent event;
	
	int numMotorMessages;
	int i;
	
	int rollAngle;
	int pitchAngle;
	float rollAngleIncrement;
	float pitchAngleIncrement;
	
	int prevRollAngle = 0;
	int prevPitchAngle = 0;
	
	int rollAngleStart = 0;
	int pitchAngleStart = 0;
	
	while(1)
	{
		event = osMessageGet(interpolator_message_box, osWaitForever);  // wait for message
		
		if (event.status == osEventMessage)
		{
      interpolator_m = event.value.p;
			
			//If real time mode, ignore delta t and forward message to motor thread
			if (interpolator_m->realtime)
			{
				motor_m = osPoolAlloc(motor_pool);                     // Allocate memory for the message
				motor_m->rollAngle = interpolator_m->rollAngle;
				motor_m->pitchAngle = interpolator_m->pitchAngle;
				
				prevRollAngle = rollAngle;
				prevPitchAngle = pitchAngle;
				
				osMessagePut(motor_message_box, (uint32_t)motor_m, osWaitForever);  // Send Message
			}
			//else if keypad mode interpolate
			else
			{
				rollAngleStart = prevRollAngle;
				pitchAngleStart = prevPitchAngle;
				
				numMotorMessages = 1000000*interpolator_m->delta_t / motorPeriod;
				
				rollAngleIncrement = ((float)interpolator_m->rollAngle - prevRollAngle) / numMotorMessages;
				pitchAngleIncrement = ((float)interpolator_m->pitchAngle - prevPitchAngle) / numMotorMessages;
				
				for (i = 0; i < numMotorMessages; i++)
				{
					rollAngle = ceil(rollAngleStart + i*rollAngleIncrement);
					pitchAngle = ceil(pitchAngleStart + i*pitchAngleIncrement);
					
					motor_m = osPoolAlloc(motor_pool);                     // Allocate memory for the message
					motor_m->rollAngle = rollAngle;
					motor_m->pitchAngle = pitchAngle;
					
					prevRollAngle = rollAngle;
					prevPitchAngle = pitchAngle;
					
					osMessagePut(motor_message_box, (uint32_t)motor_m, osWaitForever);  // Send Message
				}
				motor_m = osPoolAlloc(motor_pool);                     // Allocate memory for the message
				motor_m->rollAngle = interpolator_m->rollAngle;
				motor_m->pitchAngle = interpolator_m->pitchAngle;
				
				prevRollAngle = rollAngle;
				prevPitchAngle = pitchAngle;
				
				osMessagePut(motor_message_box, (uint32_t)motor_m, osWaitForever);  // Send Message
			}
			
      osPoolFree(interpolator_pool, interpolator_m);                  // free memory allocated for message
    }
	}
}
void wireless_thread(const void* arg)
{
	Interpolator_message *interpolator_m;
	
	//initialize wireless
	
	while(1)
	{
		//wait for wireless receive
		
		interpolator_m = osPoolAlloc(interpolator_pool);                     // Allocate memory for the message
		interpolator_m->rollAngle = 0;
		interpolator_m->pitchAngle = 0;
		interpolator_m->delta_t = 0;
		interpolator_m->realtime = 0;
		osMessagePut(interpolator_message_box, (uint32_t)interpolator_m, osWaitForever);  // Send Message
	}
}

// Timer 2 interrupt for motor movement
void TIM2_IRQHandler() {
    osSignalSet(tid_motor, MOTOR_MOVE_SIGNAL);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}


