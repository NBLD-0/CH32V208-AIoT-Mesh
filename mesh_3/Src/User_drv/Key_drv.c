#include "Key_drv.h"
#include "Led_drv.h"


uint8_t key_value = 0, key_down = 0, key_up = 0, key_old = 0;

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(KEY1_RCC_Periph, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = KEY1_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY1_PORT, &GPIO_InitStruct);

    RCC_APB2PeriphClockCmd(KEY2_RCC_Periph, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = KEY2_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY2_PORT, &GPIO_InitStruct);
}

uint8_t Key_ReadPin(void)
{
    uint8_t temp = 0;
    if(GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN) == 0) {
        temp = 1;
    }
    if(GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN) == 0) {
        temp = 2;
    }
    return temp;
}

void Key_Task(void)
{
    key_value = Key_ReadPin();
    key_down = key_value & (key_value ^ key_old);
    key_up = ~key_value & (key_value ^ key_old);
    key_old = key_value;

    switch(key_down)
    {
        case 1:
            LED1_Turn();
            printf("1\r\n");
            break;
        case 2:
            LED2_Turn();
            printf("2\r\n");
            break;
    }
}


tmosEvents MyTask(tmosTaskID task_id, tmosEvents events)
{
    if(events & MY_TASK_EVENT)
    {
        // 执行你的任务逻辑
        Key_Task();

        // 重新安排下一次触发：10ms
        tmos_start_task(task_id, MY_TASK_EVENT, MS1_TO_SYSTEM_TIME(10));

        return events ^ MY_TASK_EVENT; // 清除已处理事件
    }
    return 0;
}
