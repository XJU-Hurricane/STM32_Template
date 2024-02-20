/**
 * @file    uart.c
 * @author  Deadline039
 * @version 1.0
 * @date    2023-09-14
 * @note    STM32F103串口(usart1->uart5)的初始化函数以及中断服务函数
 *          如果启用了串口1,printf函数将会被重定义为从串口1输出
 *          如果使用串口DMA, 根据需要设置缓冲区大小, 在串口初始化的DMA
 *          Init函数中
 * @warning 要使用相关函数请预先在uart.h里打开宏定义
 *
 */

#include "uart.h"
#include "dma_uart.h"
#include "stdarg.h"
#include "string.h"

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h" /* os 使用 */
#endif          /* SYS_SUPPORT_OS */

static uint8_t g_uart_tx_buf[TX_BUF_LEN];

/*******************************************************************************
 * @defgroup 串口1
 * @{
 */

#ifdef EN_USART1
UART_HandleTypeDef g_usart1_handle;
#if EN_USART1_RX
#if !USART1_USE_DMA_RX
uint8_t g_usart1_rx_buf[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t g_usart1_rx_status = 0;
uint8_t g_usart1_recv_buf[RXBUFFERSIZE];
#endif /* !USART1_USE_DMA_RX */
/**
 * @brief 串口1中断服务函数
 */
void USART1_IRQHandler(void) {
#if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
#endif /* SYS_SUPPORT_OS */

#if USART1_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&g_usart1_handle, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&g_usart1_handle);
        uart_dmarx_idle_callback(&g_usart1_handle);
    }
#endif /* USART1_USE_IDLE_IT */

    HAL_UART_IRQHandler(&g_usart1_handle); /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
#endif /* SYS_SUPPORT_OS */
}
#endif /* EN_USART1_RX */
/**
 * @brief 串口1初始化
 * @param bound 波特率
 */
void usart1_init(uint32_t bound) {
    g_usart1_handle.Instance = USART1;
    g_usart1_handle.Init.BaudRate = bound;
    g_usart1_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_usart1_handle.Init.StopBits = UART_STOPBITS_1;
    g_usart1_handle.Init.Parity = UART_PARITY_NONE;
    g_usart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_usart1_handle.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&g_usart1_handle);

#if USART1_USE_DMA_TX
    /* 打开DMA发送 */
    usart1_dmatx_init(2048, 128);
#endif /* USART1_USE_DMA_TX */

#if EN_USART1_RX
#if USART1_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    usart1_dmarx_init(4096, 128);
#else  /* USART1_USE_DMA_RX */
    HAL_UART_Receive_IT(&g_usart1_handle, (uint8_t *)g_usart1_recv_buf,
                        RXBUFFERSIZE);
#endif /* USART1_USE_DMA_RX */
#endif /* EN_USART1_RX */
}

