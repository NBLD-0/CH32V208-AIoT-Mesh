#include "Relay.h"

void Relay_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = RELAY_FAN_PIN | RELAY_LIGHT_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RELAY_PORT, &GPIO_InitStruct);

    GPIO_WriteBit(RELAY_PORT, RELAY_FAN_PIN, RESET);
    GPIO_WriteBit(RELAY_PORT, RELAY_LIGHT_PIN, RESET);
}

void Relay_Fan_On(void)
{
    GPIO_WriteBit(RELAY_PORT, RELAY_FAN_PIN, SET);
}

void Relay_Fan_Off(void)
{
    GPIO_WriteBit(RELAY_PORT, RELAY_FAN_PIN, RESET);
}

void Relay_Light_On(void)
{
    GPIO_WriteBit(RELAY_PORT, RELAY_LIGHT_PIN, SET);
}

void Relay_Light_Off(void)
{
    GPIO_WriteBit(RELAY_PORT, RELAY_LIGHT_PIN, RESET);
}

void Relay_All_Off(void)
{
    GPIO_WriteBit(RELAY_PORT, RELAY_FAN_PIN, RESET);
    GPIO_WriteBit(RELAY_PORT, RELAY_LIGHT_PIN, RESET);
}
