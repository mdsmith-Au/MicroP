#include "accelerometer.h"

// Private method to enable/disable interrupts on the accelerometer
void Accelerometer_data_ready(uint8_t state);

// Filter structs for both roll and pitch
FilterStruct rollFilter;
FilterStruct pitchFilter;

void Accelerometer_configure() {

	LIS302DL_InitTypeDef initStruct;
	LIS302DL_InterruptConfigTypeDef interruptStruct;
	LIS302DL_FilterConfigTypeDef filterStruct;
	
	/* Basic configuration */
	// Want to use all axes (enable Data Ready for all)
	initStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
	// Use lower scale for more precision (18 vs 72 mg/digit when reading)
  // We don't plan on subjecting the board to > 2g of acceleration,
  // so more precision is better vs being able to represent values > 2g.
	initStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	// 100Hz requirement for this lab
	initStruct.Output_DataRate = LIS302DL_DATARATE_100;
	// Turn on...
	initStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	// Self test not used
	initStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;

	LIS302DL_Init(&initStruct);
	
	/* (Click) Interrupt configuration */
	// Disable mouse click capability
	interruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE;
	interruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_XYZ_DISABLE;
	// Don't need to latch interrupts
	interruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_NOTLATCHED;
  
	LIS302DL_InterruptConfig(&interruptStruct);
	
	/* Filter Config */
	// @ 100Hz -> filter out anything that moves > 2Hz
	// See LIS302DL datasheet, sec 7.3, Table 22
	filterStruct.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_0;
	// Actually use filter
	filterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
  // ** Or, if it should be disabled during testing, replace with the below **
  //filterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_BYPASSED;
	// Don't need Free-Fall/Wake-Up
	filterStruct.HighPassFilter_Interrupt = LIS302DL_HIGHPASSFILTERINTERRUPT_OFF;
  
	LIS302DL_FilterConfig(&filterStruct);
  
  // Enable data ready interrupts
  Accelerometer_data_ready(1);
  
  // Clear filter
  initFilterBuffer(&rollFilter);
  initFilterBuffer(&pitchFilter);
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


/* Set the data ready bit on the sensor
 * State = 1 : set data ready on INT1
 * State = 0 : disable data ready on INT1
 * This tells the sensor whether or not to send interrupts
 * when it has collected new data.
 * See LIS302DL datasheet, sec 7.4, pg 28
 */
void Accelerometer_data_ready(uint8_t state) {
  
  uint8_t ctrl_reg_value = 0x0;
  
  // Data ready on INT1 = 4
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

/* Equations 8 and 9 in ST Doc ID 17289 Rev. 1
 * Tilt Angle Application Notes
 * Makes use of the three axes to calculate pitch and roll
 * Runs data through an SMA filter before returning it */
int Accelerometer_get_pitch(int x, int y, int z) {
  return filterSMA((int)atan_table( x / sqrt( pow(y,2) + pow(z,2))), &pitchFilter);
}

int Accelerometer_get_roll(int x, int y, int z) {
  return filterSMA((int)atan_table( y / sqrt( pow(x,2) + pow(z,2))), &rollFilter);
}


/* Get acceleration.  */
void Accelerometer_get_data(int* x, int* y, int* z) {
  
  // Get data from sensor
  int buffer[3];
  LIS302DL_ReadACC(buffer);
  
  /* Apply calibration matrix (convert ints to float ahead
   * of time to save on converting to FP from int again and again */
  float raw_x = buffer[0];
  float raw_y = buffer[1];
  float raw_z = buffer[2];
 
  
  float x_temp = ACC11*raw_x+ ACC12*raw_y + ACC13*raw_z;
  float y_temp = ACC21*raw_x+ ACC22*raw_y + ACC23*raw_z;
  float z_temp = ACC31*raw_x+ ACC32*raw_y + ACC33*raw_z;
  
  
  // Apply offset and return.  Float precision no longer needed
  *x = (int)(x_temp + ACC10);
  *y = (int)(y_temp + ACC20);
  *z = (int)(z_temp + ACC30);
}
