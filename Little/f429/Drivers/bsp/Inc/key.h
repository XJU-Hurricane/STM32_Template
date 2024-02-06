/**
 * @file    key.h
 * @author  Deadline039
 * @brief   按键检测及初始化
 * @version 1.0
 * @date    2023-10-20
 */

#ifndef __KEY_H
#define __KEY_H

#include "delay.h"
#include "sys.h"

/* KEY0定义 */
#define KEY0_PORT     GPIOH
#define KEY0_ENABLE() __HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY0_PIN      GPIO_PIN_3

/* KEY1定义 */
#define KEY1_PORT     GPIOH
#define KEY1_ENABLE() __HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY1_PIN      GPIO_PIN_2

/* KEY2定义 */
#define KEY2_PORT     GPIOC
#define KEY2_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define KEY2_PIN      GPIO_PIN_13

/* WK_UP定义 */
#define WKUP_PORT     GPIOA
#define WKUP_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define WKUP_PIN      GPIO_PIN_0

/* 检测按键按下 */
#define KEY0          HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN)
#define KEY1          HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)
#define KEY2          HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN)
#define WK_UP         HAL_GPIO_ReadPin(WKUP_PORT, WKUP_PIN)

#define KEY0_PRES     1
#define KEY1_PRES     2
#define KEY2_PRES     3
#define WKUP_PRES     4

void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif /* __KEY_H */
