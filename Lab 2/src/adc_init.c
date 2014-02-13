#include "adc_init.h"

void ADC_configure(void) {
	ADC_InitTypeDef InitStruct;                                            /* Init struct for single ADC */
	ADC_CommonInitTypeDef InitStructCommon;                                /* Init struct for multiple ADCs */
	
    /* Enable clock (using ADC1 because it's the only one tied to the sensor) */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	
	InitStruct.ADC_Resolution             = ADC_Resolution_12b;            /* Set max resolution */
	InitStruct.ADC_ScanConvMode           = DISABLE;                       /* Disable multichannel scan conversion */
	InitStruct.ADC_ContinuousConvMode     = DISABLE;                       /* Perform conversion in single mode (i.e. not in background) */
	InitStruct.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None; /* Disable external trigger */
	InitStruct.ADC_DataAlign              = ADC_DataAlign_Right;           /* Align 12 bit data to the right */
	InitStruct.ADC_NbrOfConversion        = 1;                             /* Perform only one conversion */
	
	InitStructCommon.ADC_Mode             = ADC_Mode_Independent;          /* No need to use more than one ADC */
	InitStructCommon.ADC_Prescaler        = ADC_Prescaler_Div2;            /* Keep clock as fast as possible */
	InitStructCommon.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;    /* Don't need to write to memory */
	InitStructCommon.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  /* Keep delay between two consecutive samples at a min */
	
	/* Enable temp sensor */
	ADC_TempSensorVrefintCmd(ENABLE);
	
	/* Pass structs along to init methods */
	ADC_CommonInit(&InitStructCommon);
	ADC_Init(ADC1, &InitStruct);
	
	/* Actually enable ADC */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Use ADC1, Temp sensors channel, rank 1 (rank not relevant with only 1 ADC/channel in use)
       Keep sample time > 10us (actually, 480cycles ~ 32us, but OK) */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_480Cycles);
}
