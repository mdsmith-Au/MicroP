#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "LCD_driver.h"
#include "mems_controller.h"
#include "atan_LUT.h"
#include "arm_math.h"
#include "filter.h"
#include "wireless_cc2500.h"
#include "keypad_driver.h"
#include "stdio.h"

#define WIRELESS_MESSAGE_QUEUE_SIZE 1000
#define KEYPAD_QUEUE_SIZE 10

#define ANGLE_FILTER_DEPTH 16	/*!< Filter depth for angle filters */
#define GRAV_ACC 1000.0f	/*!< Value of g */
#define NINETY_DEG_THRESH 1000	/*!< Threshold for 90 degree readings */
#define SENSOR_X_OFFSET 18	/*!< X offset from calibration */
#define SENSOR_Y_OFFSET 18	/*!< Y offset from calibration */
#define SENSOR_Z_OFFSET -64	/*!< Z offset from calibration (expected 1000)*/

#define ACCELERATON_FLAG	0x01	/*!< Acceleration signaling flag */
#define KEYPAD_FLAG	0x02

static int displayValue = 0;
static int RXNow = 0;

// mode switching
typedef enum {
  KEYPAD_MODE = 1, 
  REALTIME_MODE = 0
} OPERATION_MODE;

// Mode the board is in; see above
OPERATION_MODE mode = REALTIME_MODE;

typedef struct {                               
	int8_t rollAngle;
	int8_t pitchAngle;
	int8_t delta_t;						// 
	int8_t realtime;						// is it realtime mode or not
} Wireless_message;

typedef struct {
  uint32_t EXTI_Line;
  uint16_t row_data;
}Keypad_data;

static Filter rollFilter;
static Filter pitchFilter;
static Queue rollBuffer;
static Queue pitchBuffer;

osPoolDef(wireless_pool, WIRELESS_MESSAGE_QUEUE_SIZE, Wireless_message);                    // Define memory pool queue size 16 for now
osPoolId wireless_pool;

osMessageQDef(wireless_message_box, WIRELESS_MESSAGE_QUEUE_SIZE, Wireless_message);   //queue size 16 arbitrarily for now (queue should theoretically never get full)
osMessageQId wireless_message_box;

osPoolDef(keypad_pool, KEYPAD_QUEUE_SIZE, Keypad_data);
osPoolId keypad_pool;

osMessageQDef(keypad_message_box, KEYPAD_QUEUE_SIZE, Keypad_data);
osMessageQId keypad_message_box;

void orientation_thread(const void* arg);
void wireless_thread(const void* arg);
void keypad_thread(const void* arg);

osThreadDef(orientation_thread, osPriorityNormal, 1, 0);
osThreadDef(wireless_thread, osPriorityNormal, 1, 0);
osThreadDef(keypad_thread, osPriorityNormal, 1, 0);

osThreadId tid_orientation, tid_wireless, tid_keypad;

osSemaphoreId modeSemaphore;   
osSemaphoreDef(modeSemaphore);

osSemaphoreId displaySemaphore;   
osSemaphoreDef(displaySemaphore);

/*!
 ISR for external 0
 */
void EXTI0_IRQHandler(void);

/*!
	ISR for external 1 
 */
void EXTI1_IRQHandler(void);

/*!
 Timer ISR for LCD display
 */
void display_timer_ISR(void const *argument);

/*!
 Initialize user button
 */
void init_user_button(void);

/*!
 Get angle corresponding to given acceleration value
 @param[in] acc Acceleration value
 */
int get_angle(int acc);

/*!
 Initialize angle filtering
 */
void init_angle_filtering(void);

void write_wireless_message(Wireless_message *m);

/*!
 @brief Program entry point
 */
