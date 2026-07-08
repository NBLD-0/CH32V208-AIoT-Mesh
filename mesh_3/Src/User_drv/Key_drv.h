#ifndef __KEY_DRV_H
#define __KEY_DRV_H
#include "debug.h"
#include "HAL.h"
#include "lwns_mesh_example.h"
#define MY_TASK_EVENT 0x01

#define KEY1_PIN        GPIO_Pin_4
#define KEY1_PORT       GPIOC
#define KEY1_RCC_Periph RCC_APB2Periph_GPIOC
#define KEY2_PIN        GPIO_Pin_5
#define KEY2_PORT       GPIOC
#define KEY2_RCC_Periph RCC_APB2Periph_GPIOC

void Key_Init(void);
void Key_Task(void);
tmosEvents MyTask(tmosTaskID task_id, tmosEvents events);
#endif
