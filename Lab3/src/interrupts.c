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
  // TODO: What are events?
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  // Trigger interrupt on rising edge -> data ready.  But could also use falling
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  // Enable interrupt
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
  
  // Configure NVIC
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

