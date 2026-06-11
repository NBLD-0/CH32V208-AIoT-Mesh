#include "MQ2.h"
#include "string.h"
/* DMA缓存大小 */
#define ADC_DMA_SIZE   64

/* ADC DMA Buffer */
volatile uint16_t ADC_DMA_Buffer[ADC_DMA_SIZE];

void MQ2_ADC_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_InitTypeDef ADC_InitStruct = {0};
    DMA_InitTypeDef DMA_InitStruct = {0};
    RCC_APB2PeriphClockCmd(MQ2_ADC_RCC_APB, ENABLE);
    RCC_APB2PeriphClockCmd(MQ2_ADC_GPIO_RCC_APB,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);


    GPIO_InitStruct.GPIO_Pin = MQ2_ADC_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MQ2_ADC_PORT, &GPIO_InitStruct);

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(MQ2_ADC, &ADC_InitStruct);
    ADC_Cmd(MQ2_ADC, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStruct.DMA_PeripheralBaseAddr =  (uint32_t)&ADC1->RDATAR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_DMA_Buffer;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = ADC_DMA_SIZE;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_RegularChannelConfig(MQ2_ADC, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
    ADC_ResetCalibration(MQ2_ADC);
    while(ADC_GetResetCalibrationStatus(MQ2_ADC));
    ADC_StartCalibration(MQ2_ADC);
    while(ADC_GetCalibrationStatus(MQ2_ADC));

    ADC_DMACmd(MQ2_ADC, ENABLE);
    ADC_SoftwareStartConvCmd(MQ2_ADC, ENABLE);
}


uint16_t MQ2_ReadStable(void)
{
    uint16_t buf[ADC_DMA_SIZE];
    memcpy(buf,(const void*)ADC_DMA_Buffer,sizeof(buf));

    // 冒泡排序
    for(int i=0;i<ADC_DMA_SIZE-1;i++)
    {
        for(int j=0;j<ADC_DMA_SIZE-1-i;j++)
        {
            if(buf[j] > buf[j+1])
            {
                uint16_t t = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = t;
            }
        }
    }
    // 去掉最大最小值
    uint32_t sum = 0;
    for(int i=4;i<12;i++)
    {
        sum += buf[i];
    }
    return sum / 8;
}

tmosEvents My_MQ2_Task(tmosTaskID task_id, tmosEvents events)
{
    if(events & MY_MQ2_ADC_TASK_EVENT)
    {
       uint16_t ADC_MQ2 = 0.0f;
       static uint8_t PRINT_CNT = 0;
       PRINT_CNT++;
       ADC_MQ2 = MQ2_ReadStable();
       float voltage;
       voltage = ADC_MQ2 * 3.3f / 4095.0f;
       {
           if(PRINT_CNT >= 10)
           {
               PRINT_CNT = 0;
               PRINT("MQ2:%d  VOL:%d\r\n",(uint16_t)ADC_MQ2,(uint16_t)(voltage * 100));
           }
       }
        // 重新安排下一次触发：10ms
        tmos_start_task(task_id, MY_MQ2_ADC_TASK_EVENT, MS1_TO_SYSTEM_TIME(10));

        return events ^ MY_MQ2_ADC_TASK_EVENT; // 清除已处理事件
    }
    return 0;
}
