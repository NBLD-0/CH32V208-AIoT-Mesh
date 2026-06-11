#include "Led_drv.h"

void Led_Init(void)
{
    RCC_APB2PeriphClockCmd(LED1_RCC_Periph, ENABLE);
    RCC_APB2PeriphClockCmd(LED2_RCC_Periph, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = LED1_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = LED2_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED2_PORT, &GPIO_InitStruct);

    GPIO_WriteBit(LED1_PORT, LED1_PIN, RESET);
    GPIO_WriteBit(LED2_PORT, LED2_PIN, RESET);
}


void LED1_Turn(void)
{
    if(GPIO_ReadOutputDataBit(LED1_PORT, LED1_PIN) == 0)
    {
        GPIO_WriteBit(LED1_PORT, LED1_PIN, 1);
    }
    else {
        GPIO_WriteBit(LED1_PORT, LED1_PIN, 0);
    }
}

void LED2_Turn(void)
{
    if(GPIO_ReadOutputDataBit(LED2_PORT, LED2_PIN) == 0)
    {
        GPIO_WriteBit(LED2_PORT, LED2_PIN, 1);
    }
    else {
        GPIO_WriteBit(LED2_PORT, LED2_PIN, 0);
    }
}
