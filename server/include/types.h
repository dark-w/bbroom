#pragma once

#include "config.h"

typedef unsigned char __u8, u8, byte, uint8_t, u_char;
typedef unsigned short __u16, u16, uint16_t;
typedef unsigned int __u32, u32, uint32_t, umode_t;
typedef unsigned long size_t, u_long;

typedef unsigned long long u64;

// FIXME
typedef unsigned short __le16;
typedef unsigned int __le32;

typedef enum { false, true } bool;

#define ALIGN_UP(len, align)                                                   \
    do {                                                                       \
        typeof(len) _________tmp;                                              \
        if (!((align) & ((align)-1))) {                                        \
            (len) = (len + align - 1) & ~((align)-1);                          \
        } else if ((_________tmp = (len) % (align))) {                         \
            (len) += (align)-_________tmp;                                     \
        }                                                                      \
    } while (0)

// #define NULL ((void *)0)

#define container_of(ptr, type, member)                                        \
    (type *)((char *)ptr - (long)(&((type *)0)->member))

#define WORD_SIZE sizeof(long)
#define WORD_ALIGN_UP(addr) (((addr) + WORD_SIZE - 1) & ~(WORD_SIZE - 1))
#define WORD_ALIGN_DOWN(addr) ((addr) & ~(WORD_SIZE - 1))
#define DWORD_SIZE (WORD_SIZE << 1)
#define WORD_BITS (WORD_SIZE * 8)

#define __UNUSED__ __attribute__((unused))
