#include "debug.h"
#include "lcd.h"
#include "pic.h"
#include "timer.h"
#include "key.h"
#include "uart.h"
#include "audio.h"
#include "as608.h"
#include "HAL.h"
#include "lwns_adapter_blemesh_mac.h"
#include "lwns_mesh_example.h"
#include <string.h>

void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static void key_proc(void);
static void lock_proc(void);
static void as608_proc(void);
static void lcd_proc(void);
static void lcd_show_startup_logo(void);
static void lcd_show_key_labels(void);
static void lcd_draw_key_label(u16 x, u16 y, const u8 *glyph1, const u8 *glyph2);
static void lcd_draw_glyph12(u16 x, u16 y, const u8 *glyph);
static void mesh_proc(void);
static void mesh_notify_door_open(void);
static void scheduler_init(void);
static void scheduler_run(void);
static void key_clear(void);
static void string_copy(u8 *src, u8 *dst, u8 len);
static u8 string_check(u8 *a, u8 *b, u8 len);
static u8 rfid_check(void);
static void Ps_Wait(void);
static void lcd_show_message(u16 y, u8 msg);

#define SPLASH_BLUE 0x1A6F

enum
{
    UI_NONE = 0,
    UI_INPUT_PASSWORD,
    UI_ENTER_PASSWORD,
    UI_ADMIN_PASSWORD,
    UI_CHANGE_PASSWORD,
    UI_ADD_FINGER,
    UI_ADD_CARD,
    UI_SWIPE_CARD,
    UI_TOUCH_FINGER,
    UI_FINGER_FAILED,
    UI_CARD_FAILED,
    UI_VERIFY_FAILED,
    UI_NEW_PASSWORD,
    UI_CLEAR_OK,
    UI_DOOR_OPEN,
    UI_LOCKED,
    UI_SUCCESS,
    UI_BOOT_OK
};

volatile unsigned long int uwtick;
__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];

static u8 key_val, key_old, key_down, key_up;
static u8 key_sample, key_sample_count;
static u8 password[6] = {1, 2, 3, 4, 5, 6};
static u8 password_cmd[6] = {2, 7, 7, 5, 1, 6};
static u8 key_temp[7] = {10, 10, 10, 10, 10, 10, 10};
static u8 key_index;
static u8 key_index_old;
static u16 time5000ms;
static volatile u8 lock_flag = 1;
static volatile u8 password_error;
static u16 time15s = 15;
static u16 time1000ms;
static volatile u8 show_flag;
static u8 show_flag_old = 0xff;
static volatile u8 mode;
static volatile u8 ui_message = UI_INPUT_PASSWORD;
static volatile u8 ui_sub_message = UI_NONE;
static u8 ui_message_old = 0xff;
static u8 ui_sub_message_old = 0xff;
static volatile u16 ui_return_ms;
static volatile u8 mesh_door_open_pending;

