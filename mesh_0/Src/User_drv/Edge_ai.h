#ifndef __EDGE_AI_H
#define __EDGE_AI_H
#include "HAL.h"
#include "lwns_mesh_example.h"
#include "stdbool.h"
#include "Relay.h"

typedef struct {
    uint8_t  HC_SR501_status;   // 人体感应 0/1
    float    temperature;       // 融合温度
    float    humidity;          // 融合湿度
    uint16_t Lux;               // 融合光照
    uint16_t ADC_MQ2;           // MQ2 ADC值
    uint8_t  SW420_status;      // 震动 0/1

    uint8_t  comfort_score;     // 舒适度 0-100
    uint8_t  danger_score;      // 危险度 0-100
    uint8_t  fan_speed;         // 风扇 0/30/60/80/90
    uint8_t  light_need;        // 补光灯数量 0-4
    BOOL     intrusion_prob;    // 入侵判定
    uint8_t  gas_level;         // 燃气等级 0-3
    BOOL     fire_prob;         // 火灾判定
} Systeam_condition;

typedef struct {
    uint8_t fan;                // 风扇开关 0/1
    uint8_t fan_speed;          // 风扇速度
    uint8_t light;              // 灯光开关 0/1
    uint8_t buzzer;             // 蜂鸣器 0/1
    uint8_t gas_alarm;          // 燃气报警
    uint8_t fire_alarm;         // 火灾报警
    uint8_t intrusion_alarm;    // 入侵报警
} AI_Command;

extern Systeam_condition systeam_status;
extern AI_Command Command;

float    Fusion_temperature(void);
float    Fusion_humidity(void);
uint16_t Fusion_Lux(void);
BOOL     Fusion_HCSR501(void);
BOOL     Fusion_SW420(void);
uint16_t Fusion_MQ2(void);
uint8_t  Calc_Comfort(void);
uint8_t  Fan_Need(void);
uint8_t  Led_light(void);
uint8_t  GasLevel(void);
BOOL     FireRisk(void);
BOOL     Intrusion(void);
uint8_t  DangerScore(void);

void EdgeAI_Init(void);
void EdgeAI_Process(void);
void EdgeAI_ExecuteAction(void);

#endif
