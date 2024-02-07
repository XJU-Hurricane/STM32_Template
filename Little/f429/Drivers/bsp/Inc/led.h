/**
 * @file    led.h
 * @author  Deadline039
 * @brief   LED驱动代码
 * @version 1.0
 * @date    2023-10-20
 */

#ifndef __LED_H
#define __LED_H

#include "stdarg.h"
#include "string.h"
#include "sys.h"

/* LED0定义 */
#define LED0_PORT     GPIOB
#define LED0_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED0_PIN      GPIO_PIN_1

/* LED1定义 */
#define LED1_PORT     GPIOB
#define LED1_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_PIN      GPIO_PIN_0

/* 控制两个LED */
#define LED0(x)                                                                \
    x ? HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_SET)                   \
      : HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_RESET) /* LED0 */
#define LED1(x)                                                                \
    x ? HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET)                   \
      : HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET) /* LED1 */

#define LED0_TOGGLE() HAL_GPIO_TogglePin(LED0_PORT, LED0_PIN) /* 翻转LED0 */
#define LED1_TOGGLE() HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN) /* 翻转LED1 */

void led_init(void);

#endif /* __LED_H */
