/**
 * @file    dma_uart.c
 * @author  Deadline039
 * @brief   使用DMA+半满中断+满中断+空闲中断实现高可靠串口数据收发
 * @version 1.0
 * @date    2024-01-18
 * @note    stm32f103串口DMA配置文件
 * @see     https://github.com/Prry/stm32-uart-dma
 * @see     https://gitee.com/wei513723/stm32-stable-uart-transmit-receive
 */

#include "dma_uart.h"

/*******************************************************************************
 * @defgroup 串口1 DMA
 * @{
 */

#ifdef EN_USART1

    #if USART1_USE_DMA_TX

static DMA_HandleTypeDef USART1_DMA_TX_Handler = {0};
/* FIFO+BUF, 双缓冲 */
static uart_tx_fifo_t g_usart1_tx_fifo = {0};
static uint8_t *g_usart1_send_buf;

/**
 * @brief 串口1发送DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 * @warning `fifo_size`必须是2的幂次方
 */
void USART1_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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

    __HAL_RCC_DMA1_CLK_ENABLE();
    USART1_DMA_TX_Handler.Instance = DMA1_Channel4;
    USART1_DMA_TX_Handler.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    USART1_DMA_TX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART1_DMA_TX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART1_DMA_TX_Handler.Init.Mode = DMA_NORMAL;
    USART1_DMA_TX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART1_DMA_TX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART1_DMA_TX_Handler.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&USART1_DMA_TX_Handler);
    __HAL_LINKDMA(&USART1_Handler, hdmatx, USART1_DMA_TX_Handler);

    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

/**
 * @brief 串口1发送中断句柄
 *
 */
void DMA1_Channel4_IRQHandler(void) {
    HAL_DMA_IRQHandler(&USART1_DMA_TX_Handler);
}
    #endif /* USART1_USE_DMA_TX */

    #if USART1_USE_DMA_RX

static DMA_HandleTypeDef USART1_DMA_RX_Handler = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart1_rx_fifo = {0};
static uint8_t *g_usart1_recv_buf;

/**
 * @brief 串口1接收DMA初始化
 *
 * @param fifo_size FIFO大小
 * @param buf_size 缓冲区大小
 */
void USART1_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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
    __HAL_RCC_DMA1_CLK_ENABLE();

    USART1_DMA_RX_Handler.Instance = DMA1_Channel5;
    USART1_DMA_RX_Handler.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    USART1_DMA_RX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART1_DMA_RX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART1_DMA_RX_Handler.Init.Mode = DMA_CIRCULAR;
    USART1_DMA_RX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART1_DMA_RX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART1_DMA_RX_Handler.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&USART1_DMA_RX_Handler);
    __HAL_LINKDMA(&USART1_Handler, hdmarx, USART1_DMA_RX_Handler);

    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

        #if USART1_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&USART1_Handler, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&USART1_Handler);
        #endif /* USART1_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&USART1_Handler, (uint8_t *)g_usart1_recv_buf,
                         buf_size);
}

/**
 * @brief 串口1接收中断句柄
 *
 */
void DMA1_Channel5_IRQHandler(void) {
    HAL_DMA_IRQHandler(&USART1_DMA_RX_Handler);
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

static DMA_HandleTypeDef USART2_DMA_TX_Handler = {0};
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
void USART2_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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
    USART2_DMA_TX_Handler.Instance = DMA1_Channel7;
    USART2_DMA_TX_Handler.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    USART2_DMA_TX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART2_DMA_TX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART2_DMA_TX_Handler.Init.Mode = DMA_NORMAL;
    USART2_DMA_TX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART2_DMA_TX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART2_DMA_TX_Handler.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&USART2_DMA_TX_Handler);
    __HAL_LINKDMA(&USART2_Handler, hdmatx, USART2_DMA_TX_Handler);

    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/**
 * @brief 串口2发送中断句柄
 *
 */
