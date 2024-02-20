/**
 * @file    ring_fifo.h
 * @author  mcdx
 * @brief   环形FIFO
 * @version 1.0
 * @date    2021-10-22
 */

#ifndef __RING_FIFO_H
#define __RING_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

/* ring type */
enum ring_fifo_type {
    RF_TYPE_FRAME,
    RF_TYPE_STREAM
};

/* 环形缓冲区结构 */
typedef struct {
    volatile uint32_t head; /* 消费者指针 */
    volatile uint32_t tail; /* 生产者指针 */

    uint32_t size; /* 缓冲区的大小 */
    uint32_t mask; /* 缓冲区的大小掩码 */

    void *buf;           /* 缓冲区指针 */
    uint32_t is_dynamic; /* 是否使用了动态内存 */

    enum ring_fifo_type type; /* fifo的类型 */
} ring_fifo_t;

/**
 * @brief    初始化环形缓冲区
 * @param[in]    buf     缓冲区指针，如果为NULL，则默认使用堆内存进行分配
 * @param[in]    size    缓冲区长度
 * @param[in]    type    fifo类型
 * @retval   执行结果
 * -         NULL    内存分配失败，或buf非NULL时指定的size不为2的幂次方
 * -         非NULL  初始化成功
 */
ring_fifo_t *ring_fifo_init(void *buf, uint32_t size, enum ring_fifo_type type);

/**
 * @brief    销毁环形缓冲区
 * @param[in]    ring    环形缓冲区句柄
 */
void ring_fifo_destroy(ring_fifo_t *ring);

/**
 * @brief    写入到环形缓冲区(单生产者无锁)
 * @param[in]    ring    环形缓冲区句柄
 * @param[in]    buf     指向待写入数据
 * @param[in]    len     待写入数据长度(byte)
 * @retval   执行结果
 * -         成功写入的长度(byte)
 */
uint32_t ring_fifo_write(ring_fifo_t *ring, const void *buf, uint32_t len);

/**
 * @brief    从环形缓冲区读出(单消费者无锁)
 * @param[in]    ring    环形缓冲区句柄
 * @param[out]   buf     存放待读出数据
 * @param[in]    len     存放待读出数据缓冲区的长度(byte)
 * @retval   执行结果
 * -         成功读出的长度(byte)
 */
uint32_t ring_fifo_read(ring_fifo_t *ring, void *buf, uint32_t len);

/**
 * @brief    环形缓冲区是否为满
 * @param[in]    ring    环形缓冲区句柄
 * @retval   执行结果
 * -         0   未满
 * -         1   满
 */
uint32_t ring_fifo_is_full(ring_fifo_t *ring);

/**
 * @brief    环形缓冲区是否为空
 * @param[in]    ring    环形缓冲区句柄
 * @retval   执行结果
 * -         0   非空
 * -         1   空
 */
uint32_t ring_fifo_is_empty(ring_fifo_t *ring);

/**
 * @brief    获取环形缓冲区未使用内存大小(byte)
 * @param[in]    ring    环形缓冲区句柄
 * @retval   执行结果
 * -         环形缓冲区未使用内存大小(byte)
 */
uint32_t ring_fifo_avail(ring_fifo_t *ring);

/**
 * @brief    获取环形缓冲区可读取内存大小(byte)
 * @param[in]    ring    环形缓冲区句柄
 * @retval   执行结果
 * -         环形缓冲区可读取内存大小(byte)
 */
uint32_t ring_fifo_count(ring_fifo_t *ring);

#ifdef __cplusplus
}
#endif

#endif /*__RING_FIFO_H*/
