#include "HC_SR501.h"

void HC_SR501_Init(void)
{
    RCC_APB2PeriphClockCmd(HC_SR_RCC_Periph, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = HC_SR_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC_SR_PORT, &GPIO_InitStruct);
}


void HC_SR501_Task(void)
{
    if(GPIO_ReadInputDataBit(HC_SR_PORT, HC_SR_PIN) == SET)
    {
        PRINT("发现了人\r\n");
        User_TxData[0] = 1;
    }
    else {
        PRINT("未发现了人\r\n");
        User_TxData[0] = 0;
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
