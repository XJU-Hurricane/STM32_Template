/**
 * @file    dma_uart.c
 * @author  Deadline039
 * @brief   使用DMA+半满中断+满中断+空闲中断实现高可靠串口数据收发
 * @version 1.0
 * @date    2024-01-21
 * @note    不使用硬件FIFO, 仅有4字节大小不满足要求
 * @see     https://github.com/Prry/stm32-uart-dma
 * @see     https://gitee.com/wei513723/stm32-stable-uart-transmit-receive
 */

#include "dma_uart.h"
#include "stdlib.h"
#include "sys.h"
#include "uart.h"

/*******************************************************************************
 * @defgroup 串口1 DMA
 * @{
 */

#ifdef EN_USART1

#if USART1_USE_DMA_TX

static DMA_HandleTypeDef g_usart1_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_usart1_tx_fifo = {0};
static uint8_t *g_usart1_send_buf;

/**
 * @brief 串口1 DMA发送初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void usart1_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart1_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_usart1_tx_fifo.send_buf = g_usart1_send_buf;
    g_usart1_tx_fifo.send_buf_size = buf_size;
    g_usart1_tx_fifo.tc_flag = 1;
    g_usart1_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_usart1_tx_fifo.tx_fifo =
        ring_fifo_init(g_usart1_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_usart1_tx_fifo.tx_fifo_buf == NULL) {
        printf("USART1 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA2_CLK_ENABLE();

    g_usart1_dmatx_handle.Instance = DMA2_Stream7;
    g_usart1_dmatx_handle.Init.Channel = DMA_CHANNEL_4;
    g_usart1_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_usart1_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart1_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart1_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_usart1_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart1_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart1_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_usart1_dmatx_handle);
    __HAL_LINKDMA(&g_usart1_handle, hdmatx, g_usart1_dmatx_handle);

    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

/**
 * @brief 串口1发送中断句柄
 *
 */
void DMA2_Stream7_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart1_dmatx_handle);
}
#endif /* USART1_USE_DMA_TX */

#if USART1_USE_DMA_RX

static DMA_HandleTypeDef g_usart1_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart1_rx_fifo = {0};
static uint8_t *g_usart1_recv_buf;

/**
 * @brief 串口1接收DMA初始化
 *
 * @param fifo_size FIFO大小
 * @param buf_size 缓冲区大小
 */
void usart1_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart1_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_usart1_rx_fifo.head_ptr = 0;
    g_usart1_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_usart1_rx_fifo.rx_fifo =
        ring_fifo_init(g_usart1_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_usart1_rx_fifo.rx_fifo == NULL) {
        printf("USART1 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA2_CLK_ENABLE();

    g_usart1_dmarx_handle.Instance = DMA2_Stream5;
    g_usart1_dmarx_handle.Init.Channel = DMA_CHANNEL_4;
    g_usart1_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_usart1_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart1_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart1_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_usart1_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart1_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart1_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_usart1_dmarx_handle);
    __HAL_LINKDMA(&g_usart1_handle, hdmarx, g_usart1_dmarx_handle);

    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

#if USART1_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_usart1_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_usart1_handle);
#endif /* USART1_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_usart1_handle, (uint8_t *)g_usart1_recv_buf,
                         buf_size);
}

/**
 * @brief 串口1接收中断句柄
 *
 */
void DMA2_Stream5_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart1_dmarx_handle);
}
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

#if USART2_USE_DMA_TX

static DMA_HandleTypeDef g_usart2_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_usart2_tx_fifo = {0};
static uint8_t *g_usart2_send_buf;