#endif /* EN_USART1 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口2
 * @{
 */

#ifdef EN_USART2
UART_HandleTypeDef g_usart2_handle;
#if EN_USART2_RX
#if !USART2_USE_DMA_RX
uint8_t g_usart2_rx_buf[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t g_usart2_rx_status = 0;
uint8_t g_usart2_recv_buf[RXBUFFERSIZE];
#endif /* !USART2_USE_DMA_RX */
/**
 * @brief 串口2中断服务函数
 */
void USART2_IRQHandler(void) {
#if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
#endif /* SYS_SUPPORT_OS */

#if USART2_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&g_usart2_handle, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&g_usart2_handle);
        uart_dmarx_idle_callback(&g_usart2_handle);
    }
#endif /* USART2_USE_IDLE_IT */

    HAL_UART_IRQHandler(&g_usart2_handle); /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
#endif /* SYS_SUPPORT_OS */
}
#endif /* EN_USART2_RX */
/**
 * @brief 串口2初始化
 * @param bound 波特率
 */
void usart2_init(uint32_t bound) {
    g_usart2_handle.Instance = USART2;
    g_usart2_handle.Init.BaudRate = bound;
    g_usart2_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_usart2_handle.Init.StopBits = UART_STOPBITS_1;
    g_usart2_handle.Init.Parity = UART_PARITY_NONE;
    g_usart2_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_usart2_handle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&g_usart2_handle);

#if USART2_USE_DMA_TX
    /* 打开DMA发送 */
    usart2_dmatx_init(2048, 128);
#endif /* USART2_USE_DMA_TX */

#if EN_USART2_RX
#if USART2_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    usart2_dmarx_init(4096, 128);
#else  /* USART2_USE_DMA_RX */
    HAL_UART_Receive_IT(&g_usart2_handle, (uint8_t *)g_usart2_recv_buf,
                        RXBUFFERSIZE);
#endif /* USART2_USE_DMA_RX */
#endif /* EN_USART2_RX */
}
#endif /* EN_USART2 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口3
 * @{
 */

#ifdef EN_USART3
UART_HandleTypeDef g_usart3_handle;
#if EN_USART3_RX
#if !USART3_USE_DMA_RX
uint8_t g_usart3_rx_buf[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t g_usart3_rx_status = 0;
uint8_t g_usart3_recv_buf[RXBUFFERSIZE];
#endif /* !USART3_USE_DMA_RX */
/**
 * @brief 串口3中断服务函数
 */
void USART3_IRQHandler(void) {
#if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
#endif /* SYS_SUPPORT_OS */

#if USART3_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&g_usart3_handle, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&g_usart3_handle);
        uart_dmarx_idle_callback(&g_usart3_handle);
    }
#endif /* USART3_USE_IDLE_IT */

    HAL_UART_IRQHandler(&g_usart3_handle); /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
#endif /* SYS_SUPPORT_OS */
}
#endif /* EN_USART3_RX */
/**
 * @brief 串口3初始化
 * @param bound 波特率
 */
void usart3_init(uint32_t bound) {
    g_usart3_handle.Instance = USART3;
    g_usart3_handle.Init.BaudRate = bound;
    g_usart3_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_usart3_handle.Init.StopBits = UART_STOPBITS_1;
    g_usart3_handle.Init.Parity = UART_PARITY_NONE;
    g_usart3_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_usart3_handle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&g_usart3_handle);

#if USART3_USE_DMA_TX
    /* 打开DMA发送 */
    usart3_dmatx_init(2048, 128);
#endif /* USART3_USE_DMA_TX */

#if EN_USART3_RX
#if USART3_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    usart3_dmarx_init(4096, 128);
#else  /* USART3_USE_DMA_RX */
    HAL_UART_Receive_IT(&g_usart3_handle, (uint8_t *)g_usart3_recv_buf,
                        RXBUFFERSIZE);
#endif /* USART3_USE_DMA_RX */
#endif /* EN_USART3_RX */
}
#endif /* EN_USART3 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口4
 * @{
 */

#ifdef EN_UART4
UART_HandleTypeDef UART4_Handler;
#if EN_UART4_RX
#if !UART4_USE_DMA_RX
uint8_t g_uart4_rx_buf[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t g_uart4_rx_status = 0;
uint8_t g_uart4_recv_buf[RXBUFFERSIZE];
#endif /* !UART4_USE_DMA_RX */
/**
 * @brief 串口4中断服务函数
 */
void UART4_IRQHandler(void) {
#if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
#endif /* SYS_SUPPORT_OS */

#if UART4_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&UART4_Handler, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&UART4_Handler);
        uart_dmarx_idle_callback(&UART4_Handler);
    }
#endif /* UART4_USE_IDLE_IT */

    HAL_UART_IRQHandler(&UART4_Handler); /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
#endif /* SYS_SUPPORT_OS */
}
#endif /* EN_UART4_RX */
/**
 * @brief 串口4初始化
 * @param bound 波特率
 */
void uart4_init(uint32_t bound) {
    UART4_Handler.Instance = UART4;
    UART4_Handler.Init.BaudRate = bound;
    UART4_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART4_Handler.Init.StopBits = UART_STOPBITS_1;
    UART4_Handler.Init.Parity = UART_PARITY_NONE;
    UART4_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART4_Handler.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&UART4_Handler);

#if UART4_USE_DMA_TX
    /* 打开DMA发送 */
    uart4_dmatx_init(2048, 128);
#endif /* UART4_USE_DMA_TX */

#if EN_UART4_RX
#if UART4_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    uart4_dmarx_init(4096, 128);
#else  /* UART4_USE_DMA_RX */
    HAL_UART_Receive_IT(&UART4_Handler, (uint8_t *)g_uart4_recv_buf,
                        RXBUFFERSIZE);
#endif /* UART4_USE_DMA_RX */
#endif /* EN_UART4_RX */
}
#endif /* EN_UART4 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 串口5
 * @{
 */

#ifdef EN_UART5
UART_HandleTypeDef UART5_Handler;
#if EN_UART5_RX
uint8_t g_uart5_rx_buf[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t g_uart5_rx_status = 0;
uint8_t g_uart5_recv_buf[RXBUFFERSIZE];
/**
 * @brief 串口5中断服务函数
 */
void UART5_IRQHandler(void) {
#if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
#endif /* SYS_SUPPORT_OS */

    HAL_UART_IRQHandler(&UART5_Handler); /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
#endif /* SYS_SUPPORT_OS */
}
#endif /* EN_UART5_RX */
/**
 * @brief 串口5初始化
 * @param bound 波特率
 */
void uart5_init(uint32_t bound) {
    UART5_Handler.Instance = UART5;
    UART5_Handler.Init.BaudRate = bound;
    UART5_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART5_Handler.Init.StopBits = UART_STOPBITS_1;
    UART5_Handler.Init.Parity = UART_PARITY_NONE;
    UART5_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART5_Handler.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&UART5_Handler);
#if EN_UART5_RX
    HAL_UART_Receive_IT(&UART5_Handler, (uint8_t *)g_uart5_recv_buf,
                        RXBUFFERSIZE);
#endif /* EN_UART5_RX */
}
#endif /* EN_UART5 */

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 公共处理函数
 * @{
 */

/**
 * @brief 串口底层驱动
 *
 * @param huart 串口句柄
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    GPIO_InitTypeDef GPIO_Initure = {0};
    if (huart->Instance == USART1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin = GPIO_PIN_9;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_Initure); /* PA9 */
        GPIO_Initure.Pin = GPIO_PIN_10;
        GPIO_Initure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &GPIO_Initure); /* PA10 */
#if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        HAL_NVIC_SetPriority(USART1_IRQn, 2, 2);
#endif /* EN_USART1_RX */
    } else if (huart->Instance == USART2) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin = GPIO_PIN_2;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_Initure); /* PA2 */
        GPIO_Initure.Pin = GPIO_PIN_3;
        GPIO_Initure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &GPIO_Initure); /* PA3 */
