#ifndef __LED_DRV_H
#define __LED_DRV_H
#include "debug.h"
#include "lwns_mesh_example.h"

#define LED1_PIN        GPIO_Pin_12
#define LED1_PORT       GPIOC
#define LED1_RCC_Periph RCC_APB2Periph_GPIOC
#define LED2_PIN        GPIO_Pin_2
#define LED2_PORT       GPIOD
#define LED2_RCC_Periph RCC_APB2Periph_GPIOD

void Led_Init(void);
void LED1_Turn(void);
void LED2_Turn(void);
void GPIO_All_Low_Init(void);
#endif
