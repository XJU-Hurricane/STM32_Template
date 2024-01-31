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

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
    #include "os.h" /* os 使用 */
#endif              /* SYS_SUPPORT_OS */

static uint8_t USART_TX_Buf[TX_BUF_LEN];

/*******************************************************************************
 * @defgroup 串口1
 * @{
 */

#ifdef EN_USART1
UART_HandleTypeDef USART1_Handler;
    #if EN_USART1_RX
        #if !USART1_USE_DMA_RX
uint8_t USART1_RX_BUF[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t USART1_RX_STA = 0;
uint8_t g_usart1_rx_buf[RXBUFFERSIZE];
        #endif /* !USART1_USE_DMA_RX */
/**
 * @brief 串口1中断服务函数
 */
void USART1_IRQHandler(void) {
        #if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
        #endif /* SYS_SUPPORT_OS */

        #if USART1_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&USART1_Handler, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&USART1_Handler);
        UART_DMARX_Idle_Callback(&USART1_Handler);
    }
        #endif /* USART1_USE_IDLE_IT */

    HAL_UART_IRQHandler(&USART1_Handler); /* 调用HAL库中断处理公用函数 */

        #if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
        #endif /* SYS_SUPPORT_OS */
}
    #endif /* EN_USART1_RX */
/**
 * @brief 串口1初始化
 * @param bound 波特率
 */
void USART1_Init(uint32_t bound) {
    USART1_Handler.Instance = USART1;
    USART1_Handler.Init.BaudRate = bound;
    USART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART1_Handler.Init.StopBits = UART_STOPBITS_1;
    USART1_Handler.Init.Parity = UART_PARITY_NONE;
    USART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    USART1_Handler.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&USART1_Handler);

    #if USART1_USE_DMA_TX
    /* 打开DMA发送 */
    USART1_TX_DMA_Init(2048, 128);
    #endif /* USART1_USE_DMA_TX */

    #if EN_USART1_RX
        #if USART1_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    USART1_RX_DMA_Init(4096, 128);
        #else  /* USART1_USE_DMA_RX */
    HAL_UART_Receive_IT(&USART1_Handler, (uint8_t *)g_usart1_rx_buf,
                        RXBUFFERSIZE);
        #endif /* USART1_USE_DMA_RX */
    #endif     /* EN_USART1_RX */
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
UART_HandleTypeDef USART2_Handler;
    #if EN_USART2_RX
        #if !USART2_USE_DMA_RX
uint8_t USART2_RX_BUF[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t USART2_RX_STA = 0;
uint8_t g_usart2_rx_buf[RXBUFFERSIZE];
        #endif /* !USART2_USE_DMA_RX */
/**
 * @brief 串口2中断服务函数
 */
void USART2_IRQHandler(void) {
        #if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
        #endif /* SYS_SUPPORT_OS */

        #if USART2_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&USART2_Handler, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&USART2_Handler);
        UART_DMARX_Idle_Callback(&USART2_Handler);
    }
        #endif /* USART2_USE_IDLE_IT */

    HAL_UART_IRQHandler(&USART2_Handler); /* 调用HAL库中断处理公用函数 */

        #if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
        #endif /* SYS_SUPPORT_OS */
}
    #endif /* EN_USART2_RX */
/**
 * @brief 串口2初始化
 * @param bound 波特率
 */
void USART2_Init(uint32_t bound) {
    USART2_Handler.Instance = USART2;
    USART2_Handler.Init.BaudRate = bound;
    USART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART2_Handler.Init.StopBits = UART_STOPBITS_1;
    USART2_Handler.Init.Parity = UART_PARITY_NONE;
    USART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    USART2_Handler.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&USART2_Handler);

    #if USART2_USE_DMA_TX
    /* 打开DMA发送 */
    USART2_TX_DMA_Init(2048, 128);
    #endif /* USART2_USE_DMA_TX */

    #if EN_USART2_RX
        #if USART2_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    USART2_RX_DMA_Init(4096, 128);
        #else  /* USART2_USE_DMA_RX */
    HAL_UART_Receive_IT(&USART2_Handler, (uint8_t *)g_usart2_rx_buf,
                        RXBUFFERSIZE);
        #endif /* USART2_USE_DMA_RX */
    #endif     /* EN_USART2_RX */
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
UART_HandleTypeDef USART3_Handler;
    #if EN_USART3_RX
        #if !USART3_USE_DMA_RX
