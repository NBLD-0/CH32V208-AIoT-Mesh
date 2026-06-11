#ifndef __SW420_H
#define __SW420_H
#include "HAL.h"
#define MY_SW_TASK_EVENT 0x06

void SW420_Init(void);
tmosEvents My_SW_Task(tmosTaskID task_id, tmosEvents events);
#endif
