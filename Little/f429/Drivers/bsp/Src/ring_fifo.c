/**
 * @file    ring_fifo.c
 * @author  mcdx
 * @brief   环形FIFO
 * @version 1.0
 * @date    2021-10-22
 */
#include "ring_fifo.h"

#include <string.h>

#define min(a, b)      ((a) > (b) ? (b) : (a))
#define fifo_max_depth (0xffffffff >> 1)

static inline uint32_t is_pow_of_2(uint32_t n) {
    return (0 != n) && (0 == (n & (n - 1)));
}

static inline uint32_t pow2gt(uint32_t x) {
    --x;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return x + 1;
}

ring_fifo_t *ring_fifo_init(void *buf, uint32_t size,
                            enum ring_fifo_type type) {
    ring_fifo_t *ring;

    if ((NULL != buf) && (0 == is_pow_of_2(size))) {
        return NULL;
    }

    ring = malloc(sizeof(ring_fifo_t));
    if (NULL == ring) {
        return NULL;
    }

    if (size > fifo_max_depth) {
        size = fifo_max_depth;
    }

    size = pow2gt(size);

    if (NULL == buf) {
        ring->buf = malloc(size);
        if (NULL == ring->buf) {
            free(ring);

            return NULL;
        }
        ring->is_dynamic = 1;
    } else {
        ring->buf = buf;
        ring->is_dynamic = 0;
    }

    ring->head = ring->tail = 0;
    ring->size = size;
    ring->mask = size - 1;
    ring->type = type;

    return ring;
}

void ring_fifo_destroy(ring_fifo_t *ring) {
    if (0 != ring->is_dynamic) {
        free(ring->buf);
        ring->buf = NULL;
    }

    free(ring);
}

uint32_t ring_fifo_write(ring_fifo_t *ring, const void *buf, uint32_t len) {
    uint32_t wlen;
    uint32_t unused;
    uint32_t off, l;
    uint32_t frame_off, skip;

    unused = ring->size - (ring->tail - ring->head);
    switch (ring->type) {
        case RF_TYPE_FRAME:
            frame_off = sizeof(uint32_t);
            skip = 0;
            if (ring->size - (ring->tail & ring->mask) < frame_off) {
                skip = ring->size - (ring->tail & ring->mask);
                /* 跳过尾部[1, frame_off - 1]字节 */
                frame_off += skip;
            }
            /* 如果不能存下此帧，丢弃 */
            if (len + frame_off > unused) {
                return 0;
            }
            wlen = len;
            if (0 == wlen) {
                return 0;
            }
            /* 写入帧长 */
            *(uint32_t *)(ring->buf + ((ring->tail + skip) & ring->mask)) =
                wlen;
            break;
        default: /* RF_TYPE_STREAM */
            frame_off = 0;
            wlen = min(len, unused);
            if (0 == wlen) {
                return 0;
            }
            break;
    }

    /* 计算写入位置 */
    off = (ring->tail + frame_off) & ring->mask;
    l = min(wlen, ring->size - off);
    memcpy((uint8_t *)ring->buf + off, buf, l);
    memcpy(ring->buf, (uint8_t *)buf + l, wlen - l);

    ring->tail += wlen + frame_off;

    return wlen;
}

uint32_t ring_fifo_read(ring_fifo_t *ring, void *buf, uint32_t len) {
    uint32_t rlen;
    uint32_t used;
    uint32_t off, l;
    uint32_t frame_off, skip;

    used = ring->tail - ring->head;
    switch (ring->type) {
        case RF_TYPE_FRAME:
            frame_off = sizeof(uint32_t);
            skip = 0;
            if (ring->size - (ring->head & ring->mask) < frame_off) {
                skip = ring->size - (ring->head & ring->mask);
                /* 跳过尾部[1, frame_off - 1]字节 */
                frame_off += skip;
            }
            if (0 == used) {
                return 0;
            }
            /* 读取帧长 */
            rlen =
                *(uint32_t *)(ring->buf + ((ring->head + skip) & ring->mask));
            /* 给定的缓冲区小于要读出的帧长 */
            if (len < rlen) {
                return 0;
            }
            break;
        default: /* RF_TYPE_STREAM */
            frame_off = 0;
            rlen = min(len, used);
            if (0 == rlen) {
                return 0;
            }
            break;
    }

    /* 计算读取位置 */
    off = (ring->head + frame_off) & ring->mask;
    l = min(rlen, ring->size - off);
    memcpy(buf, (uint8_t *)ring->buf + off, l);
    memcpy((uint8_t *)buf + l, ring->buf, rlen - l);

    ring->head += rlen + frame_off;

    return rlen;
}

uint32_t ring_fifo_is_full(ring_fifo_t *ring) {
    return ring->size == (ring->tail - ring->head);
}

uint32_t ring_fifo_is_empty(ring_fifo_t *ring) {
    return ring->tail == ring->head;
}

uint32_t ring_fifo_avail(ring_fifo_t *ring) {
    return ring->size - (ring->tail - ring->head);
}

uint32_t ring_fifo_count(ring_fifo_t *ring) {
    return ring->tail - ring->head;
}
