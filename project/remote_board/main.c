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

#define ANGLE_FILTER_DEPTH 16	/*!< Filter depth for angle filters */
#define GRAV_ACC 1000.0f	/*!< Value of g */
#define NINETY_DEG_THRESH 1000	/*!< Threshold for 90 degree readings */
#define SENSOR_X_OFFSET 18	/*!< X offset from calibration */
#define SENSOR_Y_OFFSET 18	/*!< Y offset from calibration */
#define SENSOR_Z_OFFSET -64	/*!< Z offset from calibration (expected 1000)*/

#define ACCELERATON_FLAG	0x01	/*!< Acceleration signaling flag */

static int displayValue = 0;

// mode switching
typedef enum {
  KEYPAD_MODE = 1, 
  REALTIME_MODE = 0
} OPERATION_MODE;

// Mode the board is in; see above
OPERATION_MODE mode = REALTIME_MODE;

typedef struct {                               
	int rollAngle;
	int pitchAngle;
	int delta_t;						// 
	int realtime;						// is it realtime mode or not
} Wireless_message;

static Filter rollFilter;
static Filter pitchFilter;
static Queue rollBuffer;
static Queue pitchBuffer;

osPoolDef(wireless_pool, WIRELESS_MESSAGE_QUEUE_SIZE, Wireless_message);                    // Define memory pool queue size 16 for now
osPoolId wireless_pool;
osMessageQDef(wireless_message_box, WIRELESS_MESSAGE_QUEUE_SIZE, Wireless_message);   //queue size 16 arbitrarily for now (queue should theoretically never get full)
osMessageQId wireless_message_box;

void orientation_thread(const void* arg);
void wireless_thread(const void* arg);

osThreadDef(orientation_thread, osPriorityNormal, 1, 0);
osThreadDef(wireless_thread, osPriorityNormal, 1, 0);

osThreadId tid_orientation, tid_wireless;

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
	/*
	CC2500_Init();
	
	uint8_t buffer[] = {0, 0, 0, 0};
	CC2500_SPI_Cmd_Strobe(SRES);
	CC2500_SPI_Cmd_Strobe(SIDLE);
	
	CC2500_Read_Reg(buffer, PARTNUM, 1);
	printf("Buff: %x\n", buffer[0]);
	CC2500_Read_Reg(buffer, VERSION, 1);
	printf("Buff: %x\n", buffer[0]);
	*/
	//buffer[0] = 0xF;
	//CC2500_Write_Reg(buffer, FREQ0_WRITE_SINGLE, 1);
	//buffer[0] = 0;
	//CC2500_Read_Reg(buffer, FREQ0_READ_SINGLE, 1);
	//printf("Buff: %x\n", buffer[0]);
	/*
	buffer[0] = 0xA;
	buffer[1] = 0xB;
	buffer[2] = 0xC;
	buffer[3] = 0xD;
	CC2500_Write_Reg(buffer, FREQ1_WRITE_BURST, 4);
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	CC2500_Read_Reg(buffer, FREQ1_READ_BURST, 4);
	printf("Buff: %x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3]);*/
	
	Keypad_configure();
	
	
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(1000);
		printLCDString("Hello World!",1);
		osDelay(1000);
		printLCDString("LCD test in progress.",2);
		osDelay(1000);
		clearLCD();
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
			
			wireless_m = osPoolAlloc(wireless_pool);                     // Allocate memory for the message
			wireless_m->rollAngle = 0;
			wireless_m->pitchAngle = 0;
			wireless_m->delta_t = 0;
			wireless_m->realtime = 0;
			osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
		}
	}
}

void wireless_thread(const void* arg)
{
	//init wireless
	
	Wireless_message  *wireless_m;
  osEvent event;
	
	while(1)
	{
		event = osMessageGet(wireless_message_box, osWaitForever);  // wait for message
		
    if (event.status == osEventMessage)
		{
      wireless_m = event.value.p;
			
			//send wireless message
			
      osPoolFree(wireless_pool, wireless_m);                  // free memory allocated for message
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
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/* Keypad Interrupt Handlers */
void EXTI2_IRQHandler() {
	printf("EXTI2 Handler\n");
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler() {
	printf("EXTI3 Handler\n");
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI9_5_IRQHandler() {
	if (EXTI_GetFlagStatus(EXTI_Line6)) {
		printf("EXTI6 Handler\n");
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	else if (EXTI_GetFlagStatus(EXTI_Line7)) {
		printf("EXTI7 Handler\n");
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	
}
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