void DMA1_Channel7_IRQHandler(void) {
    HAL_DMA_IRQHandler(&USART2_DMA_TX_Handler);
}
    #endif /* USART2_USE_DMA_TX */

    #if USART2_USE_DMA_RX

static DMA_HandleTypeDef USART2_DMA_RX_Handler = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart2_rx_fifo = {0};
static uint8_t *g_usart2_recv_buf;

/**
 * @brief 串口2接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void USART2_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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

    USART2_DMA_RX_Handler.Instance = DMA1_Channel6;
    USART2_DMA_RX_Handler.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    USART2_DMA_RX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART2_DMA_RX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART2_DMA_RX_Handler.Init.Mode = DMA_CIRCULAR;
    USART2_DMA_RX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART2_DMA_RX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART2_DMA_RX_Handler.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&USART2_DMA_RX_Handler);
    __HAL_LINKDMA(&USART2_Handler, hdmarx, USART2_DMA_RX_Handler);

    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

        #if USART2_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&USART2_Handler, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&USART2_Handler);
        #endif /* USART2_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&USART2_Handler, (uint8_t *)g_usart2_recv_buf,
                         buf_size);
}

/**
 * @brief 串口2接收中断句柄
 *
 */
void DMA1_Channel6_IRQHandler(void) {
    HAL_DMA_IRQHandler(&USART2_DMA_RX_Handler);
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

static DMA_HandleTypeDef USART3_DMA_TX_Handler = {0};
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
void USART3_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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
    USART3_DMA_TX_Handler.Instance = DMA1_Channel2;
    USART3_DMA_TX_Handler.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    USART3_DMA_TX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART3_DMA_TX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART3_DMA_TX_Handler.Init.Mode = DMA_NORMAL;
    USART3_DMA_TX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART3_DMA_TX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART3_DMA_TX_Handler.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&USART3_DMA_TX_Handler);
    __HAL_LINKDMA(&USART3_Handler, hdmatx, USART3_DMA_TX_Handler);

    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

/**
 * @brief 串口3发送中断句柄
 *
 */
void DMA1_Channel2_IRQHandler(void) {
    HAL_DMA_IRQHandler(&USART3_DMA_TX_Handler);
}
    #endif /* USART3_USE_DMA_TX */

    #if USART3_USE_DMA_RX

static DMA_HandleTypeDef USART3_DMA_RX_Handler = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_usart3_rx_fifo = {0};
static uint8_t *g_usart3_recv_buf;

/**
 * @brief 串口3接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void USART3_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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

    USART3_DMA_RX_Handler.Instance = DMA1_Channel3;
    USART3_DMA_RX_Handler.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    USART3_DMA_RX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART3_DMA_RX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    USART3_DMA_RX_Handler.Init.Mode = DMA_CIRCULAR;
    USART3_DMA_RX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART3_DMA_RX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    USART3_DMA_RX_Handler.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&USART3_DMA_RX_Handler);
    __HAL_LINKDMA(&USART3_Handler, hdmarx, USART3_DMA_RX_Handler);

    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

        #if USART3_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&USART3_Handler, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&USART3_Handler);
        #endif /* USART3_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&USART3_Handler, (uint8_t *)g_usart3_recv_buf,
                         buf_size);
}
/**
 * @brief 串口3接收中断句柄
 *
 */
void DMA1_Channel3_IRQHandler(void) {
    HAL_DMA_IRQHandler(&USART3_DMA_RX_Handler);
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

static DMA_HandleTypeDef UART4_DMA_TX_Handler = {0};
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
void UART4_TX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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

    __HAL_RCC_DMA2_CLK_ENABLE();
    UART4_DMA_TX_Handler.Instance = DMA2_Channel5;
    UART4_DMA_TX_Handler.Init.Direction =
        DMA_MEMORY_TO_PERIPH; /* 发送, 内存到外设 */
    UART4_DMA_TX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    UART4_DMA_TX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    UART4_DMA_TX_Handler.Init.Mode = DMA_NORMAL;
    UART4_DMA_TX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    UART4_DMA_TX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    UART4_DMA_TX_Handler.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&UART4_DMA_TX_Handler);
    __HAL_LINKDMA(&UART4_Handler, hdmatx, UART4_DMA_TX_Handler);

    HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
}
/**
 * @brief 串口4发送中断句柄
 *
 */
