/**
 * @file    uart.h
 * @date    2023-09-14
 * @author  Deadline039
 * @version 1.0
 * @note    STM32F103串口(usart1->uart5)的初始化函数以及中断服务函数
 *          如果启用了串口1,printf函数将会被重定义为从串口1输出
 *          根据自己的需要修改代码, 不一定非得这样写
 * @warning 要使用相关函数请预先在此文件里打开宏开关 若使用DMA到dma_uart.h
 *          中打开相关宏开关
 *          如果使用了DMA, 接收中的BUF,STA,aRxBuffer将不可用!!!
 *
 */

#ifndef __UART_H
#define __UART_H

#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "sys.h"

#define USART_REC_LEN 200 /* 定义USART最大接收字节数 200 */
#define TX_BUF_LEN    256 /* 定义发送缓冲区字节数 256 */
#define RXBUFFERSIZE  1   /* 接收缓冲区大小 */

/**
 * @brief 是否使用串口,启用就在此处define
 * @warning 未定义的串口,相关函数将无法使用,编译不通过!
 */

#define EN_USART1
// #define EN_USART2
// #define EN_USART3
// #define EN_UART4
// #define EN_UART5

/**
 * @brief 是否启用串口接收,不启用就将1改为0或者直接注释
 * @warning 未定义为1的串口,串口接收的变量无法使用!
 * @warning 如果串口未定义不能启用对应的串口中断!!!!!
 */

#define EN_USART1_RX 1
// #define EN_USART2_RX 1
// #define EN_USART3_RX 1
// #define EN_UART4_RX  1
// #define EN_UART5_RX  1

/*******************************************************************************
 * @defgroup 串口1
 * @{
 */

#ifdef EN_USART1
/* 句柄 */
extern UART_HandleTypeDef USART1_Handler;
    /* 接收buf */
    #if EN_USART1_RX
extern uint8_t USART1_RX_BUF[USART_REC_LEN];
extern uint16_t USART1_RX_STA;
extern uint8_t g_usart1_rx_buf[RXBUFFERSIZE];
    #endif /* EN_USART1_RX */

void USART1_Init(uint32_t bound);
#endif /* EN_USART1 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口2
 * @{
 */

#ifdef EN_USART2
/* 句柄 */
extern UART_HandleTypeDef USART2_Handler;
    /* 接收buf */
    #if EN_USART2_RX
extern uint8_t USART2_RX_BUF[USART_REC_LEN];
extern uint16_t USART2_RX_STA;
extern uint8_t g_usart2_rx_buf[RXBUFFERSIZE];
    #endif /* EN_USART2 */

void USART2_Init(uint32_t bound);
#endif /* EN_USART2 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口3
 * @{
 */

#ifdef EN_USART3
/* 句柄 */
extern UART_HandleTypeDef USART3_Handler;
    /* 接收buf */
    #if EN_USART3_RX
extern uint8_t USART3_RX_BUF[USART_REC_LEN];
extern uint16_t USART3_RX_STA;
extern uint8_t g_usart3_rx_buf[RXBUFFERSIZE];
    #endif /* EN_USART3_RX */
void USART3_Init(uint32_t bound);
#endif /* EN_USART3 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口4
 * @{
 */

#ifdef EN_UART4
/* 句柄 */
extern UART_HandleTypeDef UART4_Handler;
    /* 接收buf */
    #if EN_UART4_RX
extern uint8_t UART4_RX_BUF[USART_REC_LEN];
extern uint16_t UART4_RX_STA;
extern uint8_t g_uart4_rx_buf[RXBUFFERSIZE];
    #endif /* EN_UART4_RX */

void UART4_Init(uint32_t bound);
#endif /* EN_UART4 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口5
 * @{
 */

#ifdef EN_UART5
/* 句柄 */
extern UART_HandleTypeDef UART5_Handler;
    /* 接收buf */
    #if EN_UART5_RX
extern uint8_t UART5_RX_BUF[USART_REC_LEN];
extern uint16_t UART5_RX_STA;
extern uint8_t g_uart5_rx_buf[RXBUFFERSIZE];
    #endif /* EN_UART5_RX */

void UART5_Init(uint32_t bound);
#endif /* EN_UART5 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 公共处理函数
 * @{
 */

void uart_print(UART_HandleTypeDef *huart, const char *__format, ...);

/**
 * @}
 */

#endif /* __UART_H */
