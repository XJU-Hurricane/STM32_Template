/**
 * @file    key.h
 * @author  Deadline039
 * @brief   按键驱动以及检测代码头文件
 * @version 1.0
 * @date    2023-10-20
 */
#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

/* KEY0定义 */
#define KEY0_PORT     GPIOC
#define KEY0_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define KEY0_PIN      GPIO_PIN_5

/* KEY1定义 */
#define KEY1_PORT     GPIOA
#define KEY1_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY1_PIN      GPIO_PIN_15

/* WK_UP定义 */
#define WKUP_PORT     GPIOA
#define WKUP_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define WKUP_PIN      GPIO_PIN_0

/* 检测按键按下 */
#define KEY0          HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN)
#define KEY1          HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)
#define WK_UP         HAL_GPIO_ReadPin(WKUP_PORT, WKUP_PIN)

#define KEY0_PRES     1
#define KEY1_PRES     2
#define WKUP_PRES     3

void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif /* __KEY_H */