/**
 * @brief 串口2发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void usart2_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart2_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_usart2_tx_fifo.send_buf = g_usart2_send_buf;
    g_usart2_tx_fifo.send_buf_size = buf_size;
    g_usart2_tx_fifo.tc_flag = 1;
    g_usart2_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_usart2_tx_fifo.tx_fifo =
        ring_fifo_init(g_usart2_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_usart2_tx_fifo.tx_fifo_buf == NULL) {
        printf("USART2 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();
    g_usart2_dmatx_handle.Instance = DMA1_Stream6;
    g_usart2_dmatx_handle.Init.Channel = DMA_CHANNEL_4;
    g_usart2_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_usart2_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart2_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart2_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_usart2_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart2_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart2_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_usart2_dmatx_handle);
    __HAL_LINKDMA(&g_usart2_handle, hdmatx, g_usart2_dmatx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

/**
 * @brief 串口2发送中断句柄
 *
 */
void DMA1_Stream6_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart2_dmatx_handle);
}
#endif /* USART2_USE_DMA_TX */

#if USART2_USE_DMA_RX

static DMA_HandleTypeDef g_usart2_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart2_rx_fifo = {0};
static uint8_t *g_usart2_recv_buf;

/**
 * @brief 串口2接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void usart2_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart2_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_usart2_rx_fifo.head_ptr = 0;
    g_usart2_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_usart2_rx_fifo.rx_fifo =
        ring_fifo_init(g_usart2_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_usart2_rx_fifo.rx_fifo == NULL) {
        printf("USART2 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();

    g_usart2_dmarx_handle.Instance = DMA1_Stream5;
    g_usart2_dmarx_handle.Init.Channel = DMA_CHANNEL_4;
    g_usart2_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_usart2_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart2_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart2_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_usart2_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart2_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart2_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_usart2_dmarx_handle);
    __HAL_LINKDMA(&g_usart2_handle, hdmarx, g_usart2_dmarx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

#if USART2_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_usart2_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_usart2_handle);
#endif /* USART2_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_usart2_handle, (uint8_t *)g_usart2_recv_buf,
                         buf_size);
}

/**
 * @brief 串口2接收中断句柄
 *
 */
void DMA1_Stream5_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart2_dmarx_handle);
}
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

#if USART3_USE_DMA_TX

static DMA_HandleTypeDef g_usart3_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_usart3_tx_fifo = {0};
static uint8_t *g_usart3_send_buf;

/**
 * @brief 串口3发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void usart3_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart3_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_usart3_tx_fifo.send_buf = g_usart3_send_buf;
    g_usart3_tx_fifo.send_buf_size = buf_size;
    g_usart3_tx_fifo.tc_flag = 1;
    g_usart3_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_usart3_tx_fifo.tx_fifo =
        ring_fifo_init(g_usart3_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_usart3_tx_fifo.tx_fifo_buf == NULL) {
        printf("USART3 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();
    g_usart3_dmatx_handle.Instance = DMA1_Stream3;
    g_usart3_dmatx_handle.Init.Channel = DMA_CHANNEL_4;
    g_usart3_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_usart3_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart3_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart3_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_usart3_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart3_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart3_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_usart3_dmatx_handle);
    __HAL_LINKDMA(&g_usart3_handle, hdmatx, g_usart3_dmatx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
}

/**
 * @brief 串口3发送中断句柄
 *
 */
void DMA1_Stream3_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart3_dmatx_handle);
}
#endif /* USART3_USE_DMA_TX */

#if USART3_USE_DMA_RX

static DMA_HandleTypeDef g_usart3_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart3_rx_fifo = {0};
static uint8_t *g_usart3_recv_buf;

/**
 * @brief 串口3接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void usart3_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart3_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_usart3_rx_fifo.head_ptr = 0;
    g_usart3_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_usart3_rx_fifo.rx_fifo =
        ring_fifo_init(g_usart3_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_usart3_rx_fifo.rx_fifo == NULL) {
        printf("USART3 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();

    g_usart3_dmarx_handle.Instance = DMA1_Stream1;
    g_usart3_dmarx_handle.Init.Channel = DMA_CHANNEL_4;
    g_usart3_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_usart3_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart3_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart3_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_usart3_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart3_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart3_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_usart3_dmarx_handle);
    __HAL_LINKDMA(&g_usart3_handle, hdmarx, g_usart3_dmarx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

#if USART3_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_usart3_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_usart3_handle);
#endif /* USART3_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_usart3_handle, (uint8_t *)g_usart3_recv_buf,
                         buf_size);
}

/**
 * @brief 串口3接收中断句柄
 *
 */
