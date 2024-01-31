/**
 * @file    dma_uart.h
 * @author  Deadline039
 * @brief   使用DMA+半满中断+满中断+空闲中断实现高可靠串口数据收发
 * @version 1.0
 * @date    2024-01-21
 * @note    stm32f429串口DMA配置文件
 *          除f429和f439外, 串口7和串口8无DMA请求
 *          使用内存环形FIFO, 不使用硬件FIFO
 *          打开对应的宏开关就可以使用DMA传输, 无需配置其他东西
 * @warning UART7和UART8的DMA会与其他串口冲突, 具体参照STM32F4xx的手册
 *          除非必要, 否则不建议使用UART7和UART8的DMA
 */

#ifndef __DMA_UART_H
#define __DMA_UART_H

#include "ring_fifo.h"
#include "stdlib.h"
#include "sys.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 串口发送FIFO
 *
 */
typedef struct {
    ring_fifo_t *tx_fifo;   /*!< 发送FIFO */
    uint8_t *tx_fifo_buf;   /*!< FIFO数据存储区 */
    uint8_t *send_buf;      /*!< 发送数据缓冲区 */
    uint32_t send_buf_size; /*!< 缓冲区大小 */
    __IO uint32_t tc_flag;  /*!< 是否发送完成, 0-未完成; 1-完成 */
} uart_tx_fifo_t;

/**
 * @brief 串口接收FIFO
 *
 */
typedef struct {
    ring_fifo_t *rx_fifo; /*!< 接收FIFO */
    uint8_t *rx_fifo_buf; /*!< FIFO数据存储区 */
    uint32_t head_ptr;    /*!< 位置指针, 用来控制半满和溢出 */
} uart_rx_fifo_t;

/*******************************************************************************
 * @defgroup 串口1 DMA
 * @{
 */

#ifdef EN_USART1
    /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
    #define USART1_USE_DMA_TX 0
    #define USART1_USE_DMA_RX 0

    /* 下面的代码按需改动 */

    #if USART1_USE_DMA_TX
void USART1_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART1_USE_DMA_TX */

    #if USART1_USE_DMA_RX
        #if !EN_USART1_RX
            #error Accept must be enabled USART1 receive first to use DMA receive.
        #endif /* !EN_USART1_RX */

        #define USART1_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void USART1_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART1_USE_DMA_RX */

