#include "Led_drv.h"

void Led_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, RESET);
}


void LED_Turn(void)
{
    if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_6) == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_6, 1);
    }
    else {
        GPIO_WriteBit(GPIOA, GPIO_Pin_6, 0);
    }
}