static const u8 msg_input_password[] = {0xC7, 0xEB, 0xCA, 0xE4, 0xC8, 0xEB, 0xC3, 0xDC, 0xC2, 0xEB, 0x00};
static const u8 msg_enter_password[] = {0xC7, 0xEB, 0xC8, 0xEB, 0xC3, 0xDC, 0xC2, 0xEB, 0x00};
static const u8 msg_admin_password[] = {0xC7, 0xEB, 0xCA, 0xE4, 0xC8, 0xEB, 0xB9, 0xDC, 0xC0, 0xED, 0xD4, 0xB1, 0xC3, 0xDC, 0xC2, 0xEB, 0x00};
static const u8 msg_change_password[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xC3, 0xDC, 0xC2, 0xEB, 0x00};
static const u8 msg_add_finger[] = {0xC2, 0xBC, 0xC8, 0xEB, 0xD6, 0xB8, 0xCE, 0xC6, 0x00};
static const u8 msg_add_card[] = {0xC2, 0xBC, 0xC8, 0xEB, 0xBF, 0xA8, 0xC6, 0xAC, 0x00};
static const u8 msg_swipe_card[] = {0xC7, 0xEB, 0xCB, 0xA2, 0xBF, 0xA8, 0x00};
static const u8 msg_touch_finger[] = {0xC7, 0xEB, 0xB0, 0xB4, 0xD6, 0xB8, 0xCE, 0xC6, 0x00};
static const u8 msg_finger_failed[] = {0xD6, 0xB8, 0xCE, 0xC6, 0xCA, 0xA7, 0xB0, 0xDC, 0x00};
static const u8 msg_card_failed[] = {0xCB, 0xA2, 0xBF, 0xA8, 0xCA, 0xA7, 0xB0, 0xDC, 0x00};
static const u8 msg_verify_failed[] = {0xD1, 0xE9, 0xD6, 0xA4, 0xCA, 0xA7, 0xB0, 0xDC, 0x00};
static const u8 msg_new_password[] = {0xD0, 0xC2, 0xC3, 0xDC, 0xC2, 0xEB, 0x00};
static const u8 msg_clear_ok[] = {0xC7, 0xE5, 0xBF, 0xD5, 0xB3, 0xC9, 0xB9, 0xA6, 0x00};
static const u8 msg_door_open[] = {0xC3, 0xC5, 0xD2, 0xD1, 0xBF, 0xAA, 0x00};
static const u8 msg_locked[] = {0xD2, 0xD1, 0xC9, 0xCF, 0xCB, 0xF8, 0x00};
static const u8 msg_success[] = {0xB3, 0xC9, 0xB9, 0xA6, 0x00};
static const u8 msg_boot_ok[] = {0xC6, 0xF4, 0xB6, 0xAF, 0xB3, 0xC9, 0xB9, 0xA6, 0x00};
static const u8 msg_splash_enable_design[] = {0xB8, 0xB3, 0xC4, 0xDC, 0xC9, 0xE8, 0xBC, 0xC6, 0x00};
static const u8 msg_splash_design_light[] = {0xC9, 0xE8, 0xBC, 0xC6, 0xB5, 0xE3, 0xC1, 0xC1, 0x00};
static const u8 glyph_key_que[] = {
    0x40, 0x00, 0xCF, 0x03, 0x24, 0x02, 0x04, 0x01,
    0xE2, 0x07, 0xAE, 0x04, 0xEB, 0x07, 0xAA, 0x04,
    0xEA, 0x07, 0xAE, 0x04, 0xAA, 0x04, 0x10, 0x06
};
static const u8 glyph_key_ren[] = {
    0x82, 0x00, 0x84, 0x00, 0x80, 0x00, 0x80, 0x00,
    0x87, 0x00, 0x84, 0x00, 0x44, 0x01, 0x44, 0x01,
    0x54, 0x01, 0x2C, 0x02, 0x24, 0x02, 0x10, 0x04
};
static const u8 glyph_key_hui[] = {
    0xFE, 0x07, 0x02, 0x04, 0x02, 0x04, 0xF2, 0x04,
    0x92, 0x04, 0x92, 0x04, 0xF2, 0x04, 0x02, 0x04,
    0x02, 0x04, 0xFE, 0x07, 0x02, 0x04, 0x00, 0x00
};
static const u8 glyph_key_tui[] = {
    0xF2, 0x03, 0x14, 0x02, 0xF0, 0x03, 0x10, 0x02,
    0xF7, 0x03, 0x54, 0x04, 0x94, 0x02, 0x54, 0x01,
    0x34, 0x02, 0x14, 0x02, 0x0A, 0x00, 0xF1, 0x07
};

static u8 rfid_index;
static u8 rfid_temp[4] = {0};
static u8 rfid[4][4] = {0};
static u8 rfid_password_index;

static volatile u8 uart4_rec_string[20] = {0};
static volatile u8 uart4_rec_index;
static volatile u8 uart4_rec_tick;
static volatile u8 ps_wait_flag;
static u8 as608_proc_flag;
static u8 as608_proc_flag_old;
static u8 as608_store_index = 1;