void DMA1_Stream1_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart3_dmarx_handle);
}
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

#if UART4_USE_DMA_TX

static DMA_HandleTypeDef g_uart4_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_uart4_tx_fifo = {0};
static uint8_t *g_uart4_send_buf;

/**
 * @brief 串口4发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void uart4_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart4_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_uart4_tx_fifo.send_buf = g_uart4_send_buf;
    g_uart4_tx_fifo.send_buf_size = buf_size;
    g_uart4_tx_fifo.tc_flag = 1;
    g_uart4_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_uart4_tx_fifo.tx_fifo =
        ring_fifo_init(g_uart4_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_uart4_tx_fifo.tx_fifo_buf == NULL) {
        printf("UART4 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();
    g_uart4_dmatx_handle.Instance = DMA1_Stream4;
    g_uart4_dmatx_handle.Init.Channel = DMA_CHANNEL_4;
    g_uart4_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_uart4_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart4_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart4_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_uart4_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart4_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart4_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_uart4_dmatx_handle);
    __HAL_LINKDMA(&g_uart4_handle, hdmatx, g_uart4_dmatx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}

/**
 * @brief 串口4发送中断句柄
 *
 */
void DMA1_Stream4_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart4_dmatx_handle);
}
#endif /* UART4_USE_DMA_TX */

#if UART4_USE_DMA_RX
static DMA_HandleTypeDef g_uart4_dmarx_handle = {0};

/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_uart4_rx_fifo = {0};
static uint8_t *g_uart4_recv_buf;

/**
 * @brief 串口4接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void uart4_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart4_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_uart4_rx_fifo.head_ptr = 0;
    g_uart4_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_uart4_rx_fifo.rx_fifo =
        ring_fifo_init(g_uart4_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_uart4_rx_fifo.rx_fifo == NULL) {
        printf("UART4 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();

    g_uart4_dmarx_handle.Instance = DMA1_Stream2;
    g_uart4_dmarx_handle.Init.Channel = DMA_CHANNEL_4;
    g_uart4_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_uart4_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart4_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart4_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_uart4_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart4_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart4_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_uart4_dmarx_handle);
    __HAL_LINKDMA(&g_uart4_handle, hdmarx, g_uart4_dmarx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

#if UART4_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_uart4_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_uart4_handle);
#endif /* UART4_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_uart4_handle, (uint8_t *)g_uart4_recv_buf, buf_size);
}

/**
 * @brief 串口4接收中断句柄
 *
 */
void DMA1_Stream2_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart4_dmarx_handle);
}
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

#if UART5_USE_DMA_TX

static DMA_HandleTypeDef g_uart5_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_uart5_tx_fifo = {0};
static uint8_t *g_uart5_send_buf;

/**
 * @brief 串口5发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void uart5_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart5_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_uart5_tx_fifo.send_buf = g_uart5_send_buf;
    g_uart5_tx_fifo.send_buf_size = buf_size;
    g_uart5_tx_fifo.tc_flag = 1;
    g_uart5_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_uart5_tx_fifo.tx_fifo =
        ring_fifo_init(g_uart5_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_uart5_tx_fifo.tx_fifo_buf == NULL) {
        printf("UART5 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();
    g_uart5_dmatx_handle.Instance = DMA1_Stream7;
    g_uart5_dmatx_handle.Init.Channel = DMA_CHANNEL_4;
    g_uart5_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_uart5_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart5_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart5_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_uart5_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart5_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart5_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_uart5_dmatx_handle);
    __HAL_LINKDMA(&g_uart5_handle, hdmatx, g_uart5_dmatx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
}

/**
 * @brief 串口5发送中断句柄
 *
 */
void DMA1_Stream7_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart5_dmatx_handle);
}
#endif /* UART5_USE_DMA_TX */

#if UART5_USE_DMA_RX

