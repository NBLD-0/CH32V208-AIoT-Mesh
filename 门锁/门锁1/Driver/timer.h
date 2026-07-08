#ifndef DRIVER_TIMER_H_
#define DRIVER_TIMER_H_

#include "debug.h"

void TIM4_PWM_Init(void);
void TIM2_PWM_Init(void);
void lock(u8 enable);
void Tim3_Init(u16 arr, u16 psc);

#endif
