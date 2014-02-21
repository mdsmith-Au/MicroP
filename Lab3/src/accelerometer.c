#include "accelerometer.h"

void Accelerometer_set_data_ready(uint8_t state);

void Accelerometer_configure() {

	LIS302DL_InitTypeDef initStruct;
	LIS302DL_InterruptConfigTypeDef interruptStruct;
	LIS302DL_FilterConfigTypeDef filterStruct;
	
	/* Basic configuration */
	// Want to use all axes (enable Data Ready for all)
	initStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
	// Use lower scale for more precision (18 vs 72 mg/digit when reading)
	initStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	// 100Hz requirement for this lab
	initStruct.Output_DataRate = LIS302DL_DATARATE_100;
	// Turn on...
	initStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	// Self test not used
	initStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	// Send struct for processing
	LIS302DL_Init(&initStruct);
	
	/* Interrupt configuration */
	// Disable mouse click capability
	interruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE;
	interruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_XYZ_DISABLE;
	// Don't need to latch interrupts
	interruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_NOTLATCHED;
	LIS302DL_InterruptConfig(&interruptStruct);
	
	/* Filter Config */
	// @ 100Hz -> filter out anything that moves > 2Hz
	filterStruct.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_0;
	// Actually use filter
	filterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
  // ** Or, if it should be disabled during testing, replace with the below **
  //filterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_BYPASSED;
	// Don't need Free-Fall/Wake-Up
	filterStruct.HighPassFilter_Interrupt = LIS302DL_HIGHPASSFILTERINTERRUPT_OFF;
	LIS302DL_FilterConfig(&filterStruct);
  
  //Enable data ready
  Accelerometer_set_data_ready(1);
}

/* Used once to calibrate the accelerometer in debug mode.
 * Run for 5 - 10 seconds in each position from Table 2
 * in ST Doc ID 17289 Rev 1 manually.  Should be called
 * at a frequency of 100Hz.  Additionally, the high pass
 * filter should be disabled to allow for raw data
 * collection. */
void Accelerometer_calibrate(){
 
    int buffer[3];
    LIS302DL_ReadACC(buffer);
      
    printf("%i,%i,%i\n", buffer[0],buffer[1],buffer[2]);
}


/* Set the data ready bit.
 * State = 1 : set data ready on INT1
 * State = 0 : disable data ready on INT1
 */
void Accelerometer_set_data_ready(uint8_t state) {
  
  uint8_t ctrl_reg_value;
  
  // Data ready on INT1
  if (state == 1) {
    ctrl_reg_value = 0x4;
  }
  // No data ready interrupts
  else {
    ctrl_reg_value = 0x0;
  }
  
  // Write setting to register
  LIS302DL_Write(&ctrl_reg_value, LIS302DL_CTRL_REG3_ADDR, sizeof(uint8_t));

 
}

void Accelerometer_clear_data_ready() {
  
  // Read existing data
  uint8_t status_reg_value;
  LIS302DL_Read(&status_reg_value, LIS302DL_STATUS_REG_ADDR, sizeof(uint8_t));
  
  // Clear last 4 bits (i.e. only the ready ones)
  status_reg_value &= 0xF0;
  LIS302DL_Write(&status_reg_value, LIS302DL_STATUS_REG_ADDR, sizeof(uint8_t));
}


 


