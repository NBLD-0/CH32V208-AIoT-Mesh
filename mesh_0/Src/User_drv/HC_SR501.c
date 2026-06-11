#include "HC_SR501.h"

void HC_SR501_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void HC_SR501_Task(void)
{
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == SET)
    {
        PRINT("发现了人\r\n");

    }
    else {
        PRINT("未发现了人\r\n");
    }
}

tmosEvents My_HC_SR501_Task(tmosTaskID task_id, tmosEvents events)
{
    if(events & MY_HC_SR_TASK_EVENT)
    {
        // 执行你的任务逻辑
        HC_SR501_Task();

        // 重新安排下一次触发：10ms
        tmos_start_task(task_id, MY_HC_SR_TASK_EVENT, MS1_TO_SYSTEM_TIME(200));

        return events ^ MY_HC_SR_TASK_EVENT; // 清除已处理事件
    }
    return 0;
}