static DMA_HandleTypeDef g_uart5_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_uart5_rx_fifo = {0};
static uint8_t *g_uart5_recv_buf;

/**
 * @brief 串口5接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void uart5_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart5_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_uart5_rx_fifo.head_ptr = 0;
    g_uart5_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_uart5_rx_fifo.rx_fifo =
        ring_fifo_init(g_uart5_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_uart5_rx_fifo.rx_fifo == NULL) {
        printf("UART5 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();

    g_uart5_dmarx_handle.Instance = DMA1_Stream0;
    g_uart5_dmarx_handle.Init.Channel = DMA_CHANNEL_4;
    g_uart5_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_uart5_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart5_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart5_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_uart5_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart5_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart5_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_uart5_dmarx_handle);
    __HAL_LINKDMA(&g_uart5_handle, hdmarx, g_uart5_dmarx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

#if UART5_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_uart5_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_uart5_handle);
#endif /* UART5_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_uart5_handle, (uint8_t *)g_uart5_recv_buf, buf_size);
}

/**
 * @brief 串口5接收中断句柄
 *
 */
void DMA1_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart5_dmarx_handle);
}
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

static DMA_HandleTypeDef g_usart6_dmatx_handle = {0};
static uart_tx_fifo_t g_usart6_tx_fifo = {0};
static uint8_t *g_usart6_send_buf;

#if USART6_USE_DMA_TX

/**
 * @brief 串口6 DMA发送初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void usart6_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart6_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_usart6_tx_fifo.send_buf = g_usart6_send_buf;
    g_usart6_tx_fifo.send_buf_size = buf_size;
    g_usart6_tx_fifo.tc_flag = 1;
    g_usart6_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_usart6_tx_fifo.tx_fifo =
        ring_fifo_init(g_usart6_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_usart6_tx_fifo.tx_fifo_buf == NULL) {
        printf("USART6 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA2_CLK_ENABLE();

    g_usart6_dmatx_handle.Instance = DMA2_Stream6;
    g_usart6_dmatx_handle.Init.Channel = DMA_CHANNEL_5;
    g_usart6_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_usart6_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart6_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart6_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_usart6_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart6_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart6_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_usart6_dmatx_handle);
    __HAL_LINKDMA(&g_usart6_handle, hdmatx, g_usart6_dmatx_handle);

    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
}

/**
 * @brief 串口6发送中断句柄
 *
 */
void DMA2_Stream6_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart6_dmatx_handle);
}
#endif /* USART6_USE_DMA_TX */

#if USART6_USE_DMA_RX

static DMA_HandleTypeDef g_usart6_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart6_rx_fifo = {0};
static uint8_t *g_usart6_recv_buf;

/**
 * @brief 串口6接收DMA初始化
 *
 * @param fifo_size FIFO大小
 * @param buf_size 缓冲区大小
 */
void usart6_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_usart6_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_usart6_rx_fifo.head_ptr = 0;
    g_usart6_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_usart6_rx_fifo.rx_fifo =
        ring_fifo_init(g_usart6_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_usart6_rx_fifo.rx_fifo == NULL) {
        printf("USART6 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA2_CLK_ENABLE();

    g_usart6_dmarx_handle.Instance = DMA2_Stream1;
    g_usart6_dmarx_handle.Init.Channel = DMA_CHANNEL_5;
    g_usart6_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_usart6_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_usart6_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_usart6_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_usart6_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_usart6_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_usart6_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_usart6_dmarx_handle);
    __HAL_LINKDMA(&g_usart6_handle, hdmarx, g_usart6_dmarx_handle);

    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

#if USART6_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_usart6_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_usart6_handle);
#endif /* USART6_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_usart6_handle, (uint8_t *)g_usart6_recv_buf,
                         buf_size);
}

/**
 * @brief 串口6接收中断句柄
 *
 */
void DMA2_Stream1_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_usart6_dmarx_handle);
}
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

#if UART7_USE_DMA_TX

static DMA_HandleTypeDef g_uart7_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_uart7_tx_fifo = {0};
static uint8_t *g_uart7_send_buf;

