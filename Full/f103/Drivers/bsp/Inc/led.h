/**
 * @file    led.h
 * @author  Deadline039
 * @brief   LED灯
 * @version 1.0
 * @date    2023-10-07
 */

#ifndef __LED_H
#define __LED_H

#include "sys.h"

/* LED0定义 */
#define LED0_PORT     GPIOA
#define LED0_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED0_PIN      GPIO_PIN_8

/* LED1定义 */
#define LED1_PORT     GPIOD
#define LED1_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED1_PIN      GPIO_PIN_2

/* 控制两个LED */
#define LED0(x)                                                                \
    (x) ? HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_SET)                 \
        : HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_RESET)
#define LED1(x)                                                                \
    (x) ? HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET)                 \
        : HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET)

#define LED0_TOGGLE() HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN)
#define LED1_TOGGLE() HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN)

void LED_Init(void);

#endif /* __LED_H */
