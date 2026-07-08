/*
 * key.c - 4x4 matrix keypad
 *
 * Rows, output push-pull:
 *   R1 PB15, R2 PB14, R3 PB13, R4 PB12
 * Columns, input pull-up:
 *   C1 PB7, C2 PB8, C3 PB9, C4 PA8
 */
#include "debug.h"

#define ROW1_PIN GPIO_Pin_15
#define ROW2_PIN GPIO_Pin_14
#define ROW3_PIN GPIO_Pin_13
#define ROW4_PIN GPIO_Pin_12
#define ROW_PINS (ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN)

static void key_set_row(GPIO_TypeDef *port, uint16_t low_pin)
{
    GPIO_SetBits(port, ROW_PINS);
    GPIO_ResetBits(port, low_pin);
    Delay_Us(20);
}

static u8 key_read_columns(u8 k1, u8 k2, u8 k3, u8 k4)
{
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == RESET) return k1;
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == RESET) return k2;
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == RESET) return k3;
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET) return k4;
    return 0;
}

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ROW_PINS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, ROW_PINS);
}

u8 key_read(void)
{
    u8 temp;

    key_set_row(GPIOB, ROW1_PIN);
    temp = key_read_columns(13, 9, 5, 1);
    if(temp) goto key_done;

    key_set_row(GPIOB, ROW2_PIN);
    temp = key_read_columns(14, 10, 6, 2);
    if(temp) goto key_done;

    key_set_row(GPIOB, ROW3_PIN);
    temp = key_read_columns(15, 11, 7, 3);
    if(temp) goto key_done;

    key_set_row(GPIOB, ROW4_PIN);
    temp = key_read_columns(16, 12, 8, 4);

key_done:
    GPIO_SetBits(GPIOB, ROW_PINS);
    return temp;
}