/**
 * @brief 串口7发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void uart7_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart7_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_uart7_tx_fifo.send_buf = g_uart7_send_buf;
    g_uart7_tx_fifo.send_buf_size = buf_size;
    g_uart7_tx_fifo.tc_flag = 1;
    g_uart7_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_uart7_tx_fifo.tx_fifo =
        ring_fifo_init(g_uart7_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_uart7_tx_fifo.tx_fifo_buf == NULL) {
        printf("UART7 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();
    g_uart7_dmatx_handle.Instance = DMA1_Stream1;
    g_uart7_dmatx_handle.Init.Channel = DMA_CHANNEL_5;
    g_uart7_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_uart7_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart7_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart7_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_uart7_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart7_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart7_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_uart7_dmatx_handle);
    __HAL_LINKDMA(&g_uart7_handle, hdmatx, g_uart7_dmatx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

/**
 * @brief 串口7发送中断句柄
 *
 */
void DMA1_Stream1_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart7_dmatx_handle);
}
#endif /* UART7_USE_DMA_TX */

#if UART7_USE_DMA_RX

static DMA_HandleTypeDef g_uart7_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_uart7_rx_fifo = {0};
static uint8_t *g_uart7_recv_buf;

/**
 * @brief 串口7接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void uart7_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart7_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_uart7_rx_fifo.head_ptr = 0;
    g_uart7_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_uart7_rx_fifo.rx_fifo =
        ring_fifo_init(g_uart7_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_uart7_rx_fifo.rx_fifo == NULL) {
        printf("UART7 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();

    g_uart7_dmarx_handle.Instance = DMA1_Stream3;
    g_uart7_dmarx_handle.Init.Channel = DMA_CHANNEL_5;
    g_uart7_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_uart7_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart7_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart7_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_uart7_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart7_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart7_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_uart7_dmarx_handle);
    __HAL_LINKDMA(&g_uart7_handle, hdmarx, g_uart7_dmarx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

#if UART7_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_uart7_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_uart7_handle);
#endif /* UART7_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_uart7_handle, (uint8_t *)g_uart7_recv_buf, buf_size);
}

/**
 * @brief 串口7接收中断句柄
 *
 */
void DMA1_Stream3_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart7_dmarx_handle);
}
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

#if UART8_USE_DMA_TX

static DMA_HandleTypeDef g_uart8_dmatx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_uart8_tx_fifo = {0};
static uint8_t *g_uart8_send_buf;
/**
 * @brief 串口8发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void uart8_dmatx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart8_send_buf = (uint8_t *)malloc((sizeof(uint8_t)) * buf_size);
    g_uart8_tx_fifo.send_buf = g_uart8_send_buf;
    g_uart8_tx_fifo.send_buf_size = buf_size;
    g_uart8_tx_fifo.tc_flag = 1;
    g_uart8_tx_fifo.tx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));

    g_uart8_tx_fifo.tx_fifo =
        ring_fifo_init(g_uart8_tx_fifo.tx_fifo_buf, fifo_size, RF_TYPE_STREAM);
    if (g_uart8_tx_fifo.tx_fifo_buf == NULL) {
        printf("UART8 TX FIFO Init Failed. \r\n");
        while (1)
            ;
    }

    __HAL_RCC_DMA1_CLK_ENABLE();
    g_uart8_dmatx_handle.Instance = DMA1_Stream0;
    g_uart8_dmatx_handle.Init.Channel = DMA_CHANNEL_5;
    g_uart8_dmatx_handle.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    g_uart8_dmatx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart8_dmatx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart8_dmatx_handle.Init.Mode = DMA_NORMAL;
    g_uart8_dmatx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart8_dmatx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart8_dmatx_handle.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&g_uart8_dmatx_handle);
    __HAL_LINKDMA(&g_uart8_handle, hdmatx, g_uart8_dmatx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief 串口8发送中断句柄
 *
 */
void DMA1_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart8_dmatx_handle);
}
#endif /* UART8_USE_DMA_TX */

#if UART8_USE_DMA_RX

