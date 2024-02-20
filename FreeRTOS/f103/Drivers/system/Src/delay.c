/**
 ****************************************************************************************************
 * @file        delay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       使用SysTick的普通计数模式对延迟进行管理(支持ucosii)
 *              提供delay_init初始化函数， delay_us和delay_ms等延时函数
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "delay.h"

static uint16_t g_fac_us = 0; /* us延时倍乘数 */

/* 如果SYS_SUPPORT_OS定义了,说明要支持OS了(不限于UCOS) */
#if SYS_SUPPORT_OS

/* 添加公共头文件 (FreeRTOS 需要用到) */
#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);

/**
 * @brief       systick中断服务函数,使用OS时用到
 * @param       ticks: 延时的节拍数
 * @retval      无
 */
void SysTick_Handler(void) {
    HAL_IncTick();
    if (xTaskGetSchedulerState() !=
        taskSCHEDULER_NOT_STARTED) /* OS开始跑了,才执行正常的调度处理 */
    {
        xPortSysTickHandler();
    }
}

#endif

/**
 * @brief       初始化延迟函数
 * @param       sysclk: 系统时钟频率, 即CPU频率(HCLK)
 * @retval      无
 */
void delay_init(uint16_t sysclk) {
#if SYS_SUPPORT_OS /* 如果需要支持OS. */
    uint32_t reload;
#endif
    SysTick->CTRL =
        0; /* 清Systick状态，以便下一步重设，如果这里开了中断会关闭其中断 */
    HAL_SYSTICK_CLKSourceConfig(
        SYSTICK_CLKSOURCE_HCLK_DIV8); /* SYSTICK使用内核时钟源8分频,因systick的计数器最大值只有2^24
                                       */

    g_fac_us =
        sysclk / 8; /* 不论是否使用OS,g_fac_us都需要使用,作为1us的基础时基 */
#if SYS_SUPPORT_OS       /* 如果需要支持OS. */
    reload = sysclk / 8; /* 每秒钟的计数次数 单位为M */
    reload *=
        1000000 /
        configTICK_RATE_HZ;  /* 根据delay_ostickspersec设定溢出时间
                              * reload为24位寄存器,最大值:16777216,在9M下,约合1.86s左右
                              */
    SysTick->CTRL |= 1 << 1; /* 开启SYSTICK中断 */
    SysTick->LOAD = reload;  /* 每1/delay_ostickspersec秒中断一次 */
    SysTick->CTRL |= 1 << 0; /* 开启SYSTICK */
#endif
}

#if SYS_SUPPORT_OS /* 如果需要支持OS, 用以下代码 */

/**
 * @brief       延时nus
 * @param       nus: 要延时的us数.
 * @note        nus取值范围: 0 ~ 477218588(最大值即2^32 / g_fac_us @g_fac_us =
 * 9)
 * @retval      无
 */
void delay_us(uint32_t nus) {
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD; /* LOAD的值 */
    ticks = nus * g_fac_us; /* 需要的节拍数 */
    told = SysTick->VAL;    /* 刚进入时的计数器值 */

    while (1) {
        tnow = SysTick->VAL;

        if (tnow != told) {
            if (tnow < told) {
                tcnt +=
                    told -
                    tnow; /* 这里注意一下SYSTICK是一个递减的计数器就可以了. */
            } else {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks) {
                break; /* 时间超过/等于要延迟的时间,则退出. */
            }
        }
    }
}

/**
 * @brief       延时nms
 * @param       nms: 要延时的ms数 (0< nms <= 65535)
 * @retval      无
 */
void delay_ms(uint16_t nms) {
    uint32_t i;

    for (i = 0; i < nms; i++) {
        delay_us(1000);
    }
}

#else /* 不使用OS时, 用以下代码 */

/**
 * @brief       延时nus
 * @param       nus: 要延时的us数.
 * @note        注意: nus的值,不要大于1864135us(最大值即2^24 / g_fac_us
 * @g_fac_us = 9)
 * @retval      无
 */
void delay_us(uint32_t nus) {
    uint32_t temp;
    SysTick->LOAD = nus * g_fac_us; /* 时间加载 */
    SysTick->VAL = 0x00;            /* 清空计数器 */
    SysTick->CTRL |= 1 << 0;        /* 开始倒数 */

    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) &&
             !(temp & (1 << 16))); /* CTRL.ENABLE位必须为1, 并等待时间到达 */

    SysTick->CTRL &= ~(1 << 0); /* 关闭SYSTICK */
    SysTick->VAL = 0X00;        /* 清空计数器 */
}

/**
 * @brief       延时nms
 * @param       nms: 要延时的ms数 (0< nms <= 65535)
 * @retval      无
 */
void delay_ms(uint16_t nms) {
    uint32_t repeat =
        nms / 1000; /*  这里用1000,是考虑到可能有超频应用,
                     *  比如128Mhz的时候, delay_us最大只能延时1048576us左右了
                     */
    uint32_t remain = nms % 1000;

    while (repeat) {
        delay_us(1000 * 1000); /* 利用delay_us 实现 1000ms 延时 */
        repeat--;
    }

    if (remain) {
        delay_us(remain * 1000); /* 利用delay_us, 把尾数延时(remain ms)给做了 */
    }
}

/**
  * @brief HAL库内部函数用到的延时
           HAL库的延时默认用Systick，如果我们没有开Systick的中断会导致调用这个延时后无法退出
  * @param Delay 要延时的毫秒数
  * @retval None
  */
void HAL_Delay(uint32_t Delay) {
    delay_ms(Delay);
}

#endif
