#include "accelerometer.h"


void Accelerometer_configure() {

	LIS302DL_InitTypeDef initStruct;
	LIS302DL_InterruptConfigTypeDef interruptStruct;
	LIS302DL_FilterConfigTypeDef filterStruct;
	
	/* Basic configuration */
	// Want to use all axes
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
	
	filterStruct.HighPassFilter_Interrupt
	
}



