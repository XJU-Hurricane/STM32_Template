/**
 ****************************************************************************************************
 * @file        delay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-02-25
 * @brief       使用SysTick的普通计数模式对延迟进行管理(支持ucosii)
 *              提供delay_init初始化函数， delay_us和delay_ms等延时函数
 * @license     Copyright (c) 2022-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20230206
 * 第一次发布
 * V1.1 20230225
 * 修改SYS_SUPPORT_OS部分代码, 默认仅支持UCOSII 2.93.01版本, 其他OS请参考实现
 * 修改delay_init不再使用8分频,全部统一使用MCU时钟
 * 修改delay_us使用时钟摘取法延时, 兼容OS
 * 修改delay_ms直接使用delay_us延时实现.
 *
 ****************************************************************************************************
 */

#include "delay.h"
#include "sys.h"

static uint32_t g_fac_us = 0; /* us延时倍乘数 */

/* 如果SYS_SUPPORT_OS定义了,说明要支持OS了(不限于UCOS) */
#if SYS_SUPPORT_OS
    #include "FreeRTOS.h"
    #include "task.h"
extern void xPortSysTickHandler(void);

/**
 * @brief     systick中断服务函数,使用OS时用到
 * @param     ticks : 延时的节拍数
 * @retval    无
 */
void SysTick_Handler(void) {
    HAL_IncTick();
    /* OS 开始跑了,才执行正常的调度处理 */
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}
#endif

/**
 * @brief     初始化延迟函数
 * @param     sysclk: 系统时钟频率, 即CPU频率(rcc_c_ck), 168MHz
 * @retval    无
 */
void delay_init(uint16_t sysclk) {
#if SYS_SUPPORT_OS /* 如果需要支持OS */
    uint32_t reload;
#endif

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    g_fac_us = sysclk;

#if SYS_SUPPORT_OS   /* 如果需要支持OS. */
    reload = sysclk; /* 每秒钟的计数次数 单位为M */
    reload *=
        1000000 /
        configTICK_RATE_HZ; /* 根据delay_ostickspersec设定溢出时间,reload为24位
                             * 寄存器,最大值:16777216,在180M下,约合0.0932s左右
                             */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; /* 开启SYSTICK中断 */
    SysTick->LOAD = reload; /* 每1/delay_ostickspersec秒中断一次 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; /* 开启SYSTICK */
#endif
}

#if SYS_SUPPORT_OS /* 如果需要支持OS. */

/**
 * @brief     延时nus
 * @note      无论是否使用OS, 都是用时钟摘取法来做us延时
 * @param     nus: 要延时的us数
 * @note      nus取值范围: 0 ~ (2^32 / fac_us) (fac_us一般等于系统主频,
 *            自行套入计算)
 * @retval    无
 */
void delay_us(uint32_t nus) {
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; /* LOAD的值 */

    ticks = nus * g_fac_us; /* 需要的节拍数 */
    told = SysTick->VAL;    /* 刚进入时的计数器值 */

    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
                /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break; /* 时间超过/等于要延迟的时间,则退出 */
            }
        }
    }
}

/**
 * @brief     延时nms
 * @param     nms: 要延时的ms数 (0< nms <= (2^32 / fac_us /
 *            1000))(fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_ms(uint16_t nms) {
    for (uint32_t i = 0; i < nms; ++i) {
        delay_us(1000);
    }
}

#else

/**
 * @brief       延时nus
 * @param       nus: 要延时的us数.
 * @note        注意: nus的值,不要大于93206us(最大值即2^24 / g_fac_us @g_fac_us
 * = 180)
 * @retval      无
 */
void delay_us(uint32_t nus) {
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; /* LOAD的值 */
    ticks = nus * g_fac_us;          /* 需要的节拍数 */
    told = SysTick->VAL;             /* 刚进入时的计数器值 */
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
                /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break; /* 时间超过/等于要延迟的时间,则退出 */
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
    uint32_t repeat =
        nms / 540; /* 这里用540,是考虑到可能有超频应用,
                      比如248M的时候,delay_us最大只能延时541ms左右了 */
    uint32_t remain = nms % 540;

    while (repeat) {
        delay_us(540 * 1000); /* 利用delay_us 实现 540ms 延时 */
        repeat--;
    }

    if (remain) {
        delay_us(remain * 1000); /* 利用delay_us, 把尾数延时(remain ms)给做了 */
    }
}

/**
 * @brief       HAL库内部函数用到的延时
 * @note
 * HAL库的延时默认用Systick，如果我们没有开Systick的中断会导致调用这个延时后无法退出
 * @param       Delay : 要延时的毫秒数
 * @retval      无
 */
void HAL_Delay(uint32_t Delay) {
    delay_ms(Delay);
}

#endif