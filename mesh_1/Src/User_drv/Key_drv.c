#include "Key_drv.h"
#include "Led_drv.h"


uint8_t key_value = 0, key_down = 0, key_up = 0, key_old = 0;

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t Key_ReadPin(void)
{
    uint8_t temp = 0;
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0) {
        temp = 1;
    }
    else {
        temp = 0;
    }
    return temp;
}

void Key_Task(void)
{
    key_value = Key_ReadPin();
    key_down = key_value & (key_value ^ key_old);
    key_up = ~key_value & (key_value ^ key_old);
    key_old = key_value;

    if(key_down)
    {
        printf("123\r\n");
        LED_Turn();
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