uint8_t USART3_RX_BUF[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t USART3_RX_STA = 0;
uint8_t g_usart3_rx_buf[RXBUFFERSIZE];
        #endif /* !USART3_USE_DMA_RX */
/**
 * @brief 串口3中断服务函数
 */
void USART3_IRQHandler(void) {
        #if SYS_SUPPORT_OS /* 使用OS */
    OSIntEnter();
        #endif /* SYS_SUPPORT_OS */

        #if USART3_USE_IDLE_IT
    if (__HAL_UART_GET_FLAG(&USART3_Handler, UART_FLAG_IDLE)) {
        __HAL_USART_CLEAR_IDLEFLAG(&USART3_Handler);
        UART_DMARX_Idle_Callback(&USART3_Handler);
    }
        #endif /* USART3_USE_IDLE_IT */

    HAL_UART_IRQHandler(&USART3_Handler); /* 调用HAL库中断处理公用函数 */

        #if SYS_SUPPORT_OS /* 使用OS */
    OSIntExit();
        #endif /* SYS_SUPPORT_OS */
}
    #endif /* EN_USART3_RX */
/**
 * @brief 串口3初始化
 * @param bound 波特率
 */
void USART3_Init(uint32_t bound) {
    USART3_Handler.Instance = USART3;
    USART3_Handler.Init.BaudRate = bound;
    USART3_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART3_Handler.Init.StopBits = UART_STOPBITS_1;
    USART3_Handler.Init.Parity = UART_PARITY_NONE;
    USART3_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    USART3_Handler.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&USART3_Handler);

    #if USART3_USE_DMA_TX
    /* 打开DMA发送 */
    USART3_TX_DMA_Init(2048, 128);
    #endif /* USART3_USE_DMA_TX */

    #if EN_USART3_RX
        #if USART3_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    USART3_RX_DMA_Init(4096, 128);
        #else  /* USART3_USE_DMA_RX */
    HAL_UART_Receive_IT(&USART3_Handler, (uint8_t *)g_usart3_rx_buf,
                        RXBUFFERSIZE);
        #endif /* USART3_USE_DMA_RX */
    #endif     /* EN_USART3_RX */
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
uint8_t UART4_RX_BUF[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t UART4_RX_STA = 0;
uint8_t g_uart4_rx_buf[RXBUFFERSIZE];
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
        UART_DMARX_Idle_Callback(&UART4_Handler);
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
void UART4_Init(uint32_t bound) {
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
    UART4_TX_DMA_Init(2048, 128);
    #endif /* UART4_USE_DMA_TX */

    #if EN_UART4_RX
        #if UART4_USE_DMA_RX
    /* 使用DMA, 打开DMA接收 */
    UART4_RX_DMA_Init(4096, 128);
        #else  /* UART4_USE_DMA_RX */
    HAL_UART_Receive_IT(&UART4_Handler, (uint8_t *)g_uart4_rx_buf,
                        RXBUFFERSIZE);
        #endif /* UART4_USE_DMA_RX */
    #endif     /* EN_UART4_RX */
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
uint8_t UART5_RX_BUF[USART_REC_LEN];
/**
 * 接收状态
 * bit15: 接收完成标志; bit14: 接收到0x0d
 * bit13~0: 接收到的有效字节数目
 */
uint16_t UART5_RX_STA = 0;
uint8_t g_uart5_rx_buf[RXBUFFERSIZE];
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
void UART5_Init(uint32_t bound) {
    UART5_Handler.Instance = UART5;
    UART5_Handler.Init.BaudRate = bound;
    UART5_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART5_Handler.Init.StopBits = UART_STOPBITS_1;
    UART5_Handler.Init.Parity = UART_PARITY_NONE;
    UART5_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART5_Handler.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&UART5_Handler);
    #if EN_UART5_RX
    HAL_UART_Receive_IT(&UART5_Handler, (uint8_t *)g_uart5_rx_buf,
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
    GPIO_InitTypeDef GPIO_Initure;
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
    UART_DMATX_Clear_TC_Flag(huart);
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
        if ((USART1_RX_STA & 0x8000) == 0) {
            /* 接收未完成 */
            if (USART1_RX_STA & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_usart1_rx_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    USART1_RX_STA = 0; /* 接收错误,重新开始 */
                } else {
                    USART1_RX_STA |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_usart1_rx_buf[0] == 0x0d) {
                    USART1_RX_STA |= 0x4000;
                } else {
                    USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = g_usart1_rx_buf[0];
                    USART1_RX_STA++;
                    if (USART1_RX_STA > (USART_REC_LEN - 1)) {
                        USART1_RX_STA = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&USART1_Handler, (uint8_t *)g_usart1_rx_buf,
                            RXBUFFERSIZE);
    #else  /* !USART1_USE_DMA_RX */
        UART_DMARX_Done_Callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
    #endif /* !USART1_USE_DMA_RX */
#endif     /* EN_USART1_RX */
    } else if (huart->Instance == USART2) {
#if EN_USART2_RX
    #if !USART2_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((USART2_RX_STA & 0x8000) == 0) {
            /* 接收未完成 */
            if (USART2_RX_STA & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_usart2_rx_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    USART2_RX_STA = 0; /* 接收错误,重新开始 */
                } else {
                    USART2_RX_STA |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_usart2_rx_buf[0] == 0x0d) {
                    USART2_RX_STA |= 0x4000;
                } else {
                    USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = g_usart2_rx_buf[0];
                    USART2_RX_STA++;
                    if (USART2_RX_STA > (USART_REC_LEN - 1)) {
                        USART2_RX_STA = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&USART2_Handler, (uint8_t *)g_usart2_rx_buf,
                            RXBUFFERSIZE);
    #else  /* !USART2_USE_DMA_RX */
        UART_DMARX_Done_Callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
    #endif /* !USART2_USE_DMA_RX */
#endif     /* EN_USART2_RX */
    } else if (huart->Instance == USART3) {
#if EN_USART3_RX
    #if !USART3_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((USART3_RX_STA & 0x8000) == 0) {
            /* 接收未完成 */
            if (USART3_RX_STA & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_usart3_rx_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    USART3_RX_STA = 0; /* 接收错误,重新开始 */
                } else {
                    USART3_RX_STA |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_usart3_rx_buf[0] == 0x0d) {
                    USART3_RX_STA |= 0x4000;
                } else {
                    USART3_RX_BUF[USART3_RX_STA & 0X3FFF] = g_usart3_rx_buf[0];
                    USART3_RX_STA++;
                    if (USART3_RX_STA > (USART_REC_LEN - 1)) {
                        USART3_RX_STA = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&USART3_Handler, (uint8_t *)g_usart3_rx_buf,
                            RXBUFFERSIZE);
    #else  /* !USART3_USE_DMA_RX */
        UART_DMARX_Done_Callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
    #endif /* !USART3_USE_DMA_RX */
#endif     /* EN_USART3_RX */
    } else if (huart->Instance == UART4) {
#if EN_UART4_RX
    #if !UART4_USE_DMA_RX
        /**
         * 以下用于测试中断回调是否有问题,根据实际情况修改
         * 也可以将接收写在中断服务函数中
         */
        if ((UART4_RX_STA & 0x8000) == 0) {
            /* 接收未完成 */
            if (UART4_RX_STA & 0x4000) {
                /* 接收到了0x0d(即\r) */
                if (g_uart4_rx_buf[0] != 0x0a) {
                    /* 不是0x0a(即\n) */
                    UART4_RX_STA = 0; /* 接收错误,重新开始 */
                } else {
                    UART4_RX_STA |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0x0d(即\r) */
                if (g_uart4_rx_buf[0] == 0x0d) {
                    UART4_RX_STA |= 0x4000;
                } else {
                    UART4_RX_BUF[UART4_RX_STA & 0X3FFF] = g_uart4_rx_buf[0];
                    UART4_RX_STA++;
                    if (UART4_RX_STA > (USART_REC_LEN - 1)) {
                        UART4_RX_STA = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&UART4_Handler, (uint8_t *)g_uart4_rx_buf,
                            RXBUFFERSIZE);
    #else  /* !UART4_USE_DMA_RX */
        UART_DMARX_Done_Callback(huart);
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR) {
            while (HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                                        huart->RxXferSize)) {
                __HAL_UNLOCK(huart);
            }
        }
    #endif /* !UART4_USE_DMA_RX */
#endif     /* EN_UART4_RX */
    } else if (huart->Instance == UART5) {
#if EN_UART5_RX
        if ((UART5_RX_STA & 0x8000) == 0) {
            /* 接收未完成 */
            if (UART5_RX_STA & 0x4000) {
                /* 接收到了0x0d */
                if (g_uart5_rx_buf[0] != 0x0a) {
                    UART5_RX_STA = 0; /* 接收错误,重新开始 */
                } else {
                    UART5_RX_STA |= 0x8000; /* 接收完成了 */
                }
            } else {
                /* 还没收到0X0D */
                if (g_uart5_rx_buf[0] == 0x0d) {
                    UART5_RX_STA |= 0x4000;
                } else {
                    UART5_RX_BUF[UART5_RX_STA & 0X3FFF] = g_uart5_rx_buf[0];
                    UART5_RX_STA++;
                    if (UART5_RX_STA > (USART_REC_LEN - 1)) {
                        UART5_RX_STA = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
            HAL_UART_Receive_IT(&UART5_Handler, (uint8_t *)g_uart5_rx_buf,
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
    UART_DMARX_HalfDone_Callback(huart);
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
    memset(USART_TX_Buf, 0x0, TX_BUF_LEN);

    /* 填充发送缓冲区 */
    vsnprintf((char *)USART_TX_Buf, TX_BUF_LEN, (const char *)__format, ap);
    va_end(ap);
    len = strlen((const char *)USART_TX_Buf);

    /* 往串口发送数据 */
    HAL_UART_Transmit(huart, (uint8_t *)&USART_TX_Buf, (uint16_t)len, 0xFFFF);
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

        #elif ((__ARMCC_VERSION >= 5000000) &&                                 \
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

    #elif (defined(__GNUC__))                /* 使用ARM GCC编译器 */

    /* 使用ARM GCC需要添加-u _printf_float以支持浮点数 */
        #pragma import(__use_no_semihosting) /* 不适用半主机模式 */
/*重新定义__write函数*/
int _write(int fd, char *ptr, int len) {
    HAL_UART_Transmit(&USART1_Handler, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}

/*定义_sys_exit()以避免使用半主机模式*/
void _sys_exit(int x) {
    x = x;
}

    #endif /* Compiler */

#endif /* EN_USART1 */
