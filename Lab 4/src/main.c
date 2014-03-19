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

// Signal values
#define TEMP_INT_SIGNAL 0x01
#define ACC_INT_SIGNAL 0x02
#define BUTTON_INT_SIGNAL 0x04

/* Thread function prototypes */
void temperature_thread(const void* arg);
void accelerometer_thread(const void* arg);
void switch_display_thread(const void* arg);

// Temperature filter buffer
FilterStruct tempFilterBuffer;


/* Thread structure for above threads */
osThreadDef(temperature_thread, osPriorityHigh, 1, 0);
osThreadDef(accelerometer_thread, osPriorityHigh, 1, 0);
osThreadDef(switch_display_thread, osPriorityNormal, 1, 0);

osThreadId tid_temperature, tid_accelerometer, tid_switch_display;

// Display mode switching
typedef enum {
  TEMP_MODE = 1, 
  ACCEL_MODE = 0
} DISPLAY_MODE;

// Mode the board is in; see above
DISPLAY_MODE mode = TEMP_MODE;

// Mutex for accessing the Display Mode
osMutexDef (Mutex_Mode);
osMutexId Mutex_Mode_id;

/**
 * Program entry point.
 */
int main() {
    
    // Configuration of all devices/parameters
    ADC_configure();
    calibrateTempSensor();
    initFilterBuffer(&tempFilterBuffer); 

    Alarm_configure();
    Motor_PWM_configure();
    Accelerometer_configure();

    // Configure LCD, but wait for it to power up in case we just plugged in
    osDelay(200);
    LCD_configure();

    // Create the mode mutex
    Mutex_Mode_id = osMutexCreate(osMutex(Mutex_Mode));
  
    // Create + start all threads
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

// Thread responsible for switching modes and taking care of necessary LCD printing
void switch_display_thread(const void* arg) {
    while(1) {
        // Wait for the button to be pushed
        osSignalWait(BUTTON_INT_SIGNAL, osWaitForever);
        
        // Switch mode; use mutex to prevent other threads from
        // printing to screen when they shouldn't if we change mode 
        // and clear the screen halfway through their thread
        osMutexWait(Mutex_Mode_id, osWaitForever);
        if (mode == TEMP_MODE) {
            
            mode = ACCEL_MODE;
        }
        else {
            mode = TEMP_MODE;
        }
        
        clearLCD();
        
        // Print appropriate string to LCD based on mode
        if (mode == TEMP_MODE) {
            printLCDString("Temperature:    C", 1);
        }
        else {           
          printLCDString("Pitch:", 1);
          printLCDString("Roll:", 2); 
        }
        osMutexRelease(Mutex_Mode_id);
        
        // Wait a bit then re-enable the button; prvents multiple switches
        // from one button press
        osDelay(300);
        enable_button_interrupt();
    }
}


// Thread responsible for handling temperature acquisition and printing to LCD
void temperature_thread(const void* arg) {
    while(1) {
        // Wait until timer says its OK (25hz)
        osSignalWait(TEMP_INT_SIGNAL, osWaitForever);
        
        float temperature = getAndAverageTemp(&tempFilterBuffer);
        
        // Send temp to alarm to be checked
        alarmCheckTemp(temperature);
        
        // Print to LCD, but skip if we can't get the LCD in time for the next sample
        // 33 msec to be safe (sample every 40ms, 2*33 < 2*40 in worst case of 2x timeout)
        osStatus event = osMutexWait(Mutex_Mode_id, 33);
        if (event != osErrorTimeoutResource) {
            if (mode == TEMP_MODE) {
                char tempAsString[4];
                sprintf(tempAsString, "%.1f", temperature);
                printLCDToPos(tempAsString, 1, 13);
            }
            osMutexRelease(Mutex_Mode_id);
        }
    }
}


// Thread responsible for getting roll/pitch and printing to LCD
void accelerometer_thread(const void* arg) {
    while(1) {
        // Wait forever for accelerometer interrupt
        osSignalWait(ACC_INT_SIGNAL, osWaitForever);
            
        int x, y, z;
        Accelerometer_get_data(&x, &y, &z);
            
        int roll = Accelerometer_get_roll(x, y, z);
        int pitch = Accelerometer_get_pitch(x, y, z);
        
        // Print to LCD if in correct mode
        // Note: timout of 9ms ensures we skip and go wait for another sample if we can't get the LCD in time
        // as the LCD takes ~8msec to print two strings
        // Worst case: timeout twice in a row = 18msec < 20msec for 2 samples, so we get both
        // If we timeout once/run once, 8+9 is also < 20msec
        osStatus event = osMutexWait(Mutex_Mode_id, 9);
        if (event != osErrorTimeoutResource) {
            if (mode == ACCEL_MODE) {
                // Convert to string
                char rollAsString[3];
                char pitchAsString[3];
                // 3 specifies minimum width, since we leave a fixed space for it on the display
                sprintf(rollAsString, "%3i", roll);
                sprintf(pitchAsString, "%3i", pitch);
            
                // Move motor to correct angle
                motor_move_to_angle(roll);
            
                printLCDToPos(pitchAsString, 1, 7);
                printLCDToPos(rollAsString, 2, 6);
            }
            else {
                // Move to 0 when not in acceleration mode
                motor_move_to_angle(0);
            }
        
            osMutexRelease(Mutex_Mode_id);
        }
    }
}

// External interrupt on line 1 : INT2 on acclerometer
void EXTI1_IRQHandler() {
    // Note that the interrupt on the accelerometer is cleared 
    // if, and ONLY if, data is read from it.  The status
    // registers that hold the interrupt line high cannot be modified manually
    osSignalSet(tid_accelerometer, ACC_INT_SIGNAL);
    
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
