/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef __ARCH_MEM_H__
#define __ARCH_MEM_H__

#include <bao.h>
#include <arch/page_table.h>

/* 
	for arm-r52,
		typedef union {
		    unsigned long raw;
		    struct {
		        uint16_t prbar;
		        uint16_t prlar;
		    };
		} mem_flags_t;
*/
typedef uint32_t mem_flags_t;

static inline const size_t mpu_granularity()
{
    return (size_t) PAGE_SIZE;
}

#endif /* __ARCH_MEM_H__ */
