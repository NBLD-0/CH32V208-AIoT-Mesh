#include "Edge_ai.h"
#include <math.h>

Systeam_condition systeam_status;
AI_Command Command;
//temperature
float Fusion_temperature(void)
{
    systeam_status.temperature = (Ble_Node_1.temperature + Ble_Node_3.temperature) / 2.0f;
    return systeam_status.temperature;
}

//humidity
float Fusion_humidity(void)
{
    systeam_status.humidity = (Ble_Node_1.humidity + Ble_Node_3.humidity) / 2.0f;
    return systeam_status.humidity;
}

//Lux
uint16_t Fusion_Lux(void)
{
    systeam_status.Lux = (Ble_Node_1.Lux + Ble_Node_3.Lux) / 2.0f;
    return systeam_status.Lux;
}

//HC-SR501
BOOL Fusion_HCSR501(void)
{
    systeam_status.HC_SR501_status = Ble_Node_1.HC_SR501_status || Ble_Node_2.HC_SR501_status;
    return systeam_status.HC_SR501_status;
}

//SW420
BOOL Fusion_SW420(void)
{
    systeam_status.SW420_status = Ble_Node_2.SW420_status || Ble_Node_3.SW420_status;
    return systeam_status.SW420_status;
}

//MQ2
uint16_t Fusion_MQ2(void)
{
    systeam_status.ADC_MQ2 = Ble_Node_2.ADC_MQ2;
    return systeam_status.ADC_MQ2;
}


uint8_t Calc_Comfort(void)
{
    float comfort = 0.0f;

    float temp_score;
    float humi_score;
    float lux_score;

    /****************************************
     * �¶�����
     * ����¶� 24��
     ***************************************/
    float temp_error =
        fabsf(systeam_status.temperature - 24.0f);

    temp_score = 100.0f - temp_error * 4.0f;

    if(temp_score < 0)
        temp_score = 0;

    if(temp_score > 100)
        temp_score = 100;


    /****************************************
     * ʪ������
     * ���ʪ�� 50%
     ***************************************/
    float humi_error =
        fabsf(systeam_status.humidity - 50.0f);

    humi_score = 100.0f - humi_error * 2.0f;

    if(humi_score < 0)
        humi_score = 0;

    if(humi_score > 100)
        humi_score = 100;


    /****************************************
     * ��������
     * ��ѹ��� 500Lux
     ***************************************/
    float lux_error =
        fabsf((float)systeam_status.Lux - 500.0f);

    lux_score = 100.0f - lux_error / 8.0f;

    if(lux_score < 0)
        lux_score = 0;

    if(lux_score > 100)
        lux_score = 100;


    /****************************************
     * ��Ȩ�ں�
     *
     * �¶� 50%
     * ʪ�� 30%
     * ���� 20%
     ***************************************/
    comfort =
          temp_score * 0.5f
        + humi_score * 0.3f
        + lux_score  * 0.2f;


    /****************************************
     * �����������
     ***************************************/
    if(systeam_status.HC_SR501_status)
    {
        comfort += 5;
    }


    if(comfort > 100)
        comfort = 100;

    if(comfort < 0)
        comfort = 0;


    systeam_status.comfort_score =
        (uint8_t)comfort;

    return systeam_status.comfort_score;
}

//���ȵ�λ

uint8_t Fan_Need(void)
{
    float temp = systeam_status.temperature;
    uint8_t Fan_Speed = 0;
    if(temp <= 26 )
    {
        Fan_Speed = 0;
    }
    else if(temp <= 30){
        Fan_Speed = 30;
    }
    else if(temp <= 32)
    {
        Fan_Speed = 60;
    }
    else if(temp <= 34)
    {
        Fan_Speed = 80;
    }
    else {
        Fan_Speed = 90;
    }
    systeam_status.fan_speed = Fan_Speed;
    return systeam_status.fan_speed;
}

