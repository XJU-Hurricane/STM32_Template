/**
 * @file    led.c
 * @author  Deadline039
 * @brief   LED灯
 * @version 1.0
 * @date    2023-10-07
 */

#include "led.h"

/**
 * @brief LED初始化
 *
 */
void led_init(void) {
    GPIO_InitTypeDef GPIO_Initure = {0};

    LED0_ENABLE();
    LED1_ENABLE();

    GPIO_Initure.Pin = LED0_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED0_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = LED1_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED1_PORT, &GPIO_Initure);

    /* 关闭LED */
    LED0(1);
    LED1(1);
}