static DMA_HandleTypeDef g_uart8_dmarx_handle = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_uart8_rx_fifo = {0};
static uint8_t *g_uart8_recv_buf;

/**
 * @brief 串口8接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void uart8_dmarx_init(uint32_t fifo_size, uint32_t buf_size) {
    g_uart8_recv_buf = (uint8_t *)malloc((sizeof(uint8_t)) * (buf_size));
    g_uart8_rx_fifo.head_ptr = 0;
    g_uart8_rx_fifo.rx_fifo_buf =
        (uint8_t *)malloc((sizeof(uint8_t)) * (fifo_size));
    g_uart8_rx_fifo.rx_fifo =
        ring_fifo_init(g_uart8_rx_fifo.rx_fifo_buf,
                       (sizeof(uint8_t)) * (fifo_size), RF_TYPE_STREAM);
    if (g_uart8_rx_fifo.rx_fifo == NULL) {
        printf("UART8 RX FIFO Init Failed. \r\n");
        while (1)
            ;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();

    g_uart8_dmarx_handle.Instance = DMA1_Stream6;
    g_uart8_dmarx_handle.Init.Channel = DMA_CHANNEL_5;
    g_uart8_dmarx_handle.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    g_uart8_dmarx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_uart8_dmarx_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_uart8_dmarx_handle.Init.Mode = DMA_CIRCULAR;
    g_uart8_dmarx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_uart8_dmarx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_uart8_dmarx_handle.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_uart8_dmarx_handle);
    __HAL_LINKDMA(&g_uart8_handle, hdmarx, g_uart8_dmarx_handle);

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

#if UART8_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&g_uart8_handle, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&g_uart8_handle);
#endif /* UART8_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&g_uart8_handle, (uint8_t *)g_uart8_recv_buf, buf_size);
}

/**
 * @brief 串口8接收中断句柄
 *
 */
void DMA1_Stream6_IRQHandler(void) {
    HAL_DMA_IRQHandler(&g_uart8_dmarx_handle);
}
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

/**
 * @brief 从发送FIFO中读数据
 *
 * @param uart_fifo 串口发送FIFO
 * @return uint32_t 读出的长度
 */
static inline uint32_t uart_read_tx_fifo(uart_tx_fifo_t *uart_fifo) {
    return ring_fifo_read(uart_fifo->tx_fifo, uart_fifo->send_buf,
                          uart_fifo->send_buf_size);
}

/**
 * @brief 判断是那个串口, 指定到对应的发送FIFO
 *
 * @param huart 串口句柄
 * @return uart_tx_fifo_t* FIFO指针
 */
static inline uart_tx_fifo_t *uart_tx_identify(UART_HandleTypeDef *huart) {
    /* 指定发送FIFO */
    if (huart->Instance == USART1) {
#if USART1_USE_DMA_TX
        return &g_usart1_tx_fifo;
#endif /* USART1_USE_DMA_TX */
    } else if (huart->Instance == USART2) {
#if USART2_USE_DMA_TX
        return &g_usart2_tx_fifo;
#endif /* USART2_USE_DMA_TX */
    } else if (huart->Instance == USART3) {
#if USART3_USE_DMA_TX
        return &g_usart3_tx_fifo;
#endif /* USART3_USE_DMA_TX */
    } else if (huart->Instance == UART4) {
#if UART4_USE_DMA_TX
        return &g_uart4_tx_fifo;
#endif /* UART4_USE_DMA_TX */
    } else if (huart->Instance == UART5) {
#if UART5_USE_DMA_TX
        return &g_uart5_tx_fifo;
#endif /* UART5_USE_DMA_TX */
    } else if (huart->Instance == USART6) {
#if USART6_USE_DMA_TX
        return &g_usart6_tx_fifo;
#endif /* USART6_USE_DMA_TX */
    } else if (huart->Instance == UART7) {
#if UART7_USE_DMA_TX
        return &g_uart7_tx_fifo;
#endif /* UART7_USE_DMA_TX */
    } else if (huart->Instance == UART8) {
#if UART8_USE_DMA_TX
        return &g_uart8_tx_fifo;
#endif /* UART8_USE_DMA_TX */
    }
    return NULL;
}

