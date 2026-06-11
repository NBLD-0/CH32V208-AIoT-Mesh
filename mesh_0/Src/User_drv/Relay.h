#ifndef __RELAY_H
#define __RELAY_H
#include "debug.h"

#define RELAY_FAN_PIN    GPIO_Pin_12
#define RELAY_LIGHT_PIN  GPIO_Pin_13
#define RELAY_PORT       GPIOB

void Relay_Init(void);
void Relay_Fan_On(void);
void Relay_Fan_Off(void);
void Relay_Light_On(void);
void Relay_Light_Off(void);
void Relay_All_Off(void);

#endif
