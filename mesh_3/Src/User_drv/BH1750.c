#include "BH1750.h"



void BH1750_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB1PeriphClockCmd(I2C_RCC_APB, ENABLE);
    RCC_APB2PeriphClockCmd(I2C_GPIO_RCC_APB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    I2C_InitTypeDef I2C_InitStruct = {0};

    GPIO_InitStruct.GPIO_Pin = I2C_SDA | I2C_SCL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;

    GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStruct);

    I2C_DeInit(I2C_PORT);
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_Ack = ENABLE;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_Init(I2C_PORT, &I2C_InitStruct);

    I2C_Cmd(I2C_PORT, ENABLE);

}


static uint8_t I2C_WaitFlag(I2C_TypeDef *I2Cx,
                            uint32_t flag,
                            FlagStatus status,
                            uint32_t timeout)
{
    while(I2C_GetFlagStatus(I2Cx, flag) != status)
    {
        if(timeout-- == 0)
        {
            return 1;
        }
    }

    return 0;
}

static uint8_t I2C_WaitEvent(I2C_TypeDef *I2Cx,
                             uint32_t event,
                             uint32_t timeout)
{
    while(!I2C_CheckEvent(I2Cx, event))
    {
        if(timeout-- == 0)
        {
            return 1;
        }
    }

    return 0;
}

static void I2C_ResetBus(void)
{
    I2C_SoftwareResetCmd(I2C_PORT, ENABLE);
    Delay_Us(10);
    I2C_SoftwareResetCmd(I2C_PORT, DISABLE);

    I2C_Cmd(I2C_PORT, ENABLE);

    I2C_AcknowledgeConfig(I2C_PORT, ENABLE);
}

static int BH1750_WriteCmd(uint8_t cmd)
{
    if(I2C_WaitFlag(I2C_PORT,
                    I2C_FLAG_BUSY,
                    RESET,
                    100000))
        return -1;

    I2C_GenerateSTART(I2C_PORT, ENABLE);

    if(I2C_WaitEvent(I2C_PORT,
                     I2C_EVENT_MASTER_MODE_SELECT,
                     100000))
        return -2;

    I2C_Send7bitAddress(I2C_PORT,
                        ADDR << 1,
                        I2C_Direction_Transmitter);

    if(I2C_WaitEvent(I2C_PORT,
                     I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
                     100000))
        return -3;

    I2C_SendData(I2C_PORT, cmd);

    if(I2C_WaitEvent(I2C_PORT,
                     I2C_EVENT_MASTER_BYTE_TRANSMITTED,
                     100000))
        return -4;

    I2C_GenerateSTOP(I2C_PORT, ENABLE);

    return 0;
}

void BH1750_Init(void)
{
    BH1750_I2C_Init();
    if(BH1750_WriteCmd(BHPowOn) != 0)
    {
        PRINT("BH1750 not found\r\n");
        return;
    }
    BH1750_WriteCmd(BHReset);
    BH1750_WriteCmd(BHModeH1);
}
float BH1750_ReadLux(void)
{
    uint16_t data = 0;
    float lux;

    // �ȴ����߿���
    if(I2C_WaitFlag(I2C_PORT,
                    I2C_FLAG_BUSY,
                    RESET,
                    1000000))
    {
        I2C_ResetBus();
        return -1;
    }

    // START
    I2C_GenerateSTART(I2C_PORT, ENABLE);

    if(I2C_WaitEvent(I2C_PORT,
                     I2C_EVENT_MASTER_MODE_SELECT,
                     1000000))
    {
        I2C_ResetBus();
        return -2;
    }

    // ��ַ+��
    I2C_Send7bitAddress(I2C_PORT,
                        ADDR << 1,
                        I2C_Direction_Receiver);

    if(I2C_WaitEvent(I2C_PORT,
                     I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,
                     1000000))
    {
        I2C_ResetBus();
        return -3;
    }

    // �ȴ����ֽ�
    if(I2C_WaitFlag(I2C_PORT,
                    I2C_FLAG_RXNE,
                    SET,
                    1000000))
    {
        I2C_ResetBus();
        return -4;
    }

    // �����ֽ�
    data = I2C_ReceiveData(I2C_PORT);
    data <<= 8;

    // ���һ���ֽ�ǰ�ر�ACK
    I2C_AcknowledgeConfig(I2C_PORT, DISABLE);

    // STOP
    I2C_GenerateSTOP(I2C_PORT, ENABLE);

    // �ȴ����ֽ�
    if(I2C_WaitFlag(I2C_PORT,
                    I2C_FLAG_RXNE,
                    SET,
                    1000000))
    {
        I2C_ResetBus();

        // �ָ�ACK
        I2C_AcknowledgeConfig(I2C_PORT, ENABLE);

        return -5;
    }

    // �����ֽ�
    data |= I2C_ReceiveData(I2C_PORT);

    // �ָ�ACK
    I2C_AcknowledgeConfig(I2C_PORT, ENABLE);

    lux = data / 1.2f;

    return lux;
}

tmosEvents My_BH1750_Task(tmosTaskID task_id, tmosEvents events)
{
    if(events & MY_BH1750_TASK_EVENT)
    {
       float lux = 0.0f;
       lux = BH1750_ReadLux();
       if(lux < 0)
       {
           PRINT("ERR:%d\r\n",(int)lux);
       }
       else
       {
           PRINT("LUX:%d lx\r\n",(uint16_t)lux);
           User_TxData[5] = (uint16_t)(lux) & 0xff;
           User_TxData[6] = ((uint16_t)(lux) >> 8 ) & 0xff;
       }
        // ���°�����һ�δ�����10ms
        tmos_start_task(task_id, MY_BH1750_TASK_EVENT, MS1_TO_SYSTEM_TIME(150));

        return events ^ MY_BH1750_TASK_EVENT; // ����Ѵ����¼�
    }
    return 0;
}