/**
 * @brief 清除发送完成标志位
 *
 * @param huart 串口句柄
 */
void uart_dmatx_clear_tc_flag(UART_HandleTypeDef *huart) {
    uart_tx_fifo_t *tx_fifo = uart_tx_identify(huart);

    if (tx_fifo == NULL) {
        return;
    }

    tx_fifo->tc_flag = 1;
}

/**
 * @brief 向串口发送FIFO中写数据
 *
 * @param huart 串口句柄
 * @param data 数据
 * @param len 数据长度
 */
void uart_dmatx_write(UART_HandleTypeDef *huart, const void *data,
                      uint32_t len) {
    if ((data == NULL) || (len == 0)) {
        return;
    }

    uart_tx_fifo_t *send_tx_fifo = uart_tx_identify(huart);

    if (send_tx_fifo == NULL) {
        return;
    }

    uint32_t copied = ring_fifo_write(send_tx_fifo->tx_fifo, data, len);
    if (copied != len) {
        // printf("%s if full. \n", __FUNCTION__);
    }
}

/**
 * @brief 把FIFO中的数据通过DMA发送
 *
 * @param huart 串口句柄
 * @note 先使用`UART_DMARX_FIFO_Write`写入数据
 */
void uart_dmatx_send(UART_HandleTypeDef *huart) {
    uart_tx_fifo_t *send_tx_fifo = uart_tx_identify(huart);

    if (send_tx_fifo == NULL) {
        return;
    }

    /* 未发送完毕 */
    if (!send_tx_fifo->tc_flag) {
        return;
    }

    uint32_t len = uart_read_tx_fifo(send_tx_fifo);
    /* FIFO为空 */
    if (!len) {
        return;
    }

    if (huart->hdmatx == NULL) {
        printf("This uart is not enable DMA TX. \r\n");
        return;
    }
    send_tx_fifo->tc_flag = 0;
    HAL_UART_Transmit_DMA(huart, send_tx_fifo->send_buf, (uint16_t)len);
}

/**
 * @brief 向接收FIFO写数据
 *
 * @param uart_fifo 串口接收FIFO
 * @param data 要写入的数据
 * @param len 数据长度
 */
static inline void uart_write_rx_fifo(uart_rx_fifo_t *uart_fifo,
                                     const void *data, uint32_t len) {
    if ((data == NULL) || (len == 0)) {
        return;
    }

    uint32_t copied = ring_fifo_write(uart_fifo->rx_fifo, data, len);
    if (copied != len) {
        // printf("%s is full. \r\n", __FUNCTION__);
    }
}

/**
 * @brief 判断是那个串口, 指定到对应的接收FIFO
 *
 * @param huart 串口句柄
 * @return uart_rx_fifo_t* FIFO指针
 */
static inline uart_rx_fifo_t *uart_rx_identify(UART_HandleTypeDef *huart) {
    /* 指定接收FIFO */
    if (huart->Instance == USART1) {
#if USART1_USE_DMA_RX
        return &g_usart1_rx_fifo;
#endif /* USART1_USE_DMA_RX */
    } else if (huart->Instance == USART2) {
#if USART2_USE_DMA_RX
        return &g_usart2_rx_fifo;
#endif /* USART2_USE_DMA_RX */
    } else if (huart->Instance == USART3) {
#if USART3_USE_DMA_RX
        return &g_usart3_rx_fifo;
#endif /* USART3_USE_DMA_RX */
    } else if (huart->Instance == UART4) {
#if UART4_USE_DMA_RX
        return &g_uart4_rx_fifo;
#endif /* UART4_USE_DMA_RX */
    } else if (huart->Instance == UART5) {
#if UART5_USE_DMA_RX
        return &g_uart5_rx_fifo;
#endif /* UART5_USE_DMA_RX */
    } else if (huart->Instance == USART6) {
#if USART6_USE_DMA_RX
        return &g_usart6_rx_fifo;
#endif /* USART6_USE_DMA_RX */
    } else if (huart->Instance == UART7) {
#if UART7_USE_DMA_RX
        return &g_uart7_rx_fifo;
#endif /* UART7_USE_DMA_RX */
    } else if (huart->Instance == UART8) {
#if UART8_USE_DMA_RX
        return &g_uart8_rx_fifo;
#endif /* UART8_USE_DMA_RX */
    }
    return NULL;
}

