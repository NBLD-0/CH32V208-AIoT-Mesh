#include "SW420.h"

void SW420_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

uint8_t SW_ReadPin(void)
{
    uint8_t status = 0;
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == SET)
    {
        status = 1;
    }
    else if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == RESET){
        status = 0;
    }
    return status;
}

tmosEvents My_SW_Task(tmosTaskID task_id, tmosEvents events)
{
    if(events & MY_SW_TASK_EVENT)
    {
       uint8_t status_SW = 0;
       static uint8_t PRINT_CNT = 0;
       PRINT_CNT++;
       status_SW = SW_ReadPin();
           if(PRINT_CNT >= 50)
           {
               PRINT_CNT = 0;
               PRINT("SW:%d\r\n",status_SW);
           }

        // 重新安排下一次触发：10ms
        tmos_start_task(task_id, MY_SW_TASK_EVENT, MS1_TO_SYSTEM_TIME(10));

        return events ^ MY_SW_TASK_EVENT; // 清除已处理事件
    }
    return 0;
}
