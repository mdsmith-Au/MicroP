#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "motors_driver.h"
#include "base_board_interrupts_config.h"

#include "wireless_cc2500.h"
#include <stdio.h>

#define MOTOR_MOVE_SIGNAL 0x01
#define WIRELESS_SIGNAL 	0x02

#define MOTOR_MESSAGE_QUEUE_SIZE 1000
#define INTERPOLATOR_MESSAGE_QUEUE_SIZE 1000

int motorPeriod = BASEBOARD_TIM2_PERIOD;

typedef struct {                               
	int8_t rollAngle;
	int8_t pitchAngle;
} Motor_message;

osPoolDef(motor_pool, MOTOR_MESSAGE_QUEUE_SIZE, Motor_message);                    // Define memory pool queue size 16 for now
osPoolId motor_pool;
osMessageQDef(motor_message_box, MOTOR_MESSAGE_QUEUE_SIZE, Motor_message);   //queue size 16 arbitrarily for now (queue should theoretically never get full)
osMessageQId motor_message_box;

typedef struct {                               
	int8_t rollAngle;
	int8_t pitchAngle;
	int8_t delta_t; //in seconds
	int8_t realtime; //is it realtime mode or not
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
void read_wireless_message(Interpolator_message *m);
void wireless_timer_callback(void const *arg);


osTimerDef (wireless_timer, wireless_timer_callback); 
osTimerId wireless_timer_id;

/*!
 @brief Program entry point
 */
int main (void)
{
	init_motors();
	move_to_angles(0, 0);
	
	osDelay(5000);
	
	baseboard_tim2_interrupt_config();
	
	wireless_timer_id = osTimerCreate (osTimer(wireless_timer), osTimerPeriodic, NULL);
	osTimerStart(wireless_timer_id, 10);
	
	motor_pool = osPoolCreate(osPool(motor_pool));                 // create memory pool
  motor_message_box = osMessageCreate(osMessageQ(motor_message_box), NULL);  // create msg queue
	
	interpolator_pool = osPoolCreate(osPool(interpolator_pool));                 // create memory pool
  interpolator_message_box = osMessageCreate(osMessageQ(interpolator_message_box), NULL);  // create msg queue
	
	tid_motor = osThreadCreate(osThread(motor_thread), NULL);
	tid_interpolator = osThreadCreate(osThread(interpolator_thread), NULL);
	tid_wireless = osThreadCreate(osThread(wireless_thread), NULL);
	
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
			move_to_angles(motor_m->rollAngle, -motor_m->pitchAngle);
			
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
	int8_t numBytes;
	int8_t state;
	
	//initialize wireless
	CC2500_Init();
	CC2500_CmdStrobe(SRX);
	
	while(1)
	{
		osSignalWait(WIRELESS_SIGNAL, osWaitForever);
		
		//wait for wireless receive
		CC2500_Read_Reg(&numBytes, RXBYTES, 1);
		numBytes = numBytes & 0x7f;
		CC2500_Read_Reg(&state, MARCSTATE, 1);
		
		//printf("State: %x\n", state);
		osDelay(1);
		if(numBytes > 0)
		{
			interpolator_m = osPoolAlloc(interpolator_pool);                     // Allocate memory for the message
			read_wireless_message(interpolator_m);
			
			osMessagePut(interpolator_message_box, (uint32_t)interpolator_m, osWaitForever);  // Send Message
		}		
	}
}

// Timer 2 interrupt for motor movement
void TIM2_IRQHandler() {
    osSignalSet(tid_motor, MOTOR_MOVE_SIGNAL);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void read_wireless_message(Interpolator_message *m)
{
	int8_t packetSize;
	int8_t junk;
	
	CC2500_ReadFIFO(&packetSize, FIFO_READ_ADDRESS, 1);		
	CC2500_CmdStrobe(SRX);
	
	CC2500_ReadFIFO((int8_t*)m, FIFO_READ_BURST_ADDRESS, sizeof(Interpolator_message));		
	CC2500_CmdStrobe(SRX);
	
	CC2500_ReadFIFO(&junk, FIFO_READ_BURST_ADDRESS, 2);		
	CC2500_CmdStrobe(SRX);
}

void wireless_timer_callback(void const *arg)
{
	osSignalSet(tid_wireless, WIRELESS_SIGNAL);
}

