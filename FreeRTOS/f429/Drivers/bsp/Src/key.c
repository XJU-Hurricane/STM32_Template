/**
 * @file    key.c
 * @author  Deadline039
 * @brief   按键检测及初始化
 * @version 1.0
 * @date    2023-10-20
 */

#include "delay.h"
#include "key.h"

/**
 * @brief 初始化按键
 *
 */
void key_init(void) {
    GPIO_InitTypeDef GPIO_Initure = {0};

    KEY0_ENABLE();
    KEY1_ENABLE();
    KEY2_ENABLE();
    WKUP_ENABLE();

    GPIO_Initure.Pin = KEY0_PIN;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY0_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = KEY1_PIN;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY1_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = KEY2_PIN;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY2_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = WKUP_PIN;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_PORT, &GPIO_Initure);
}

/**
 * @brief 按键扫描
 * @param mode 是否支持连按
 *  @arg 0-不支持连按, 1-支持连按
 * @return 按下的按键
 *  @retval 按下:KEY0_PRES ~ WKUP_PRES
 *  @retval 未按下:0
 * @note 注意此函数有响应优先级,KEY0 > KEY1 > KEY2 > WK_UP
 */
uint8_t key_scan(uint8_t mode) {
    static uint8_t key_up = 1; /* 按键按松开标志 */
    uint8_t keyval = 0;

    if (mode) {
        key_up = 1; /* 支持连按 */
    }

    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1)) {
        delay_ms(10); /* 去抖动 */
        key_up = 0;
        if (KEY0 == 0) {
            keyval = KEY0_PRES;
        }
        if (KEY1 == 0) {
            keyval = KEY1_PRES;
        }
        if (KEY2 == 0) {
            keyval = KEY2_PRES;
        }
        if (WK_UP == 1) {
            keyval = WKUP_PRES;
        }
    } else if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0) {
        /* 没有任何按键按下, 标记按键松开 */
        key_up = 1;
    }

    return keyval; /* 返回键值 */
}