/**
 * @brief 串口接收空闲回调
 *
 * @param huart 串口句柄
 */
void uart_dmarx_idle_callback(UART_HandleTypeDef *huart) {
    uart_rx_fifo_t *uart_rx_fifo = uart_rx_identify(huart);

    if (uart_rx_fifo == NULL) {
        return;
    }

    uint32_t tail_ptr;
    uint32_t copy, offset;

    /**
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     * |     head_ptr          tail_ptr         |
     * |         |                 |            |
     * |         v                 v            |
     * | --------*******************----------- |
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     */

    /* 已接收 */
    tail_ptr = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);

    offset = (uart_rx_fifo->head_ptr) % (uint32_t)(huart->RxXferSize);
    copy = tail_ptr - offset;
    uart_rx_fifo->head_ptr += copy;

    uart_write_rx_fifo(uart_rx_fifo, huart->pRxBuffPtr + offset, copy);
}

/**
 * @brief DMA接收半满回调
 *
 * @param huart 串口句柄
 */
void uart_dmarx_halfdone_callback(UART_HandleTypeDef *huart) {
    uart_rx_fifo_t *uart_rx_fifo = uart_rx_identify(huart);

    if (uart_rx_fifo == NULL) {
        return;
    }

    uint32_t tail_ptr;
    uint32_t offset, copy;

    /**
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     * |                  half                  |
     * |     head_ptr   tail_ptr                |
     * |         |          |                   |
     * |         v          v                   |
     * | --------*******************----------- |
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     */

    tail_ptr = (huart->RxXferSize >> 1) + (huart->RxXferSize & 1);

    offset = (uart_rx_fifo->head_ptr) % (uint32_t)(huart->RxXferSize);
    copy = tail_ptr - offset;
    uart_rx_fifo->head_ptr += copy;

    uart_write_rx_fifo(uart_rx_fifo, huart->pRxBuffPtr + offset, copy);
}

/**
 * @brief DMA溢满中断回调
 *
 * @param huart 串口句柄
 */
void uart_dmarx_done_callback(UART_HandleTypeDef *huart) {
    uart_rx_fifo_t *uart_rx_fifo = uart_rx_identify(huart);

    if (uart_rx_fifo == NULL) {
        return;
    }

    uint32_t tail_ptr;
    uint32_t offset, copy;

    /**
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     * |                  half                  |
     * |                    | head_ptr tail_ptr |
     * |                    |    |            | |
     * |                    v    v            v |
     * | ------------------------************** |
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     */

    tail_ptr = huart->RxXferSize;

    offset = (uart_rx_fifo->head_ptr) % (uint32_t)(huart->RxXferSize);
    copy = tail_ptr - offset;
    uart_rx_fifo->head_ptr += copy;

    uart_write_rx_fifo(uart_rx_fifo, huart->pRxBuffPtr + offset, copy);
}

/**
 * @brief 从接收FIFO读数据
 *
 * @param huart 串口句柄
 * @param buf 接收缓冲数组
 * @param len `buf`长度
 * @return uint32_t 接收到的长度
 */
uint32_t uart_dmarx_read(UART_HandleTypeDef *huart, void *buf, uint32_t len) {
    if ((buf == NULL) || (len == 0)) {
        return 0;
    }
    uart_rx_fifo_t *uart_rx_fifo = uart_rx_identify(huart);

    if (uart_rx_fifo == NULL) {
        return 0;
    }

    return ring_fifo_read(uart_rx_fifo->rx_fifo, buf, len);
}

/**
 * @}
 */