uint8_t Led_light(void)
{
    uint8_t Led_Num = 0;//���ĵ�����
    if(systeam_status.HC_SR501_status ==1)
    {
        if(systeam_status.Lux < 100)
        {
            Led_Num = 4;
        }
        else if(systeam_status.Lux < 200)
        {
            Led_Num = 3;
        }
        else if(systeam_status.Lux < 300)
        {
            Led_Num = 2;
        }
        else if(systeam_status.Lux < 400)
        {
            Led_Num = 1;
        }
        else {
            Led_Num = 0;
        }
    }
    else {
        Led_Num = 0;
    }
    systeam_status.light_need = Led_Num;
    return systeam_status.light_need;
}


uint8_t GasLevel(void)
{
    uint16_t MQ2 = systeam_status.ADC_MQ2;
    uint8_t Level = 0;
    if(MQ2 < 200)
    {
        Level = 0;
    }
    else if (MQ2 < 400) {
        Level = 1;
    }
    else if (MQ2 < 600) {
        Level = 2;
    }
    else{
        Level = 3;
    }
    systeam_status.gas_level = Level;
    return systeam_status.gas_level;
}

BOOL FireRisk(void)
{
    if(systeam_status.ADC_MQ2 > 500 && systeam_status.temperature > 40.0f)
    {
        systeam_status.fire_prob = TRUE;
        return TRUE;
    }
    systeam_status.fire_prob = FALSE;
    return FALSE;
}

BOOL Intrusion(void)
{
    if(systeam_status.HC_SR501_status && systeam_status.SW420_status)
    {
        systeam_status.intrusion_prob = TRUE;
        return TRUE;
    }
    systeam_status.intrusion_prob = FALSE;
    return FALSE;
}

uint8_t DangerScore(void)
{
    uint8_t score = 0;

    if(systeam_status.temperature > 30)
        score += 10;

    if(systeam_status.temperature > 40)
        score += 20;

    if(systeam_status.humidity > 80)
        score += 10;

    if(systeam_status.ADC_MQ2 > 400)
        score += 30;

    if(systeam_status.HC_SR501_status)
        score += 10;

    if(systeam_status.SW420_status)
        score += 20;

    systeam_status.danger_score = score;

    return score;
}

// ============ 蜂鸣器控制 (PA6 复用为LED, PB14为蜂鸣器) ============
// 蜂鸣器接PB14，高电平响
#define BUZZER_PIN   GPIO_Pin_14
#define BUZZER_PORT  GPIOB

static void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN, RESET);
}

static void Buzzer_On(void)
{
    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN, SET);
}

static void Buzzer_Off(void)
{
    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN, RESET);
}

// LED状态指示 (PA6)
#define LED_PIN   GPIO_Pin_6
#define LED_PORT  GPIOA

static void LED_Set(uint8_t on)
{
    GPIO_WriteBit(LED_PORT, LED_PIN, on ? SET : RESET);
}

// ============ 气体趋势检测 ============
#define GAS_TREND_WINDOW  10
static uint16_t gas_trend_buf[GAS_TREND_WINDOW];
static uint8_t  gas_trend_idx = 0;
static uint8_t  gas_trend_cnt = 0;

static BOOL Gas_IsRising(void)
{
    if(gas_trend_cnt < 5) return FALSE;
    // 比较最近5个值的均值 vs 之前5个值的均值
    uint32_t recent = 0, older = 0;
    for(uint8_t i = 0; i < 5; i++)
    {
        uint8_t ri = (gas_trend_idx - 1 - i + GAS_TREND_WINDOW) % GAS_TREND_WINDOW;
        uint8_t oi = (gas_trend_idx - 6 - i + GAS_TREND_WINDOW) % GAS_TREND_WINDOW;
        recent += gas_trend_buf[ri];
        older  += gas_trend_buf[oi];
    }
    return (recent > older + 100);  // 最近均值比之前高100以上
}

static void Gas_Trend_Update(uint16_t value)
{
    gas_trend_buf[gas_trend_idx] = value;
    gas_trend_idx = (gas_trend_idx + 1) % GAS_TREND_WINDOW;
    if(gas_trend_cnt < GAS_TREND_WINDOW) gas_trend_cnt++;
}

// ============ 入侵时间窗口 ============
#define INTRUSION_WINDOW_MS  10000  // 10秒内同时检测到才算
static uint32_t last_motion_time = 0;
static uint32_t last_vibration_time = 0;

