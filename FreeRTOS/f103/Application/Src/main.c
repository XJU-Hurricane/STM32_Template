/**
 * @file    main.c
 * @author  Deadline039
 * @brief   STM32F103 FreeRTOS新工程模板文件
 * @version 1.0
 * @date    2023-09-18
 */

#include "main.h"
#include "delay.h"
#include "dma_uart.h"
#include "key.h"
#include "led.h"
#include "rtos_tasks.h"
#include "uart.h"

/**
 * @brief 主函数
 *
 * @return int
 */
int main(void) {
    bsp_init();
    freertos_start();
}

/**
 * @brief 板层驱动初始化
 *
 */
void bsp_init(void) {
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟,72M */
    delay_init(72);                     /* 初始化延时 */
    usart1_init(115200);                /* 串口1初始化 */
    led_init();                         /* LED初始化 */
    key_init();                         /* 按键初始化 */
}