static void key_proc(void)
{
    u8 raw_key = key_read();

    if(raw_key == key_sample)
    {
        if(key_sample_count < 2) key_sample_count++;
    }
    else
    {
        key_sample = raw_key;
        key_sample_count = 0;
    }
    if(key_sample_count < 1) return;

    key_val = key_sample;
    key_down = (key_val != 0 && key_old == 0) ? key_val : 0;
    key_up = (key_val == 0 && key_old != 0) ? key_old : 0;
    key_old = key_val;

    if(key_down == 15) key_down = 16;
    else if(key_down == 16) key_down = 15;

    if(password_error == 3) return;
    if(key_down) audio_play(1);

    switch(key_down)
    {
        case 1: key_temp[key_index++] = 1; break;
        case 2: key_temp[key_index++] = 2; break;
        case 3: key_temp[key_index++] = 3; break;
        case 4:
            if(mode == 0)
            {
                mode = 1;
                show_flag = 0;
                ui_message = UI_CHANGE_PASSWORD;
                ui_sub_message = UI_ADMIN_PASSWORD;
                ui_return_ms = 0;
                audio_play(6);
                password_error = 0;
                key_clear();
            }
            break;
        case 5: key_temp[key_index++] = 4; break;
        case 6: key_temp[key_index++] = 5; break;
        case 7: key_temp[key_index++] = 6; break;
        case 8:
            if(mode == 0)
            {
                mode = 5;
                show_flag = 0;
                ui_message = UI_ADD_FINGER;
                ui_sub_message = UI_ADMIN_PASSWORD;
                ui_return_ms = 0;
                audio_play(18);
                password_error = 0;
                key_clear();
            }
            break;
        case 9: key_temp[key_index++] = 7; break;
        case 10: key_temp[key_index++] = 8; break;
        case 11: key_temp[key_index++] = 9; break;
        case 12:
            if(mode == 0)
            {
                mode = 3;
                show_flag = 0;
                ui_message = UI_ADD_CARD;
                ui_sub_message = UI_ADMIN_PASSWORD;
                ui_return_ms = 0;
                audio_play(13);
                password_error = 0;
                key_clear();
            }
            break;
        case 13:
            key_clear();
            if(mode == 6)
            {
                PS_Empty();
                mode = 0;
                show_flag = 0;
                ui_message = UI_CLEAR_OK;
                ui_sub_message = UI_NONE;
                ui_return_ms = 1500;
                password_error = 0;
            }
            break;
        case 14: key_temp[key_index++] = 0; break;
        case 15:
            if(key_index)
            {
                key_index--;
                key_temp[key_index] = 10;
            }
            break;
        case 16:
            switch(mode)
            {
                case 0:
                    if(string_check(key_temp, password, 6))
                    {
                        lock_flag = 0;
                        time5000ms = 0;
                        lock(0);
                        mesh_notify_door_open();
                        show_flag = 1;
                        ui_message = UI_DOOR_OPEN;
                        ui_sub_message = UI_NONE;
                        audio_play(3);
                        key_clear();
                        password_error = 0;
                    }
                    else
                    {
                        audio_play(4);
                        key_clear();
                        if(++password_error == 3)
                        {
                            audio_play(5);
                            show_flag = 2;
                            ui_message = UI_LOCKED;
                            ui_sub_message = UI_NONE;
                        }
                    }
                    break;
                case 1:
                    if(string_check(key_temp, password_cmd, 6))
                    {
                        mode = 2;
                        show_flag = 0;
                        ui_message = UI_NEW_PASSWORD;
                        ui_sub_message = UI_NONE;
                        ui_return_ms = 0;
                        audio_play(7);
                        password_error = 0;
                        key_clear();
                    }
                    else if(++password_error == 3)
                    {
                        audio_play(10);
                        mode = 0;
                        show_flag = 2;
                        ui_message = UI_LOCKED;
                        ui_sub_message = UI_NONE;
                        key_clear();
                    }
                    else
                    {
                        ui_message = UI_VERIFY_FAILED;
                        ui_sub_message = UI_ADMIN_PASSWORD;
                        audio_play(9);
                        key_clear();
                    }
                    break;
                case 2:
                    string_copy(key_temp, password, 6);
                    audio_play(8);
                    mode = 0;
                    show_flag = 0;
                    ui_message = UI_SUCCESS;
                    ui_sub_message = UI_NONE;
                    ui_return_ms = 1500;
                    password_error = 0;
                    key_clear();
                    break;
                case 3:
                    if(string_check(key_temp, password_cmd, 6))
                    {
                        mode = 4;
                        show_flag = 0;
                        ui_message = UI_SWIPE_CARD;
                        ui_sub_message = UI_NONE;
                        ui_return_ms = 0;
                        audio_play(14);
                        password_error = 0;
                        key_clear();
                    }
                    else if(++password_error == 3)
                    {
                        audio_play(10);
                        mode = 0;
                        show_flag = 2;
                        ui_message = UI_LOCKED;
                        ui_sub_message = UI_NONE;
                        key_clear();
                    }
                    else
                    {
                        ui_message = UI_VERIFY_FAILED;
                        ui_sub_message = UI_ADMIN_PASSWORD;
                        audio_play(9);
                        key_clear();
                    }
                    break;
                case 5:
                    if(string_check(key_temp, password_cmd, 6))
                    {
                        mode = 6;
                        show_flag = 0;
                        ui_message = UI_TOUCH_FINGER;
                        ui_sub_message = UI_NONE;
                        ui_return_ms = 0;
                        audio_play(19);
                        password_error = 0;
                        key_clear();
                    }
                    else if(++password_error == 3)
                    {
                        audio_play(10);
                        mode = 0;
                        show_flag = 2;
                        ui_message = UI_LOCKED;
                        ui_sub_message = UI_NONE;
                        key_clear();
                    }
                    else
                    {
                        ui_message = UI_VERIFY_FAILED;
                        ui_sub_message = UI_ADMIN_PASSWORD;
                        audio_play(9);
                        key_clear();
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if(key_index > 6) key_index = 6;
}

static void lock_proc(void)
{
    lock(lock_flag);
}

static void mesh_notify_door_open(void)
{
    mesh_door_open_pending = 1;
}

static void mesh_proc(void)
{
    if(mesh_door_open_pending)
    {
        mesh_door_open_pending = 0;
        lwns_mesh_send_door_open();
    }
}

static void as608_proc(void)
{
    as608_proc_flag = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);
    if(as608_proc_flag == as608_proc_flag_old) return;
    as608_proc_flag_old = as608_proc_flag;
    if(as608_proc_flag == 0) return;

    if(mode == 6)
    {
        PS_GetImage();
        Ps_Wait();
        PS_GenChar(1);
        Ps_Wait();
        PS_GetImage();
        Ps_Wait();
        PS_GenChar(2);
        Ps_Wait();
        PS_RegModel();
        Ps_Wait();
        PS_StoreChar(as608_store_index++);
        Ps_Wait();
        audio_play(21);
        mode = 0;
        show_flag = 0;
        ui_message = UI_SUCCESS;
        ui_sub_message = UI_NONE;
        ui_return_ms = 1500;
        password_error = 0;
        key_clear();
        return;
    }

    if(mode == 0)
    {
        PS_GetImage();
        Ps_Wait();
        PS_GenChar(2);
        Ps_Wait();
        PS_Search();
        Ps_Wait();

        if(uart4_rec_string[13] > 50)
        {
            audio_play(16);
            lock_flag = 0;
            time5000ms = 0;
            lock(0);
            mesh_notify_door_open();
            show_flag = 1;
            ui_message = UI_DOOR_OPEN;
            ui_sub_message = UI_NONE;
            password_error = 0;
        }
        else
        {
            show_flag = 0;
            ui_message = UI_FINGER_FAILED;
            ui_sub_message = UI_NONE;
            ui_return_ms = 1500;
            audio_play(17);
        }
    }
}

static void lcd_proc(void)
{
    if(key_index != key_index_old)
    {
        u8 i;
        LCD_Fill(16, 45, 112, 66, YELLOW);
        for(i = 0; i < key_index && i < 6; i++)
        {
            LCD_ShowChar(20 + 16 * i, 45, '*', RED, YELLOW, 16, 0);
        }
        key_index_old = key_index;
    }

    if(show_flag != show_flag_old || ui_message != ui_message_old || ui_sub_message != ui_sub_message_old)
    {
        LCD_Fill(0, 0, 128, 32, WHITE);
        show_flag_old = show_flag;
        ui_message_old = ui_message;
        ui_sub_message_old = ui_sub_message;
    }

    switch(show_flag)
    {
        case 0:
            lcd_show_message(0, ui_message);
            lcd_show_message(16, ui_sub_message);
            break;
        case 1:
            lcd_show_message(0, ui_message);
            break;
        case 2:
            lcd_show_message(0, ui_message);
            LCD_ShowIntNum(72, 16, time15s, 2, RED, WHITE, 16);
            break;
        default:
            break;
    }
}

static void lcd_show_message(u16 y, u8 msg)
{
    const u8 *text = msg_input_password;

    switch(msg)
    {
        case UI_NONE:
            return;
        case UI_ENTER_PASSWORD:
            text = msg_enter_password;
            break;
        case UI_ADMIN_PASSWORD:
            text = msg_admin_password;
            break;
        case UI_CHANGE_PASSWORD:
            text = msg_change_password;
            break;
        case UI_ADD_FINGER:
            text = msg_add_finger;
            break;
        case UI_ADD_CARD:
            text = msg_add_card;
            break;
        case UI_SWIPE_CARD:
            text = msg_swipe_card;
            break;
        case UI_TOUCH_FINGER:
            text = msg_touch_finger;
            break;
        case UI_FINGER_FAILED:
            text = msg_finger_failed;
            break;
        case UI_CARD_FAILED:
            text = msg_card_failed;
            break;
        case UI_VERIFY_FAILED:
            text = msg_verify_failed;
            break;
        case UI_NEW_PASSWORD:
            text = msg_new_password;
            break;
        case UI_CLEAR_OK:
            text = msg_clear_ok;
            break;
        case UI_DOOR_OPEN:
            text = msg_door_open;
            break;
        case UI_LOCKED:
            text = msg_locked;
            break;
        case UI_SUCCESS:
            text = msg_success;
            break;
        case UI_BOOT_OK:
            text = msg_boot_ok;
            break;
        case UI_INPUT_PASSWORD:
        default:
            text = msg_input_password;
            break;
    }

    lcd_show_chinese(0, y, (u8 *)text, RED, WHITE, 16, 0);
}

static void lcd_show_startup_logo(void)
{
    u8 i;

    LCD_Fill(0, 0, 128, 128, WHITE);

    for(i = 0; i < 3; i++)
    {
        LCD_DrawRectangle(8 + i, 16 + i, 42 - i, 54 - i, SPLASH_BLUE);
    }

    for(i = 0; i < 5; i++)
    {
        u16 y = 20 + i * 7;
        LCD_DrawLine(3, y, 8, y, SPLASH_BLUE);
        LCD_DrawLine(42, y, 47, y, SPLASH_BLUE);
    }

    for(i = 0; i < 5; i++)
    {
        u16 x = 12 + i * 6;
        LCD_DrawLine(x, 11, x, 16, SPLASH_BLUE);
        LCD_DrawLine(x, 54, x, 59, SPLASH_BLUE);
    }

    LCD_ShowString(14, 24, (const u8 *)"9", SPLASH_BLUE, WHITE, 32, 0);
    LCD_ShowString(29, 36, (const u8 *)"th", SPLASH_BLUE, WHITE, 16, 0);

    LCD_ShowString(48, 18, (const u8 *)"AI", SPLASH_BLUE, WHITE, 16, 0);
    lcd_show_chinese(64, 18, (u8 *)msg_splash_enable_design, SPLASH_BLUE, WHITE, 16, 0);
    lcd_show_chinese(48, 38, (u8 *)msg_splash_design_light, SPLASH_BLUE, WHITE, 16, 0);
    LCD_ShowString(112, 38, (const u8 *)"AI", SPLASH_BLUE, WHITE, 16, 0);

    LCD_ShowString(12, 76, (const u8 *)"AI for Design", SPLASH_BLUE, WHITE, 16, 0);
    LCD_ShowString(0, 96, (const u8 *)"& Design for AI!", SPLASH_BLUE, WHITE, 16, 0);
}

static void lcd_show_key_labels(void)
{
    LCD_Fill(64, 112, 95, 127, WHITE);
    LCD_DrawRectangle(64, 112, 95, 127, LGRAYBLUE);
    lcd_draw_key_label(68, 114, glyph_key_que, glyph_key_ren);

    LCD_Fill(96, 112, 127, 127, WHITE);
    LCD_DrawRectangle(96, 112, 127, 127, LGRAYBLUE);
    lcd_draw_key_label(100, 114, glyph_key_hui, glyph_key_tui);
}

static void lcd_draw_key_label(u16 x, u16 y, const u8 *glyph1, const u8 *glyph2)
{
    lcd_draw_glyph12(x, y, glyph1);
    lcd_draw_glyph12(x + 12, y, glyph2);
}

static void lcd_draw_glyph12(u16 x, u16 y, const u8 *glyph)
{
    u8 row, col;
    u8 low, high, bit;

    for(row = 0; row < 12; row++)
    {
        low = glyph[row * 2];
        high = glyph[row * 2 + 1];

        for(col = 0; col < 12; col++)
        {
            bit = (col < 8) ? (low & (1 << col)) : (high & (1 << (col - 8)));
            LCD_DrawPoint(x + col, y + row, bit ? RED : WHITE);
        }
    }
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        uwtick++;
        uart4_rec_tick++;

        if(ui_return_ms)
        {
            ui_return_ms--;
            if(ui_return_ms == 0 && lock_flag)
            {
                show_flag = 0;
                mode = 0;
                ui_message = UI_INPUT_PASSWORD;
                ui_sub_message = UI_NONE;
            }
        }

        if(lock_flag == 0)
        {
            if(++time5000ms >= 5000)
            {
                time5000ms = 0;
                lock_flag = 1;
                lock(1);
                show_flag = 0;
                ui_message = UI_INPUT_PASSWORD;
                ui_sub_message = UI_NONE;
                audio_play(2);
            }
        }

        if(password_error == 3)
        {
            if(++time1000ms >= 1000)
            {
                time1000ms = 0;
                if(time15s > 0) time15s--;
                if(time15s == 0)
                {
                    time15s = 15;
                    password_error = 0;
                    mode = 0;
                    show_flag = 0;
                    ui_message = UI_INPUT_PASSWORD;
                    ui_sub_message = UI_NONE;
                    key_clear();
                }
            }
        }

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

typedef struct
{
    void (*task_func)(void);
    unsigned long int rate_ms;
    unsigned long int last_run;
} task_t;

static task_t scheduler_task[] = {
    {lcd_proc,   100, 0},
    {key_proc,   10,  0},
    {lock_proc,  30,  0},
    {mesh_proc,  50,  0},
    {as608_proc, 20,  0},
};

static unsigned char task_num;

static void scheduler_init(void)
{
    task_num = sizeof(scheduler_task) / sizeof(task_t);
}

static void scheduler_run(void)
{
    unsigned char i;

    for(i = 0; i < task_num; i++)
    {
        unsigned long int now_time = uwtick;
        if(now_time >= scheduler_task[i].last_run + scheduler_task[i].rate_ms)
        {
            scheduler_task[i].last_run = now_time;
            scheduler_task[i].task_func();
        }
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    Delay_Init();

    WCHBLE_Init();
    HAL_Init();
    RF_RoleInit();
    RF_Init();
    lwns_init();
    lwns_mesh_process_init();

    TIM4_PWM_Init();
    lock(1);

    LCD_Init();
    lcd_show_startup_logo();
    Delay_Ms(2000);
    LCD_Fill(0, 0, 128, 128, WHITE);
    LCD_ShowPicture(0, 0, 128, 128, gImage_2);
    lcd_show_key_labels();
    lcd_show_message(0, UI_INPUT_PASSWORD);
    LCD_Fill(16, 45, 112, 66, YELLOW);

    key_init();
    Tim3_Init(1000, 96 - 1);
    scheduler_init();
    audio_init();
    audio_yinliang(20);
    Delay_Ms(10);
    audio_play(2);
    Usart2_Init();
    as608_init();

    while(1)
    {
        TMOS_SystemProcess();
        scheduler_run();
    }
}

void UART4_IRQHandler(void)
{
    u8 temp;

    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        ps_wait_flag = 0;
        if(uart4_rec_tick > 10) uart4_rec_index = 0;
        temp = USART_ReceiveData(UART4);
        if(uart4_rec_index < sizeof(uart4_rec_string))
        {
            uart4_rec_string[uart4_rec_index++] = temp;
        }
        else
        {
            uart4_rec_index = 0;
        }
        uart4_rec_tick = 0;
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }
}

void USART2_IRQHandler(void)
{
    u8 temp;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(USART2);
        switch(rfid_index)
        {
            case 0: if(temp == 0x04) rfid_index++; break;
            case 1: if(temp == 0x0c) rfid_index++; else rfid_index = 0; break;
            case 2: if(temp == 0x02) rfid_index++; else rfid_index = 0; break;
            case 3: if(temp == 0x30) rfid_index++; else rfid_index = 0; break;
            case 4: if(temp == 0x00) rfid_index++; else rfid_index = 0; break;
            case 5: if(temp == 0x04) rfid_index++; else rfid_index = 0; break;
            case 6: if(temp == 0x00) rfid_index++; else rfid_index = 0; break;
            case 7: rfid_temp[0] = temp; rfid_index++; break;
            case 8: rfid_temp[1] = temp; rfid_index++; break;
            case 9: rfid_temp[2] = temp; rfid_index++; break;
            case 10:
                rfid_temp[3] = temp;
                rfid_index = 0;
                if(mode == 0)
                {
                    if(rfid_check())
                    {
                        audio_play(11);
                        lock_flag = 0;
                        time5000ms = 0;
                        lock(0);
                        mesh_notify_door_open();
                        show_flag = 1;
                        ui_message = UI_DOOR_OPEN;
                        ui_sub_message = UI_NONE;
                        password_error = 0;
                    }
                    else
                    {
                        show_flag = 0;
                        ui_message = UI_CARD_FAILED;
                        ui_sub_message = UI_NONE;
                        ui_return_ms = 1500;
                        audio_play(12);
                    }
                }
                else if(mode == 4)
                {
                    if(rfid_password_index < 4)
                    {
                        string_copy(rfid_temp, rfid[rfid_password_index], 4);
                        rfid_password_index++;
                    }
                    audio_play(15);
                    mode = 0;
                    show_flag = 0;
                    ui_message = UI_SUCCESS;
                    ui_sub_message = UI_NONE;
                    ui_return_ms = 1500;
                    password_error = 0;
                    key_clear();
                }
                break;
            default:
                rfid_index = 0;
                break;
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        (void)USART_ReceiveData(USART3);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

static void key_clear(void)
{
    memset(key_temp, 10, sizeof(key_temp));
    key_index = 0;
    key_index_old = 0xff;
}

static u8 string_check(u8 *a, u8 *b, u8 len)
{
    u8 i;

    for(i = 0; i < len; i++)
    {
        if(a[i] != b[i]) return 0;
    }
    return 1;
}

static void string_copy(u8 *src, u8 *dst, u8 len)
{
    u8 i;

    for(i = 0; i < len; i++)
    {
        dst[i] = src[i];
    }
}

static u8 rfid_check(void)
{
    u8 i;

    for(i = 0; i < rfid_password_index; i++)
    {
        if(string_check(rfid_temp, rfid[i], 4)) return 1;
    }
    return 0;
}

static void Ps_Wait(void)
{
    u16 timeout = 0;

    ps_wait_flag = 1;
    while(ps_wait_flag && timeout < 50)
    {
        Delay_Ms(20);
        timeout++;
    }
}


