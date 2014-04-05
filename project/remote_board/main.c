#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "stdlib.h"

#include "LCD_driver.h"
#include "mems_controller.h"
#include "atan_LUT.h"
#include "arm_math.h"
#include "filter.h"
#include "wireless_cc2500.h"
#include "keypad_driver.h"
#include "interrupts_config.h"


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

void turnOnGreenLED(void);
void turnOffGreenLED(void);
void LED_GPIO_config(void);

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
    LED_GPIO_config();
	Interrupts_configure();
	LCD_configure();
		
	//init semaphores
	osSemaphoreCreate(osSemaphore(displaySemaphore), 1);
	osSemaphoreCreate(osSemaphore(modeSemaphore), 1);
	
	//init message box and mem pool
	wireless_pool = osPoolCreate(osPool(wireless_pool));                 // create memory pool
    wireless_message_box = osMessageCreate(osMessageQ(wireless_message_box), NULL);  // create msg queue
	
	//start threads
	tid_orientation = osThreadCreate(osThread(orientation_thread), NULL);
	tid_wireless = osThreadCreate(osThread(wireless_thread), NULL);
    tid_keypad = osThreadCreate(osThread(keypad_thread), NULL);
	
	Keypad_configure();
    
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

//Orientation thread: responsible for accelerometer polling
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

//Wireless thread: responsible for transmitting messages to other board.
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
		if (numBytesFIFOBuffer + sizeof(Wireless_message) + 10 <= FIFO_SIZE)
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

//Keypad thread: responsible for keypad input
void keypad_thread(const void* arg) {
    enableCursor();
    clearLCD();    
    int samplingMode = 0;
    Wireless_message wireless[32];
    
    char prevKeypress = 0;
    
    int counter = 0;
    int pitch = 0;
    int roll = 0;
    int time = 0;
    char keypadEntry[24];
    while(1)
        {   
            osSignalWait(KEYPAD_FLAG, osWaitForever);
            
          	osSemaphoreWait(modeSemaphore, osWaitForever);
            samplingMode = (mode == KEYPAD_MODE)? 0: 1;
            osSemaphoreRelease(modeSemaphore);

            if (!samplingMode) { 
                turnOnGreenLED();
                uint32_t data= Keypad_poll();
                char currKeypress = Keypad_Get_Character(data);
                
                if (currKeypress != prevKeypress) {
                    prevKeypress = currKeypress;
                    
                    if (currKeypress != 0) {
                        
                        keypadEntry[counter] = currKeypress;
                        counter++;
                        printLCDCharKeypad(currKeypress);
                        if (currKeypress == 'A') {
                            counter = 0;
                            // Parse string
                            if (keypadEntry[0] == '*') {
                                keypadEntry[0] = '-';
                            }
                            else {
                                keypadEntry[0] = ' ';
                            }
                            char rollString[3];
                            memcpy(rollString, keypadEntry, 3);
                            roll = atoi(rollString);
                            
                            if (keypadEntry[4] == '*') {
                                keypadEntry[4] = '-';
                            }
                            else {
                                keypadEntry[4] = ' ';
                            }
                            
                            char pitchString[3];
                            memcpy(pitchString, keypadEntry + sizeof(char)*4, 3);
                            pitch = atoi(pitchString);
                            
                            char timeString[2];
                            memcpy(timeString, keypadEntry + sizeof(char)*8, 2);
                            time = atoi(timeString);
                            
                            clearLCD();
                            resetLCDPosition();
                            
                            Wireless_message *wireless_m;
                            wireless_m = osPoolAlloc(wireless_pool);                     // Allocate memory for the message
                            wireless_m->rollAngle = roll;
                            wireless_m->pitchAngle = pitch;
                            wireless_m->delta_t = time;
                            wireless_m->realtime = 0;
                            osMessagePut(wireless_message_box, (uint32_t)wireless_m, osWaitForever);  // Send Message
                        }
                    }
                }
        }
        else {
            turnOffGreenLED();
        }
    }
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

void TIM3_IRQHandler() {
    osSignalSet(tid_keypad, KEYPAD_FLAG);
    
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

//write wireless message to wireless queue
void write_wireless_message(Wireless_message *m)
{	
	int8_t size = sizeof(Wireless_message);
	
	CC2500_WriteFIFO(&size, FIFO_WRITE_ADDRESS, 1);
	//CC2500_CmdStrobe(STX);
	
	CC2500_WriteFIFO((int8_t*) m, FIFO_WRITE_BURST_ADDRESS, sizeof(Wireless_message));
	//CC2500_CmdStrobe(STX);
}

void LED_GPIO_config() {
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Drive green LED (Pin 12), normal output. For debug use. */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/* Turns on the green LED light. For testing purposes only. */
void turnOnGreenLED() {
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

void turnOffGreenLED() {
    GPIO_ResetBits(GPIOD, GPIO_Pin_12);
}

