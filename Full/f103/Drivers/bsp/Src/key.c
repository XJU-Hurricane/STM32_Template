/**
 * @file    key.c
 * @author  Deadline039
 * @brief   按键初始化以及检测代码
 * @version 1.0
 * @date    2023-10-20
 */

#include "key.h"
#include "delay.h"

/**
 * @brief 按键初始化函数
 *
 */
void key_init(void) {
    GPIO_InitTypeDef gpio_initure = {0};

    KEY0_ENABLE();
    KEY1_ENABLE();
    WKUP_ENABLE();

    gpio_initure.Pin = KEY0_PIN;
    gpio_initure.Mode = GPIO_MODE_INPUT;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY0_PORT, &gpio_initure);

    gpio_initure.Pin = KEY1_PIN;
    gpio_initure.Mode = GPIO_MODE_INPUT;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY1_PORT, &gpio_initure);

    gpio_initure.Pin = WKUP_PIN;
    gpio_initure.Mode = GPIO_MODE_INPUT;
    gpio_initure.Pull = GPIO_PULLDOWN;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_PORT, &gpio_initure);
}

/**
 * @brief 按键扫描
 *
 * @param mode 是否支持连按
 *  0-不支持连按, 1-支持连按
 * @return 按下的按键
 *  @retval 按下:`KEY0_PRES ~ WKUP_PRES`
 *  @retval 未按下:`0`
 * @note 注意此函数有响应优先级,`KEY0 > KEY1 > WK_UP`
 */
uint8_t key_scan(uint8_t mode) {
    static uint8_t key_up = 1; /* 按键松开标志 */
    if (mode == 1) {
        key_up = 1; /* 支持连按 */
    }
    if (key_up && (KEY0 == 0 || KEY1 == 0 || WK_UP == 1)) {
        delay_ms(10);
        key_up = 0;
        if (KEY0 == 0) {
            return KEY0_PRES;
        } else if (KEY1 == 0) {
            return KEY1_PRES;
        } else if (WK_UP == 1) {
            return WKUP_PRES;
        }
    } else if (KEY0 == 1 && KEY1 == 1 && WK_UP == 0) {
        key_up = 1;
    }
    return 0; /* 无按键按下 */
}