#if EN_USART2_RX
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        HAL_NVIC_SetPriority(USART2_IRQn, 2, 2);
#endif /* EN_USART2_RX */
    } else if (huart->Instance == USART3) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART3_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin = GPIO_PIN_10;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_Initure); /* PB10 */
        GPIO_Initure.Pin = GPIO_PIN_11;
        GPIO_Initure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOB, &GPIO_Initure); /* PB11 */
#if EN_USART3_RX
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        HAL_NVIC_SetPriority(USART3_IRQn, 2, 2);
#endif /* EN_USART3_RX */
    } else if (huart->Instance == UART4) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_UART4_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin = GPIO_PIN_10;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure); /* PC10 */
        GPIO_Initure.Pin = GPIO_PIN_11;
        GPIO_Initure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure); /* PC11 */
#if EN_UART4_RX
        HAL_NVIC_EnableIRQ(UART4_IRQn);
        HAL_NVIC_SetPriority(UART4_IRQn, 2, 2);
#endif /* EN_UART4_RX */
    } else if (huart->Instance == UART5) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_UART5_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin = GPIO_PIN_12;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure); /* PC12 */
        GPIO_Initure.Pin = GPIO_PIN_2;
        GPIO_Initure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOD, &GPIO_Initure); /* PD2 */
#if EN_UART5_RX
        HAL_NVIC_EnableIRQ(UART5_IRQn);
        HAL_NVIC_SetPriority(UART5_IRQn, 2, 2);
#endif /* EN_UART5_RX */
    }
}

