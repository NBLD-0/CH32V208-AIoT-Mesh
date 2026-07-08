#ifndef __BH1750_H
#define __BH1750_H

#include "HAL.h"
#include "lwns_mesh_example.h"
#define MY_BH1750_TASK_EVENT 0x04

#define I2C_PORT         I2C1
#define I2C_RCC_APB      RCC_APB1Periph_I2C1
#define I2C_GPIO_RCC_APB RCC_APB2Periph_GPIOB
#define I2C_GPIO_PORT    GPIOB
#define I2C_SDA          GPIO_Pin_7
#define I2C_SCL          GPIO_Pin_6
#define ADDR             0x23


#define BHAddWrite     0x46      //从机地址+最后写方向位
#define BHAddRead      0x47      //从机地址+最后读方向位
#define BHPowDown      0x00      //关闭模块
#define BHPowOn        0x01      //打开模块等待测量指令
#define BHReset        0x07      //重置数据寄存器值在PowerOn模式下有效
#define BHModeH1       0x10      //高分辨率 单位1lx 测量时间120ms
#define BHModeH2       0x11      //高分辨率模式2 单位0.5lx 测量时间120ms
#define BHModeL        0x13      //低分辨率 单位4lx 测量时间16ms
#define BHSigModeH     0x20      //一次高分辨率 测量 测量后模块转到 PowerDown模式
#define BHSigModeH2    0x21      //同上类似
#define BHSigModeL     0x23      // 上类似

void BH1750_Init(void);
float BH1750_ReadLux(void);
tmosEvents My_BH1750_Task(tmosTaskID task_id, tmosEvents events);
#endif
