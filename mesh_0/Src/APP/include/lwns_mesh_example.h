/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_mesh_example.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/06/28
 * Description        : mesh传输程序例子
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef _LWNS_MESH_EXAMPLE_H_
#define _LWNS_MESH_EXAMPLE_H_

#include "lwns_config.h"

typedef struct
{
    uint8_t HC_SR501_status;//0-未发现
    float temperature;
    float humidity;
    uint16_t Lux;
    uint16_t ADC_MQ2;
    uint8_t SW420_status;//0-未有震动
}Ble_Node;

#define MESH_EXAMPLE_TX_PERIOD_EVT    1 << (0)
#define MESH_EXAMPLE_TX_NODE_EVT      1 << (1)
void lwns_mesh_process_init(void);

extern Ble_Node Ble_Node_1;
extern Ble_Node Ble_Node_2;
extern Ble_Node Ble_Node_3;
#endif /* _LWNS_MESH_EXAMPLE_H_ */