int main (void) {
	init_user_button();
	LCD_configure();
	
	//Set up LCD timer
	int display_period = 500; //500 milliseconds
	osTimerDef(display_timer, display_timer_ISR);
	osTimerId display_timerId = osTimerCreate(osTimer(display_timer), osTimerPeriodic, NULL);
	osTimerStart(display_timerId, display_period);
	
	osSemaphoreCreate(osSemaphore(displaySemaphore), 1);
	osSemaphoreCreate(osSemaphore(modeSemaphore), 1);
	
	//init message box and mem pool
	wireless_pool = osPoolCreate(osPool(wireless_pool));                 // create memory pool
  wireless_message_box = osMessageCreate(osMessageQ(wireless_message_box), NULL);  // create msg queue
	
	//start threads
	tid_orientation = osThreadCreate(osThread(orientation_thread), NULL);
	tid_wireless = osThreadCreate(osThread(wireless_thread), NULL);
  tid_keypad = osThreadCreate(osThread(keypad_thread), NULL);
	/*
			Wireless_message *wireless_m1;
			wireless_m1 = osPoolAlloc(wireless_pool);                     // Allocate memory for the message
			wireless_m1->rollAngle = 30;
			wireless_m1->pitchAngle = 0;
			wireless_m1->delta_t = 5;
			wireless_m1->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m1, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m2;
			wireless_m2 = osPoolAlloc(wireless_pool);
			wireless_m2->rollAngle = 30;
			wireless_m2->pitchAngle = 30;
			wireless_m2->delta_t = 2;
			wireless_m2->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m2, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m3;
			wireless_m3 = osPoolAlloc(wireless_pool);
			wireless_m3->rollAngle = 30;
			wireless_m3->pitchAngle = 0;
			wireless_m3->delta_t = 5;
			wireless_m3->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m3, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m4;
			wireless_m4 = osPoolAlloc(wireless_pool);
			wireless_m4->rollAngle = 0;
			wireless_m4->pitchAngle = 0;
			wireless_m4->delta_t = 2;
			wireless_m4->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m4, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m5;
			wireless_m5 = osPoolAlloc(wireless_pool);
			wireless_m5->rollAngle = -30;
			wireless_m5->pitchAngle = 0;
			wireless_m5->delta_t = 5;
			wireless_m5->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m5, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m6;
			wireless_m6 = osPoolAlloc(wireless_pool);
			wireless_m6->rollAngle = -30;
			wireless_m6->pitchAngle = -30;
			wireless_m6->delta_t = 2;
			wireless_m6->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m6, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m7;
			wireless_m7 = osPoolAlloc(wireless_pool);
			wireless_m7->rollAngle = -30;
			wireless_m7->pitchAngle = 0;
			wireless_m7->delta_t = 5;
			wireless_m7->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m7, osWaitForever);  // Send Message
			
			osDelay(1000);
			Wireless_message *wireless_m8;
			wireless_m8 = osPoolAlloc(wireless_pool);
			wireless_m8->rollAngle = 0;
			wireless_m8->pitchAngle = 0;
			wireless_m8->delta_t = 2;
			wireless_m8->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m8, osWaitForever);  // Send Message
	*/		
	Keypad_configure();
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

void orientation_thread(const void* arg)
{
	Wireless_message *wireless_m;

	int acc_x, acc_y;
	float filteredRollAngle = 0;
	float filteredPitchAngle = 0;
	int rollAngle, pitchAngle;
	int acc[] = {0, 0, 0};
	int calibratedAcc[] = {0, 0, 0};
	
	init_angle_filtering();
	mems_init();
	
	int samplingMode;
	
	while(1)
	{
		osSemaphoreWait(modeSemaphore, osWaitForever);
		samplingMode = (mode == KEYPAD_MODE)? 0: 1;
		osSemaphoreRelease(modeSemaphore);
		
		if (samplingMode)
		{
			osSignalWait(ACCELERATON_FLAG, osWaitForever);
			
			//Get accel values
			mems_get_acceleration(acc);
			
			//Get calibrated accel values
			calibratedAcc[0] = acc[0] - SENSOR_X_OFFSET;
			calibratedAcc[1] = acc[1] - SENSOR_Y_OFFSET;
			calibratedAcc[2] = acc[2] - SENSOR_Z_OFFSET;
			
			acc_x = calibratedAcc[0];
			acc_y = calibratedAcc[1];
			
			//calculate roll angle
			rollAngle = get_angle(acc_x);
			
			//calculate pitch angle
			pitchAngle = get_angle(acc_y);
			
			//Add angle measurements to corresp filters
			add_measurement(&rollFilter, rollAngle);
			add_measurement(&pitchFilter, pitchAngle);
			
			filteredRollAngle = rollFilter.avg;
			filteredPitchAngle = pitchFilter.avg;
			
			wireless_m = osPoolAlloc(wireless_pool);                     // Allocate memory for the message
			wireless_m->rollAngle = (int)filteredRollAngle;
			wireless_m->pitchAngle = (int)filteredPitchAngle;
			wireless_m->delta_t = 0;
			wireless_m->realtime = 1;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
		}
		else
		{
			//get information from keypad
			/*
			wireless_m = osPoolAlloc(wireless_pool);                     // Allocate memory for the message
			wireless_m->rollAngle = 90;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 10;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = 90;
			wireless_m->pitchAngle = 90;
			wireless_m->delta_t = 10;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = 90;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 5;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = 0;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 5;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = -90;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 10;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = -90;
			wireless_m->pitchAngle = -90;
			wireless_m->delta_t = 10;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = -90;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 5;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			
			wireless_m->rollAngle = 0;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 5;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
			*/
		}
	}
}

void wireless_thread(const void* arg)
{
	//init wireless
	CC2500_Init();
	CC2500_CmdStrobe(STX);
	
	Wireless_message  *wireless_m;
  osEvent event;
	
	int8_t numBytesFIFOBuffer;
	
	while(1)
	{
		CC2500_Read_Reg(&numBytesFIFOBuffer, TXBYTES, 1);
		numBytesFIFOBuffer = numBytesFIFOBuffer & 0x7f;
		
		//if fifo buffer is not full, send message else yield
		if (numBytesFIFOBuffer + sizeof(Wireless_message) <= FIFO_SIZE)
		{
			//printf("FIFO SIZE: %d\n", numBytesFIFOBuffer);
			event = osMessageGet(wireless_message_box, osWaitForever);  // wait for message
			if (event.status == osEventMessage)
			{
				wireless_m = event.value.p;
				
				//send wireless message
				//printf("Writing message... pitch: %d roll: %d\n", wireless_m->pitchAngle, wireless_m->rollAngle);
				write_wireless_message(wireless_m);
				
				osPoolFree(wireless_pool, wireless_m);                  // free memory allocated for message
			}
		}
		else
		{
			osThreadYield();
		}
	}
}

void keypad_thread(const void* arg) {
    clearLCD();
	char prevKeypress = 0;
	while(1)
	{
		osSignalWait(KEYPAD_FLAG, osWaitForever);
		uint32_t data= Keypad_poll();
        char currKeypress = Keypad_Get_Character(data);
        
        if (currKeypress != prevKeypress) {
            prevKeypress = currKeypress;
            
            if (currKeypress != 0) {
                printLCDCharKeypad(currKeypress);
            }
        }
	}
}

void display_timer_ISR(void const *argument)
{
	osSemaphoreWait(displaySemaphore, osWaitForever);
	displayValue = 1;
	osSemaphoreRelease(displaySemaphore);
}

int get_angle(int acc)
{	
	//If angle is beyond threshold, return constant value of 90
	if (acc >= NINETY_DEG_THRESH)
	{
		return 90;
	}
	else if (acc <= -NINETY_DEG_THRESH)
	{
		return -90;
	}
	else
	{
		return atan_table(acc/(sqrt(GRAV_ACC*GRAV_ACC - acc*acc)));
	}		
}

void init_angle_filtering()
{
	initialize_queue(&rollBuffer, ANGLE_FILTER_DEPTH);
	init_filter(&rollFilter, &rollBuffer, ANGLE_FILTER_DEPTH);
	initialize_queue(&pitchBuffer, ANGLE_FILTER_DEPTH);
	init_filter(&pitchFilter, &pitchBuffer, ANGLE_FILTER_DEPTH);
}

void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		osSemaphoreWait(modeSemaphore, osWaitForever);
		mode = (mode == REALTIME_MODE)? KEYPAD_MODE: REALTIME_MODE;
		osSemaphoreRelease(modeSemaphore);
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

// Interrupt should happen at a rate of 100Hz
void EXTI1_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
		osSignalSet(tid_orientation, ACCELERATON_FLAG);
		osSignalSet(tid_keypad, KEYPAD_FLAG);
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/* Button interrupt handler */
void init_user_button()
{
	EXTI_InitTypeDef extiInit;
	NVIC_InitTypeDef nvicInit;
	
	/* Enable the GPIO A and SYSCFG clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Configure external interrupt to port E*/
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);									
	
	/* Configure the EXTI */
	extiInit.EXTI_Line = EXTI_Line0;																							
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;																			
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;																	
	extiInit.EXTI_LineCmd = ENABLE;																								
	EXTI_Init(&extiInit);
	
	/* Configure NVIC */
	nvicInit.NVIC_IRQChannel = EXTI0_IRQn;																				
	nvicInit.NVIC_IRQChannelCmd = ENABLE;																					
	nvicInit.NVIC_IRQChannelPreemptionPriority = 1;																
	nvicInit.NVIC_IRQChannelSubPriority = 1;																			
	NVIC_Init(&nvicInit);
}

void write_wireless_message(Wireless_message *m)
{	
	int8_t size = sizeof(Wireless_message);
	
	CC2500_WriteFIFO(&size, FIFO_WRITE_ADDRESS, 1);
	CC2500_CmdStrobe(STX);
	
	CC2500_WriteFIFO((int8_t*) m, FIFO_WRITE_BURST_ADDRESS, sizeof(Wireless_message));
	CC2500_CmdStrobe(STX);
}