void DMA2_Channel4_5_IRQHandler(void) {
    HAL_DMA_IRQHandler(&UART4_DMA_TX_Handler);
}
    #endif /* UART4_USE_DMA_TX */

    #if UART4_USE_DMA_RX

static DMA_HandleTypeDef UART4_DMA_RX_Handler = {0};
/* FIFO+BUF, 双缓冲 */
static uart_rx_fifo_t g_uart4_rx_fifo = {0};
static uint8_t *g_uart4_recv_buf;

/**
 * @brief 串口4接收DMA初始化
 *
 * @param fifo_size 接收FIFO大小
 * @param buf_size 接收缓冲区大小
 */
void UART4_RX_DMA_Init(uint32_t fifo_size, uint32_t buf_size) {
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
    __HAL_RCC_DMA2_CLK_ENABLE();

    UART4_DMA_RX_Handler.Instance = DMA2_Channel3;
    UART4_DMA_RX_Handler.Init.Direction =
        DMA_PERIPH_TO_MEMORY; /* 接收, 外设到内存 */
    UART4_DMA_RX_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    UART4_DMA_RX_Handler.Init.MemInc = DMA_MINC_ENABLE;
    UART4_DMA_RX_Handler.Init.Mode = DMA_CIRCULAR;
    UART4_DMA_RX_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    UART4_DMA_RX_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    UART4_DMA_RX_Handler.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&UART4_DMA_RX_Handler);
    __HAL_LINKDMA(&UART4_Handler, hdmarx, UART4_DMA_RX_Handler);

    HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);

        #if UART4_USE_IDLE_IT
    __HAL_UART_ENABLE_IT(&UART4_Handler, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&UART4_Handler);
        #endif /* UART4_USE_IDLE_IT */
    HAL_UART_Receive_DMA(&UART4_Handler, (uint8_t *)g_uart4_recv_buf, buf_size);
}

/**
 * @brief 串口4接收中断句柄
 *
 */
void DMA2_Channel3_IRQHandler(void) {
    HAL_DMA_IRQHandler(&UART4_DMA_RX_Handler);
}
    #endif /* UART4_USE_DMA_RX */

#endif /* EN_UART4 */

/**
 * @}
 */

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
static inline uint32_t UART_Read_TxFifo(uart_tx_fifo_t *uart_fifo) {
    return ring_fifo_read(uart_fifo->tx_fifo, uart_fifo->send_buf,
                          uart_fifo->send_buf_size);
}

/**
 * @brief 判断是那个串口, 指定到对应的发送FIFO
 *
 * @param huart 串口句柄
 * @return uart_tx_fifo_t* FIFO指针
 */
static inline uart_tx_fifo_t *UART_TX_Identify(UART_HandleTypeDef *huart) {
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
    }
    return NULL;
}

/**
 * @brief 清除发送完成标志位
 *
 * @param huart 串口句柄
 */
