#include "interrupts_config.h"

// Private methods

// Set up (external) interrupt for accelerometer
void mems_interrupt_config(void);
// Set up TIM3 interrupt
void tim3_interrupt_config(void);

// Global variable used to define whether or not NVIC prio. group has already been set
uint8_t NVIC_PRIORITY_SET = 0;

/* Configure interrupts to work with both TIM3 and MEMS sensor.
 * Note that the accelerometer must have been already set up. */
void Interrupts_configure() {
  
  mems_interrupt_config();
  tim3_interrupt_config();
}


void mems_interrupt_config() {
  
  // Enable SYSCFG, GPIOE
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  // Enable interrupt on E1 for MEMS (INT2)
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
  
  // Configure interrupts
  EXTI_InitTypeDef EXTI_InitStruct;
  
  // Pin E1
  EXTI_InitStruct.EXTI_Line = EXTI_Line1;
  // Use interrupts (as opposed to events)
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  // Trigger interrupt on rising edge -> data ready.
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  // Enable interrupt
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
  
  // Configure NVIC
  set_nvic_priority();
  NVIC_InitTypeDef NVIC_InitStruct;
  // Use external interrupt channel 1 -> for pin 1
  NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
  // Priority settings. Give 4 .. not the highest, but fair
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;   // arbitrary
  // Don't need sub priority
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  // Enable it
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  /* Execute a read (via interrupt) to get interrupts going.
   * Otherwise, it is already high, thus no interrupt is
   * generated on the rising edge. Reads clear the bit holding
   * the line high.   */
  EXTI_GenerateSWInterrupt(EXTI_Line1);
}

void tim3_interrupt_config() {
  
/* APB1 Max 142 Mhz
 * We know
 * TIM3CLK = 2 * PCLK1  (PCLK1 = APB1 clock -> clock of bus TIM3 is on)
 * but  PCLK1 = HCLK / 4
 * thus TIM3CLK = HCLK / 2 = SystemCoreClock /2
 * There is also the counter clock (CC), output clock and prescaler
 
 * Prescaler = (TIM3CLK / TIM3 counter clock) - 1
 * Prescaler = ((SystemCoreClock/2) / TIM3 counter clock) - 1

 * Output clock period = (TIM3 counter clock / TIM3 output clock) - 1
 
 * We chose 1Mhz counter clock, 25Hz output clock (desired) -> period = 1MHz/25Hz - 1 = 39999
 * Also keeps prescaler within unsigned 16 bit prescaler limit (65536)
 * 
 * For details about the equations, see Doc ID 018909 Rev 6 and Doc ID 022152 Rev 4
 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	// Enable clock to TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	// Prescaler set dynamically based on Clock Freq, useful if we change it later
  // According to Doc 018909 rev 6 (p. 627), sec 18.4.11:
  // Counter clock frequency CK_CNT = fCK_PSC / (PSC[15:0] + 1).
  // Hence, we must subtract 1 so that our prescaler value is simply fCK_PSC / PSC[15:0]
	uint16_t PrescalerValue                   = (uint16_t)((SystemCoreClock/2)/1000000) - 1;
	
	TIM_TimeBaseInitStruct.TIM_Period         = PERIOD;
  TIM_TimeBaseInitStruct.TIM_Prescaler      = PrescalerValue;
  
	/* No need to further divide the clock in our case */   
  TIM_TimeBaseInitStruct.TIM_ClockDivision  = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode    = TIM_CounterMode_Up;
	/* Note: TIM_RepetitionCounter does not apply to TIM3 */
	
	/* Send struct to be processed */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	/* Enable the period preload just in case */
	TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  // NVIC config
  set_nvic_priority();
  NVIC_InitTypeDef NVIC_InitStruct;
  // Use TIM3 interrupt
  NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
  // Priority settings. Give 5 .. below accelerometer
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
  // Don't really need sub priority
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  // Enable it
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  // Now tell the timer to enable interrupts
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  
	/* Enable counter -> start */
	TIM_Cmd(TIM3, ENABLE);
}

/* NVIC priority bits should be set before using NVIC, but we don't want to set
 * them manually if different parts of the code use the NVIC, since we could have one
 * piece of code setting it to one value and another setting it to another value.
 * Hence, this method which sets it once and only once. */
void set_nvic_priority() {

  if (!NVIC_PRIORITY_SET) {
    // Priority group: 3 bits for pre-emption priority, 1 bit for subpriority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); // chosen arbitrarily
    NVIC_PRIORITY_SET = 1;
  }
}
