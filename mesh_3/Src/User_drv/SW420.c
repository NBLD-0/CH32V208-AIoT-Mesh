#include "SW420.h"

void SW420_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(SW420_RCC_Periph, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = SW420_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SW420_PORT, &GPIO_InitStruct);
}

uint8_t SW_ReadPin(void)
{
    uint8_t status = 0;
    if(GPIO_ReadInputDataBit(SW420_PORT, SW420_PIN) == SET)
    {
        status = 1;
    }
    else if(GPIO_ReadInputDataBit(SW420_PORT, SW420_PIN) == RESET){
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
               User_TxData[7] = status_SW;
               PRINT("SW:%d\r\n",status_SW);
           }

        // ���°�����һ�δ�����10ms
        tmos_start_task(task_id, MY_SW_TASK_EVENT, MS1_TO_SYSTEM_TIME(10));

        return events ^ MY_SW_TASK_EVENT; // ����Ѵ����¼�
    }
    return 0;
}
