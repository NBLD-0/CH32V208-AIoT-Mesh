#ifndef __KEY_DRV_H
#define __KEY_DRV_H
#include "debug.h"
#include "HAL.h"

#define MY_TASK_EVENT 0x01

void Key_Init(void);
void Key_Task(void);
tmosEvents MyTask(tmosTaskID task_id, tmosEvents events);
#endif