#endif /* EN_USART1 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口2 DMA
 * @{
 */

#ifdef EN_USART2
    /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
    #define USART2_USE_DMA_TX 0
    #define USART2_USE_DMA_RX 0

    /* 下面的代码按需改动 */

    #if USART2_USE_DMA_TX
void USART2_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART2_USE_DMA_TX */

    #if USART2_USE_DMA_RX
        #if !EN_USART2_RX
            #error Accept must be enabled USART2 receive first to use DMA receive.
        #endif /* !EN_USART2_RX */

        #define USART2_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void USART2_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART2_USE_DMA_RX */

#endif /* EN_USART2 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口3 DMA
 * @{
 */

#ifdef EN_USART3
    /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
    #define USART3_USE_DMA_TX 0
    #define USART3_USE_DMA_RX 0

    /* 下面的代码按需改动 */

    #if USART3_USE_DMA_TX
void USART3_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART3_USE_DMA_TX */

    #if USART3_USE_DMA_RX
        #if !EN_USART3_RX
            #error Accept must be enabled USART3 receive first to use DMA receive.
        #endif /* !EN_USART3_RX */

        #define USART3_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void USART3_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART3_USE_DMA_RX */

#endif /* EN_USART3 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口4 DMA
 * @{
 */

#ifdef EN_UART4
    /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
    #define UART4_USE_DMA_TX 0
    #define UART4_USE_DMA_RX 0

    /* 下面的代码按需改动 */

    #if UART4_USE_DMA_TX
void UART4_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* UART4_USE_DMA_TX */

    #if UART4_USE_DMA_RX
        #if !EN_UART4_RX
            #error Accept must be enabled UART4 receive first to use DMA receive.
        #endif /* !EN_UART4_RX */

        #define UART4_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void UART4_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* UART4_USE_DMA_RX */

#endif /* EN_UART4 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口5 DMA
 * @{
 */

#ifdef EN_UART5
    /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
    #define UART5_USE_DMA_TX 0
    #define UART5_USE_DMA_RX 0

    /* 下面的代码按需改动 */

    #if UART5_USE_DMA_TX
void UART5_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* UART5_USE_DMA_TX */

    #if UART5_USE_DMA_RX
        #if !EN_UART5_RX
            #error Accept must be enabled UART5 receive first to use DMA receive.
        #endif /* !EN_UART5_RX */

        #define UART5_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void UART5_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* UART5_USE_DMA_RX */

#endif /* EN_UART5 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口6 DMA
 * @{
 */

#ifdef EN_USART6
    /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
    #define USART6_USE_DMA_TX 0
    #define USART6_USE_DMA_RX 0

    /* 下面的代码按需改动 */

    #if USART6_USE_DMA_TX
void USART6_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART6_USE_DMA_TX */

    #if USART6_USE_DMA_RX
        #if !EN_USART6_RX
            #error Accept must be enabled USART6 receive first to use DMA receive.
        #endif /* !EN_USART6_RX */

        #define USART6_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void USART6_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
    #endif /* USART6_USE_DMA_RX */

#endif /* EN_USART6 */

/**
 * @}
 */

#if (defined(STM32F429xx) || defined(STM32F439xx))

    /*******************************************************************************
     * @defgroup 串口7 DMA
     * @{
     */

    #ifdef EN_UART7
        /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
        #define UART7_USE_DMA_TX 0
        #define UART7_USE_DMA_RX 0

        /* 下面的代码按需改动 */

        #if UART7_USE_DMA_TX
void UART7_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
        #endif /* UART7_USE_DMA_TX */

        #if UART7_USE_DMA_RX
            #if !EN_UART7_RX
                #error Accept must be enabled UART7 receive first to use DMA receive.
            #endif /* !EN_UART7_RX */

            #define UART7_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void UART7_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
        #endif /* UART7_USE_DMA_RX */

    #endif /* EN_UART7 */

    /**
     * @}
     */

    /*******************************************************************************
     * @defgroup 串口8 DMA
     * @{
     */

    #ifdef EN_UART8
        /* 只需要启用宏即可打开DMA传输, 不需要改动代码 */
        #define UART8_USE_DMA_TX 0
        #define UART8_USE_DMA_RX 0

        /* 下面的代码按需改动 */

        #if UART8_USE_DMA_TX
void UART8_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
        #endif /* UART8_USE_DMA_TX */

        #if UART8_USE_DMA_RX
            #if !EN_UART8_RX
                #error Accept must be enabled UART8 receive first to use DMA receive.
            #endif /* !EN_UART8_RX */

            #define UART8_USE_IDLE_IT 1 /* 是否启用空闲中断 */
void UART8_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size);
        #endif /* UART8_USE_DMA_RX */

    #endif /* EN_UART8 */

    /**
     * @}
     */

#endif /* (defined(STM32F429xx) || defined(STM32F439xx))  */

/*******************************************************************************
 * @defgroup 公共处理函数
 * @{
 */
void UART_DMATX_Clear_TC_Flag(UART_HandleTypeDef *huart);
void UART_DMATX_Write(UART_HandleTypeDef *huart, const void *data,
                      uint32_t len);
void UART_DMATX_Send(UART_HandleTypeDef *huart);

void UART_DMARX_Idle_Callback(UART_HandleTypeDef *huart);
void UART_DMARX_HalfDone_Callback(UART_HandleTypeDef *huart);
void UART_DMARX_Done_Callback(UART_HandleTypeDef *huart);
uint32_t UART_DMARX_Read(UART_HandleTypeDef *huart, void *buf, uint32_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DMA_UART_H */