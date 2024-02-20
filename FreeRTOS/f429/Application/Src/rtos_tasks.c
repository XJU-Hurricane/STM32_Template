/**
 * @file    rtos_tasks.c
 * @author  Deadline039
 * @brief   RTOS任务
 * @version 0.1
 * @date    2024-01-31
 */

#include "rtos_tasks.h"
#include "FreeRTOS.h"
#include "led.h"
#include "task.h"
#include "uart.h"

/*****************************************************************************/

/**
 * START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                /* 任务优先级 */
#define START_STK_SIZE  128              /* 任务堆栈大小 */
static TaskHandle_t g_start_task_handle; /* 任务句柄 */
void start_task(void *pvParameters);     /* 任务函数 */

/**
 * TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO     2            /* 任务优先级 */
#define TASK1_STK_SIZE 128          /* 任务堆栈大小 */
static TaskHandle_t g_task1_handle; /* 任务句柄 */
void task1(void *pvParameters);     /* 任务函数 */

/**
 * TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO     2            /* 任务优先级 */
#define TASK2_STK_SIZE 128          /* 任务堆栈大小 */
static TaskHandle_t g_task2_handle; /* 任务句柄 */
void task2(void *pvParameters);     /* 任务函数 */

/*****************************************************************************/

/**
 * @brief FreeRTOS启动函数
 *
 */
void freertos_start(void) {
    xTaskCreate((TaskFunction_t)start_task, /* 任务函数 */
                (const char *)"start_task", /* 任务名称 */
                (uint16_t)START_STK_SIZE,   /* 任务堆栈大小 */
                (void *)NULL,               /* 传入给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,          /* 任务优先级 */
                (TaskHandle_t *)&g_start_task_handle); /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief 启动任务
 *
 * @param pvParameters 传入参数(未用到)
 */
void start_task(void *pvParameters) {
    UNUSED(pvParameters);
    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t)task1, (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE, (void *)(NULL),
                (UBaseType_t)TASK1_PRIO, (TaskHandle_t *)&g_task1_handle);

    xTaskCreate((TaskFunction_t)task2, (const char *)"task2",
                (uint16_t)TASK2_STK_SIZE, (void *)(NULL),
                (UBaseType_t)TASK2_PRIO, (TaskHandle_t *)&g_task2_handle);

    vTaskDelete(g_start_task_handle);
    taskEXIT_CRITICAL();
}

/**
 * @brief 任务1 跑马灯
 *
 * @param pvParameters 传入参数(未用到)
 */
void task1(void *pvParameters) {
    UNUSED(pvParameters);

    LED0_TOGGLE();
    while (1) {
        LED0_TOGGLE();
        LED1_TOGGLE();
        vTaskDelay(1000);
    }
}

/**
 * @brief 任务2, 串口发送"FreeRTOS工程模板"
 *
 * @param pvParameters 传入参数(未用到)
 */
void task2(void *pvParameters) {
    UNUSED(pvParameters);

    while (1) {
        printf("FreeRTOS工程模板\r\n");
        vTaskDelay(1000);
    }
}
