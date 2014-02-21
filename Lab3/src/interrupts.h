#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
// NVIC 
#include "misc.h"

void Interrupts_configure(void);
#endif