/**
 * @brief 串口发送完成回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    uart_dmatx_clear_tc_flag(huart);
}

/**
 * @brief 串口接收完成回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    /* 如果要使用某个串口的接收中断,请在头文件定义 */
    if (huart->Instance == USART1) {
#if EN_USART1_RX
#if !USART1_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((g_usart1_rx_status & 0x8000) == 0) {
            /* 接收未完成 */
            if (g_usart1_rx_status & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_usart1_recv_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    g_usart1_rx_status = 0; /* 接收错误,重新开始 */
                } else {
                    g_usart1_rx_status |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_usart1_recv_buf[0] == 0x0d) {
                    g_usart1_rx_status |= 0x4000;
                } else {
                    g_usart1_rx_buf[g_usart1_rx_status & 0X3FFF] =
                        g_usart1_recv_buf[0];
                    g_usart1_rx_status++;
                    if (g_usart1_rx_status > (USART_REC_LEN - 1)) {
                        g_usart1_rx_status = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_usart1_handle, (uint8_t *)g_usart1_recv_buf,
                            RXBUFFERSIZE);
#else  /* !USART1_USE_DMA_RX */
        uart_dmarx_done_callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
#endif /* !USART1_USE_DMA_RX */
#endif /* EN_USART1_RX */
    } else if (huart->Instance == USART2) {
#if EN_USART2_RX
#if !USART2_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((g_usart2_rx_status & 0x8000) == 0) {
            /* 接收未完成 */
            if (g_usart2_rx_status & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_usart2_recv_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    g_usart2_rx_status = 0; /* 接收错误,重新开始 */
                } else {
                    g_usart2_rx_status |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_usart2_recv_buf[0] == 0x0d) {
                    g_usart2_rx_status |= 0x4000;
                } else {
                    g_usart2_rx_buf[g_usart2_rx_status & 0X3FFF] =
                        g_usart2_recv_buf[0];
                    g_usart2_rx_status++;
                    if (g_usart2_rx_status > (USART_REC_LEN - 1)) {
                        g_usart2_rx_status = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_usart2_handle, (uint8_t *)g_usart2_recv_buf,
                            RXBUFFERSIZE);
#else  /* !USART2_USE_DMA_RX */
        uart_dmarx_done_callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
#endif /* !USART2_USE_DMA_RX */
#endif /* EN_USART2_RX */
    } else if (huart->Instance == USART3) {
#if EN_USART3_RX
#if !USART3_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((g_usart3_rx_status & 0x8000) == 0) {
            /* 接收未完成 */
            if (g_usart3_rx_status & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_usart3_recv_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    g_usart3_rx_status = 0; /* 接收错误,重新开始 */
                } else {
                    g_usart3_rx_status |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_usart3_recv_buf[0] == 0x0d) {
                    g_usart3_rx_status |= 0x4000;
                } else {
                    g_usart3_rx_buf[g_usart3_rx_status & 0X3FFF] =
                        g_usart3_recv_buf[0];
                    g_usart3_rx_status++;
                    if (g_usart3_rx_status > (USART_REC_LEN - 1)) {
                        g_usart3_rx_status = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_usart3_handle, (uint8_t *)g_usart3_recv_buf,
                            RXBUFFERSIZE);
#else  /* !USART3_USE_DMA_RX */
        uart_dmarx_done_callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
#endif /* !USART3_USE_DMA_RX */
#endif /* EN_USART3_RX */
    } else if (huart->Instance == UART4) {
#if EN_UART4_RX
#if !UART4_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((g_uart4_rx_status & 0x8000) == 0) {
            /* 接收未完成 */
            if (g_uart4_rx_status & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_uart4_recv_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    g_uart4_rx_status = 0; /* 接收错误,重新开始 */
                } else {
                    g_uart4_rx_status |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_uart4_recv_buf[0] == 0x0d) {
                    g_uart4_rx_status |= 0x4000;
                } else {
                    g_uart4_rx_buf[g_uart4_rx_status & 0X3FFF] =
                        g_uart4_recv_buf[0];
                    g_uart4_rx_status++;
                    if (g_uart4_rx_status > (USART_REC_LEN - 1)) {
                        g_uart4_rx_status = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&UART4_Handler, (uint8_t *)g_uart4_recv_buf,
                            RXBUFFERSIZE);
#else  /* !UART4_USE_DMA_RX */
        uart_dmarx_done_callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
#endif /* !UART4_USE_DMA_RX */
#endif /* EN_UART4_RX */
    } else if (huart->Instance == UART5) {
#if EN_UART5_RX
        if ((g_uart5_rx_status & 0x8000) == 0) {
            /* 接收未完成 */
            if (g_uart5_rx_status & 0x4000) {
                /* 接收到了0x0d */
                if (g_uart5_recv_buf[0] != 0x0a) {
                    g_uart5_rx_status = 0; /* 接收错误,重新开始 */
                } else {
                    g_uart5_rx_status |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0X0D */
                if (g_uart5_recv_buf[0] == 0x0d) {
                    g_uart5_rx_status |= 0x4000;
                } else {
                    g_uart5_rx_buf[g_uart5_rx_status & 0X3FFF] =
                        g_uart5_recv_buf[0];
                    g_uart5_rx_status++;
                    if (g_uart5_rx_status > (USART_REC_LEN - 1)) {
                        g_uart5_rx_status = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
            HAL_UART_Receive_IT(&UART5_Handler, (uint8_t *)g_uart5_recv_buf,
                                RXBUFFERSIZE);
        }
#endif /* EN_UART5_RX */
    }
}

/**
 * @brief 串口接收半满回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    uart_dmarx_halfdone_callbackk(huart);
}

/**
 * @brief 串口错误回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    __IO uint32_t tmpErr = 0x00U;

    tmpErr = HAL_UART_GetError(huart);
    if (HAL_UART_ERROR_NONE == tmpErr) {
        return;
    }

    switch (tmpErr) {
        case HAL_UART_ERROR_PE:
            __HAL_UART_CLEAR_PEFLAG(huart);
            break;
        case HAL_UART_ERROR_NE:
            __HAL_UART_CLEAR_NEFLAG(huart);
            break;
        case HAL_UART_ERROR_FE:
            __HAL_UART_CLEAR_FEFLAG(huart);
            break;
        case HAL_UART_ERROR_ORE:
            __HAL_UART_CLEAR_OREFLAG(huart);
            break;
        case HAL_UART_ERROR_DMA:

            break;
        default:
            break;
    }

    if (NULL != huart->hdmarx) {
        while (
            HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr, huart->RxXferSize)) {
            __HAL_UNLOCK(huart);
        }
    } else {
        /* 恢复接收地址指针到初始 buffer 位置 ，初始地址 = 当前地址 -
         * 已接收的数据个数，已接收的数据个数 = 需要接收数 - 还未接收数*/
        while (HAL_UART_Receive_IT(
            huart, huart->pRxBuffPtr - (huart->RxXferSize - huart->RxXferCount),
            huart->RxXferSize)) {
            __HAL_UNLOCK(huart);
        }
    }
}

/**
 * @brief 指定串口输出
 * @param huart 串口句柄指针
 * @param __format 欲输出的内容, 类似于printf格式
 */
void uart_print(UART_HandleTypeDef *huart, const char *__format, ...) {
    int len;
    va_list ap;
    va_start(ap, __format);

    /* 清空发送缓冲区 */
    memset(g_uart_tx_buf, 0x0, TX_BUF_LEN);

    /* 填充发送缓冲区 */
    vsnprintf((char *)g_uart_tx_buf, TX_BUF_LEN, (const char *)__format, ap);
    va_end(ap);
    len = strlen((const char *)g_uart_tx_buf);

    /* 往串口发送数据 */
    HAL_UART_Transmit(huart, (uint8_t *)&g_uart_tx_buf, (uint16_t)len, 0xFFFF);
}

/**
 * @}
 */

/*******************************************************************************
 * @defgroup 如果启用了串口1,重定义printf, 支持AC5, AC6, GCC
 * @{
 */

#ifdef EN_USART1
#if defined(__ARMCC_VERSION) /* Compiler */

#if (__ARMCC_VERSION >= 6010050) /* 使用AC6编译器时 */

__asm(".global __use_no_semihosting\n\t"); /* 声明不使用半主机模式 */
/* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");

#elif ((__ARMCC_VERSION >= 5000000) &&                                         \
       (__ARMCC_VERSION < 6000000)) /* 使用AC5编译器 */
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */

/* 关闭多字节警告 */
#pragma diag_suppress 870

#pragma import(__use_no_semihosting)

struct __FILE {
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
#endif /* __ARMCC_VERSION */
/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式
 */
int _ttywrch(int ch) {
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x) {
    x = x;
}

char *_sys_command_string(char *cmd, int len) {
    return NULL;
}
/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f) {
    while ((USART1->SR & 0X40) == 0)
        ; /* 等待上一个字符发送完成 */

    USART1->DR = (uint8_t)ch; /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}

#elif (defined(__GNUC__))            /* 使用ARM GCC编译器 */

/* 使用ARM GCC需要添加-u _printf_float以支持浮点数 */
#pragma import(__use_no_semihosting) /* 不适用半主机模式 */
/*重新定义__write函数*/
int _write(int fd, char *ptr, int len) {
    HAL_UART_Transmit(&g_usart1_handle, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}

/*定义_sys_exit()以避免使用半主机模式*/
void _sys_exit(int x) {
    x = x;
}

#endif /* Compiler */

#endif /* EN_USART1 */
