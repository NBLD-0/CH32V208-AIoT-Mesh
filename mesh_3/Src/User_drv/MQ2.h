#ifndef __MQ2_H
#define __MQ2_H
#include "HAL.h"
#include "lwns_mesh_example.h"
#define MY_MQ2_ADC_TASK_EVENT 0x05

#define MQ2_ADC              ADC1
#define MQ2_ADC_RCC_APB      RCC_APB2Periph_ADC1
#define MQ2_ADC_PORT         GPIOA
#define MQ2_ADC_GPIO_RCC_APB RCC_APB2Periph_GPIOA
#define MQ2_ADC_PIN          GPIO_Pin_1

void MQ2_ADC_DMA_Init(void);
tmosEvents My_MQ2_Task(tmosTaskID task_id, tmosEvents events);

#endif
