#ifndef __HC_SR501_H
#define __HC_SR501_H
#include "HAL.h"
#include "lwns_mesh_example.h"

#define MY_HC_SR_TASK_EVENT 0x02
void HC_SR501_Init(void);
tmosEvents My_HC_SR501_Task(tmosTaskID task_id, tmosEvents events);
#endif
