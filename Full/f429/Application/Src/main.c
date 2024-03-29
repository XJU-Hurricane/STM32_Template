/**
 * @file    main.c
 * @author  Deadline039
 * @brief   STM32F429新工程模板文件
 * @version 1.0
 * @date    2023-10-19
 */

#include "main.h"
#include "delay.h"
#include "dma_uart.h"
#include "key.h"
#include "led.h"
#include "uart.h"

/**
 * @brief 串口压力测试
 *
 */
void uart_stress(void) {
    static uint8_t buf[256];
    uint32_t len = 0;

    while (1) {
        uart_dmatx_send(&g_usart1_handle);
        len = uart_dmarx_read(&g_usart1_handle, buf, sizeof(buf));
        if (len > 0) {
            uart_dmatx_write(&g_usart1_handle, buf, len);
        }
    }
}

/**
 * @brief  主函数
 *
 * @return int
 */
int main(void) {
    bsp_init();

    LED0_TOGGLE();
    uint16_t times = 0;
    uint8_t key = 0;
    while (1) {
        key = key_scan(0);
        if (g_usart1_rx_status & 0x8000) {
            /* 得到此次接收到的数据长度 */
            uint16_t len = g_usart1_rx_status & (uint16_t)0x3fff;
            printf("您发送的消息为:\r\n\033[32m");
            HAL_UART_Transmit(&g_usart1_handle, (uint8_t *)g_usart1_rx_buf,
                              len, 1000); /* 发送接收到的数据 */
            while (__HAL_UART_GET_FLAG(&g_usart1_handle, UART_FLAG_TC) != SET)
                ;                   /* 等待发送结束 */
            printf("\033[39m\r\n"); /* 插入换行 */
            g_usart1_rx_status = 0;
        }

        if (key) {
            switch (key) {
                case KEY0_PRES: {
                    printf("KEY0按下\r\n");
                } break;
                case KEY1_PRES: {
                    printf("KEY1按下\r\n");
                } break;
                case KEY2_PRES: {
                    printf("KEY2按下\r\n");
                } break;
                case WKUP_PRES: {
                    printf("WK_UP按下\r\n");
                } break;
            }
        }
        if (times % 100 == 0) {
            times = 0;
            LED0_TOGGLE();
            LED1_TOGGLE();
            printf("请输入数据, 以回车键结束\r\n");
        }
        ++times;
        delay_ms(10);
    }
}

/**
 * @brief 板层驱动初始化
 * 
 */
void bsp_init(void) {
    HAL_Init();                          /* 初始化HAL库 */
    sys_stm32_clock_init(360, 25, 2, 8); /* 初始化时钟, 180MHz */
    delay_init(180);                     /* 初始化延时 */
    usart1_init(115200);                 /* 串口1初始化 */
    led_init();                          /* LED初始化 */
    key_init();                          /* 按键初始化 */
}