// ============ EdgeAI 初始化 ============
void EdgeAI_Init(void)
{
    Buzzer_Init();
    memset(&systeam_status, 0, sizeof(systeam_status));
    memset(&Command, 0, sizeof(Command));
    memset(gas_trend_buf, 0, sizeof(gas_trend_buf));
    PRINT("Edge AI initialized\r\n");
}

// ============ EdgeAI 主入口 ============
void EdgeAI_Process(void)
{
    // 1. 多传感器融合
    Fusion_temperature();
    Fusion_humidity();
    Fusion_Lux();
    Fusion_HCSR501();
    Fusion_SW420();
    Fusion_MQ2();

    // 2. 更新气体趋势
    Gas_Trend_Update(systeam_status.ADC_MQ2);

    // 3. 更新入侵时间戳
    uint32_t now_ms = TMOS_GetSystemClock() / 10;  // 粗略毫秒
    if(systeam_status.HC_SR501_status) last_motion_time = now_ms;
    if(systeam_status.SW420_status)    last_vibration_time = now_ms;

    // 4. 评估各项指标
    Calc_Comfort();
    Fan_Need();
    Led_light();
    GasLevel();
    FireRisk();
    Intrusion();
    DangerScore();

    // 5. 执行动作
    EdgeAI_ExecuteAction();
}

// ============ 动作执行（带优先级） ============
void EdgeAI_ExecuteAction(void)
{
    memset(&Command, 0, sizeof(Command));

    // === 优先级1：燃气泄漏（最危险，先关灯防电火花） ===
    if(systeam_status.gas_level >= 3 || (Gas_IsRising() && systeam_status.ADC_MQ2 > 400))
    {
        Command.gas_alarm = 1;
        Command.fan = 1;
        Command.fan_speed = 90;
        Command.light = 0;          // 先关灯！
        Command.buzzer = 1;
        Relay_Light_Off();
        Relay_Fan_On();
        Buzzer_On();
        LED_Set(1);
        PRINT(">> GAS ALARM! MQ2:%d\r\n", systeam_status.ADC_MQ2);
        return;
    }

    // === 优先级2：火灾风险 ===
    if(systeam_status.fire_prob)
    {
        Command.fire_alarm = 1;
        Command.fan = 1;
        Command.fan_speed = 90;
        Command.buzzer = 1;
        Relay_Fan_On();
        Buzzer_On();
        LED_Set(1);
        PRINT(">> FIRE RISK! Temp:%d MQ2:%d\r\n",
              (uint8_t)systeam_status.temperature, systeam_status.ADC_MQ2);
        return;
    }

    // === 优先级3：入侵检测（人体+震动10秒内同时出现） ===
    if(systeam_status.intrusion_prob)
    {
        uint32_t delay = (last_motion_time > last_vibration_time) ?
                         (last_motion_time - last_vibration_time) :
                         (last_vibration_time - last_motion_time);
        if(delay < INTRUSION_WINDOW_MS / 10)
        {
            Command.intrusion_alarm = 1;
            Command.light = 1;
            Command.buzzer = 1;
            Relay_Light_On();
            Buzzer_On();
            LED_Set(1);
            PRINT(">> INTRUSION! Motion+Vibration\r\n");
            return;
        }
    }

    // === 无报警，正常环境调节 ===
    Buzzer_Off();

    // 风扇控制
    if(systeam_status.fan_speed > 0)
    {
        Command.fan = 1;
        Command.fan_speed = systeam_status.fan_speed;
        Relay_Fan_On();
    }
    else
    {
        Relay_Fan_Off();
    }

    // 智能灯光：有人 + 光照不足 → 开灯
    if(systeam_status.HC_SR501_status && systeam_status.Lux < 300)
    {
        Command.light = 1;
        Relay_Light_On();
    }
    else if(!systeam_status.HC_SR501_status || systeam_status.Lux > 800)
    {
        Command.light = 0;
        Relay_Light_Off();
    }

    LED_Set(0);  // 无报警时LED灭
}



