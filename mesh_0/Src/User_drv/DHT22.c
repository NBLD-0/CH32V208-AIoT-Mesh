#include "dht22.h"


DHT22_Data_t DHT22_Data = {0};

/* PA0 输出模式 */
static void DHT22_Pin_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = DHT22_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(DHT22_PORT, &GPIO_InitStructure);
}

/* PA0 输入模式 */
static void DHT22_Pin_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = DHT22_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(DHT22_PORT, &GPIO_InitStructure);
}

/* 读取IO */
static uint8_t DHT22_Read_Pin(void)
{
    return GPIO_ReadInputDataBit(DHT22_PORT, DHT22_PIN);
}

/* 微秒延时 */
static void DHT22_DelayUs(uint32_t us)
{
    Delay_Us(us);
}

/* 毫秒延时 */
static void DHT22_DelayMs(uint32_t ms)
{
    Delay_Ms(ms);
}

/* DHT22启动信号 */
static void DHT22_Start(void)
{
    DHT22_Pin_Output();

    GPIO_ResetBits(DHT22_PORT, DHT22_PIN);

    /* 拉低至少1ms */
    DHT22_DelayMs(2);

    GPIO_SetBits(DHT22_PORT, DHT22_PIN);

    /* 释放总线20~40us */
    DHT22_DelayUs(30);

    DHT22_Pin_Input();
}

/* 等待响应 */
static uint8_t DHT22_CheckResponse(void)
{
    uint16_t timeout = 0;

    /* 等待低电平 */
    while(DHT22_Read_Pin())
    {
        timeout++;
        DHT22_DelayUs(1);

        if(timeout > 100)
            return 0;
    }

    timeout = 0;

    /* 等待高电平 */
    while(!DHT22_Read_Pin())
    {
        timeout++;
        DHT22_DelayUs(1);

        if(timeout > 100)
            return 0;
    }

    timeout = 0;

    /* 等待响应结束 */
    while(DHT22_Read_Pin())
    {
        timeout++;
        DHT22_DelayUs(1);

        if(timeout > 100)
            return 0;
    }

    return 1;
}

/* 读取1bit */
static uint8_t DHT22_ReadBit(void)
{
    uint16_t timeout = 0;

    /* 等待50us低电平结束 */
    while(!DHT22_Read_Pin())
    {
        timeout++;
        DHT22_DelayUs(1);

        if(timeout > 100)
            break;
    }

    /* 延时40us判断 */
    DHT22_DelayUs(40);

    if(DHT22_Read_Pin())
    {
        /* 等待高电平结束 */
        timeout = 0;

        while(DHT22_Read_Pin())
        {
            timeout++;
            DHT22_DelayUs(1);

            if(timeout > 100)
                break;
        }

        return 1;
    }
    else
    {
        return 0;
    }
}

/* 读取1字节 */
static uint8_t DHT22_ReadByte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for(i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= DHT22_ReadBit();
    }

    return data;
}

void DHT22_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    DHT22_Pin_Output();

    GPIO_SetBits(DHT22_PORT, DHT22_PIN);
}

/* 读取温湿度 */
uint8_t DHT22_Read(DHT22_Data_t *data)
{
    uint8_t buf[5];
    uint8_t i;
    uint8_t checksum;

    DHT22_Start();

    if(!DHT22_CheckResponse())
    {
        return 0;
    }

    for(i = 0; i < 5; i++)
    {
        buf[i] = DHT22_ReadByte();
    }

    checksum = buf[0] + buf[1] + buf[2] + buf[3];

    if(checksum != buf[4])
    {
        return 0;
    }

    /* 湿度 */
    data->humidity =
        ((buf[0] << 8) | buf[1]) * 0.1f;

    /* 温度 */
    if(buf[2] & 0x80)
    {
        data->temperature =
            -(((buf[2] & 0x7F) << 8) | buf[3]) * 0.1f;
    }
    else
    {
        data->temperature =
            (((buf[2] << 8) | buf[3])) * 0.1f;
    }

    return 1;
}

tmosEvents My_DHT22_Task(tmosTaskID task_id, tmosEvents events)
{
    if(events & MY_DHT22_TASK_EVENT)
    {
        // 执行你的任务逻辑
        DHT22_Read(&DHT22_Data);
        PRINT("temp:%d.humi:%d\r\n",(uint16_t)(DHT22_Data.temperature*10.0f),(uint16_t)(DHT22_Data.humidity*10.0f));
        // 重新安排下一次触发：10ms
        tmos_start_task(task_id, MY_DHT22_TASK_EVENT, MS1_TO_SYSTEM_TIME(300));

        return events ^ MY_DHT22_TASK_EVENT; // 清除已处理事件
    }
    return 0;
}