void UART_DMATX_Clear_TC_Flag(UART_HandleTypeDef *huart) {
    uart_tx_fifo_t *tx_fifo = UART_TX_Identify(huart);

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
void UART_DMATX_Write(UART_HandleTypeDef *huart, const void *data,
                      uint32_t len) {
    if ((data == NULL) || (len == 0)) {
        return;
    }

    uart_tx_fifo_t *send_tx_fifo = UART_TX_Identify(huart);

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
void UART_DMATX_Send(UART_HandleTypeDef *huart) {
    uart_tx_fifo_t *send_tx_fifo = UART_TX_Identify(huart);

    if (send_tx_fifo == NULL) {
        return;
    }

    /* 未发送完毕 */
    if (!send_tx_fifo->tc_flag) {
        return;
    }

    uint32_t len = UART_Read_TxFifo(send_tx_fifo);
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
static inline void UART_Write_RxFifo(uart_rx_fifo_t *uart_fifo,
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
static inline uart_rx_fifo_t *UART_RX_Identify(UART_HandleTypeDef *huart) {
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
    }
    return NULL;
}

/**
 * @brief 串口接收空闲回调
 *
 * @param huart 串口句柄
 */
void UART_DMARX_Idle_Callback(UART_HandleTypeDef *huart) {
    uint32_t tail_ptr;
    uint32_t copy, offset;

    /*
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     * |     head_ptr          tail_ptr         |
     * |         |                 |            |
     * |         v                 v            |
     * | --------*******************----------- |
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     */

    /* 已接收 */
    tail_ptr = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);

    uart_rx_fifo_t *uart_rx_fifo = UART_RX_Identify(huart);

    if (uart_rx_fifo == NULL) {
        return;
    }

    offset = (uart_rx_fifo->head_ptr) % (uint32_t)(huart->RxXferSize);
    copy = tail_ptr - offset;
    uart_rx_fifo->head_ptr += copy;

    UART_Write_RxFifo(uart_rx_fifo, huart->pRxBuffPtr + offset, copy);
}

/**
 * @brief DMA接收半满回调
 *
 * @param huart 串口句柄
 */
void UART_DMARX_HalfDone_Callback(UART_HandleTypeDef *huart) {
    uint32_t tail_ptr;
    uint32_t offset, copy;

    /*
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     * |                  half                  |
     * |     head_ptr   tail_ptr                |
     * |         |          |                   |
     * |         v          v                   |
     * | --------*******************----------- |
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     */

    tail_ptr = (huart->RxXferSize >> 1) + (huart->RxXferSize & 1);

    uart_rx_fifo_t *uart_rx_fifo = UART_RX_Identify(huart);

    if (uart_rx_fifo == NULL) {
        return;
    }

    offset = (uart_rx_fifo->head_ptr) % (uint32_t)(huart->RxXferSize);
    copy = tail_ptr - offset;
    uart_rx_fifo->head_ptr += copy;

    UART_Write_RxFifo(uart_rx_fifo, huart->pRxBuffPtr + offset, copy);
}

/**
 * @brief DMA溢满中断回调
 *
 * @param huart 串口句柄
 */
void UART_DMARX_Done_Callback(UART_HandleTypeDef *huart) {
    uint32_t tail_ptr;
    uint32_t offset, copy;

    /*
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     * |                  half                  |
     * |                    | head_ptr tail_ptr |
     * |                    |    |            | |
     * |                    v    v            v |
     * | ------------------------************** |
     * +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
     */

    tail_ptr = huart->RxXferSize;

    uart_rx_fifo_t *uart_rx_fifo = UART_RX_Identify(huart);

    if (uart_rx_fifo == NULL) {
        return;
    }

    offset = (uart_rx_fifo->head_ptr) % (uint32_t)(huart->RxXferSize);
    copy = tail_ptr - offset;
    uart_rx_fifo->head_ptr += copy;

    UART_Write_RxFifo(uart_rx_fifo, huart->pRxBuffPtr + offset, copy);
}
/**
 * @brief 从接收FIFO读数据
 *
 * @param huart 串口句柄
 * @param buf 接收缓冲数组
 * @param len `buf`长度
 * @return uint32_t 接收到的长度
 */
uint32_t UART_DMARX_Read(UART_HandleTypeDef *huart, void *buf, uint32_t len) {
    if ((buf == NULL) || (len == 0)) {
        return 0;
    }
    uart_rx_fifo_t *uart_rx_fifo = UART_RX_Identify(huart);

    if (uart_rx_fifo == NULL) {
        return 0;
    }

    return ring_fifo_read(uart_rx_fifo->rx_fifo, buf, len);
}

/**
 * @}
 */
