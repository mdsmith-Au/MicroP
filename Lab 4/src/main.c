//#include "arm_math.h"

// System includes
#include <stdio.h>
#include "cmsis_os.h"
#include "stm32f4xx.h"

// Project includes
#include "accelerometer.h"
#include "adc_init.h"
#include "alarm.h"
#include "interrupts_config.h"
#include "motor.h"
#include "temp_processing.h"
#include "LCD_Driver.h"

#define TEMP_INT_SIGNAL 0x01
#define ACC_INT_SIGNAL 0x02
#define BUTTON_INT_SIGNAL 0x04

/* Thread function prototypes */
void temperature_thread(const void* arg);
void accelerometer_thread(const void* arg);
void switch_display_thread(const void* arg);

FilterStruct tempFilterBuffer;

osMutexDef (Mutex_Mode);
osMutexId Mutex_Mode_id;

/* Thread structure for above threads */
osThreadDef(temperature_thread, osPriorityNormal, 1, 0);
osThreadDef(accelerometer_thread, osPriorityNormal, 1, 0);
osThreadDef(switch_display_thread, osPriorityNormal, 1, 0);

osThreadId tid_temperature, tid_accelerometer, tid_switch_display;

// Display mode switching
typedef enum {
  TEMP_MODE = 1, 
  ACCEL_MODE = 0
} DISPLAY_MODE;

static volatile DISPLAY_MODE mode = TEMP_MODE;

/**
 * Program entry point.
 */
int main() {
    
    ADC_configure();
    calibrateTempSensor();
    initFilterBuffer(&tempFilterBuffer); 

    Alarm_configure();
    Motor_PWM_configure();
    Accelerometer_configure();

    LCD_configure();

    Mutex_Mode_id = osMutexCreate(osMutex(Mutex_Mode));
  
    tid_temperature = osThreadCreate(osThread(temperature_thread), NULL);
    tid_accelerometer = osThreadCreate(osThread(accelerometer_thread), NULL);
    tid_switch_display = osThreadCreate(osThread(switch_display_thread), NULL);

    // Configure interrupts last so as to avoid a deadlock situation with
    // the signaling for the temperature
    Interrupts_configure();

    // Run the switch display thread once to get a complete string on screen
    // (this will be the opposite of the default set initially as a global variable for mode)
    EXTI_GenerateSWInterrupt(EXTI_Line0);
}

void switch_display_thread(const void* arg) {
    while(1) {
        osSignalWait(BUTTON_INT_SIGNAL, osWaitForever);
        
        // Switch mode
        if (mode == TEMP_MODE) {
            //osMutexWait(Mutex_Mode_id, osWaitForever);
            mode = ACCEL_MODE;
            //osMutexRelease(Mutex_Mode_id);
        }
        else {
            //osMutexWait(Mutex_Mode_id, osWaitForever);
            mode = TEMP_MODE;
            //osMutexRelease(Mutex_Mode_id);
        }
            
            
        clearLCD();
        //osMutexWait(Mutex_Mode_id, osWaitForever);
        if (mode == TEMP_MODE) {
            printLCDString("Temperature:    C", 1);
        }
        else {           
          printLCDString("Pitch:", 1);
          printLCDString("Roll:", 2); 
        }
        // Wait a bit then re-enable the button
        osDelay(300);
        enable_button_interrupt();
        //osMutexRelease(Mutex_Mode_id);
    }
}


void temperature_thread(const void* arg) {
    while(1) {
        
        osSignalWait(TEMP_INT_SIGNAL, osWaitForever);
        
        float temperature = getAndAverageTemp(&tempFilterBuffer);
        
        char tempAsString[4];
        sprintf(tempAsString, "%.1f", temperature);

        alarmCheckTemp(temperature);
        
        //osMutexWait(Mutex_Mode_id, osWaitForever);
        if (mode == TEMP_MODE) {
           printLCDToPos(tempAsString, 1, 13);
        }
        //osMutexRelease(Mutex_Mode_id);
    }
}


void accelerometer_thread(const void* arg) {
    while(1) {
        // Wait forever for accelerometer interrupt
        osSignalWait(ACC_INT_SIGNAL, osWaitForever);
            
        int x, y, z;
        Accelerometer_get_data(&x, &y, &z);
            
        int roll = Accelerometer_get_roll(x, y, z);
        int pitch = Accelerometer_get_pitch(x, y, z);
        
        //osMutexWait(Mutex_Mode_id, osWaitForever);
        if (mode == ACCEL_MODE) {
            char rollAsString[3];
            char pitchAsString[3];
            // 3 specifies minimum width, since we leave a fixed space for it on the display
            sprintf(rollAsString, "%3i", roll);
            sprintf(pitchAsString, "%3i", pitch);

            motor_move_to_angle(roll);
            printLCDToPos(pitchAsString, 1, 7);
            printLCDToPos(rollAsString, 2, 6);
        }
        else {
            // Move to 0 when not in acceleration mode
            motor_move_to_angle(0);
        }
        //osMutexRelease(Mutex_Mode_id);
    }
}

// External interrupt on line 1 : INT2 on acclerometer
void EXTI1_IRQHandler() {
    osSignalSet(tid_accelerometer, ACC_INT_SIGNAL);
    
    // Note that the interrupt on the accelerometer is cleared 
    // if, and ONLY if, data is read from it.  The status
    // registers that hold the interrupt line high cannot be modified manually
    EXTI_ClearITPendingBit(EXTI_Line1);
}

// Timer 3 interrupt for temperature)
void TIM3_IRQHandler() {
    osSignalSet(tid_temperature, TEMP_INT_SIGNAL);
    
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

// User Button interrupt handler
void EXTI0_IRQHandler() {
  
    // Disable interrupt to prevent multiple interrupts from one button press
    disable_button_interrupt();
  
    osSignalSet(tid_switch_display, BUTTON_INT_SIGNAL);
    
    EXTI_ClearITPendingBit(EXTI_Line0);
}
