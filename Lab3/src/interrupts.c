#include "interrupts.h"

void Interrupts_configure() {
  
  // Enable SYSCFG
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  // Enable interrupt on E0 for MEMS (INT1)
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
  
  // Configure interrupts
  EXTI_InitTypeDef EXTI_InitStruct;
  
  // Pin E0
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  // Use interrupts (as opposed to events)
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  // Trigger interrupt on rising edge -> data ready.  But could also use falling
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  // Enable interrupt
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
  
  // Configure NVIC
  NVIC_InitTypeDef NVIC_InitStruct;
  // Use external interrupt channel 0 -> for pin 0
  NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
  // Priority settings. Give 10/15 .. not the highest, but fair
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 10;
  // Don't need sub priority
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  // Enable it
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  // Clear data ready to allow for future interrupts
  Accelerometer_clear_data_ready();
  EXTI_ClearITPendingBit(EXTI_Line0);
}


extern void EXTI0_IRQHandler(void)
{
  printf("Interrupt ext");
  
  int buffer[3];
  LIS302DL_ReadACC(buffer);
    
  for (int i = 0; i < sizeof(buffer)/sizeof(int); i++) {
      printf("buf[%i] = %i\n", i, buffer[i]);
    }
  
  //Handle the interrupt
  EXTI_ClearITPendingBit(EXTI_Line0);
  
  
  
}
