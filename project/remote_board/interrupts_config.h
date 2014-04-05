#ifndef _INTERRUPTS_CONFIG_H
#define _INTERRUPTS_CONFIG_H

// External interrupts
#include "stm32f4xx_exti.h"
// Syscfg, RCC for enabling required hardware
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
// NVIC for interrupt processing
#include "misc.h"
// Timer for interrupt generation at specified interval
#include "stm32f4xx_tim.h"

// TIM3 Period.  See comments in .c file
#define PERIOD 39999

void Interrupts_configure(void);

// Set NVIC priority group, but only if not already set
void set_nvic_priority(void);

void disable_button_interrupt(void);
void enable_button_interrupt(void);
#endif
