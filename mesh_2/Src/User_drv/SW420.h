#ifndef __SW420_H
#define __SW420_H
#include "HAL.h"
#include "lwns_mesh_example.h"
#define MY_SW_TASK_EVENT 0x06

#define SW420_PIN        GPIO_Pin_0
#define SW420_PORT       GPIOC
#define SW420_RCC_Periph RCC_APB2Periph_GPIOC

void SW420_Init(void);
tmosEvents My_SW_Task(tmosTaskID task_id, tmosEvents events);
#endif
