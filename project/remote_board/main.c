#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "LCD_driver.h"

#define WIRELESS_MESSAGE_QUEUE_SIZE 16

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
	int delta_t;
	int realtime;	//is it realtime mode or not
} Wireless_message;

osPoolDef(wireless_pool, WIRELESS_MESSAGE_QUEUE_SIZE, Wireless_message);                    // Define memory pool queue size 16 for now
osPoolId wireless_pool;
osMessageQDef(wireless_message_box, WIRELESS_MESSAGE_QUEUE_SIZE, Wireless_message);   //queue size 16 arbitrarily for now (queue should theoretically never get full)
osMessageQId wireless_message_box;

void orientation_thread(const void* arg);
void wireless_thread(const void* arg);

osThreadDef(orientation_thread, osPriorityNormal, 1, 0);
osThreadDef(wireless_thread, osPriorityNormal, 1, 0);

osThreadId tid_orientation, tid_wireless;

/*!
 @brief Program entry point
 */
int main (void) {
	wireless_pool = osPoolCreate(osPool(wireless_pool));                 // create memory pool
  wireless_message_box = osMessageCreate(osMessageQ(wireless_message_box), NULL);  // create msg queue
	
	tid_orientation = osThreadCreate(osThread(orientation_thread), NULL);
	tid_wireless = osThreadCreate(osThread(wireless_thread), NULL);
	
	osDelay(2000);
	LCD_configure();
	osDelay(5);
	printLCDString("Kevin did it!",1);
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

void orientation_thread(const void* arg)
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
