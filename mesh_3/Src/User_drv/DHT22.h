#ifndef __DHT22_H
#define __DHT22_H
#include "HAL.h"
#include "lwns_mesh_example.h"
#define MY_DHT22_TASK_EVENT 0x03

#define DHT22_PORT         GPIOB
#define DHT22_PIN          GPIO_Pin_8
#define DHT22_RCC_Periph   RCC_APB2Periph_GPIOB

typedef struct
{
    float temperature;
    float humidity;
}DHT22_Data_t;

void DHT22_Init(void);

uint8_t DHT22_Read(DHT22_Data_t *data);
tmosEvents My_DHT22_Task(tmosTaskID task_id, tmosEvents events);
#endif
