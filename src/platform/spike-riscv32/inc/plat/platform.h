/**
 * SPDX-License-Identifier: Apache-2.0 
 * Copyright (c) Bao Project and Contributors. All rights reserved
 */

#ifndef __PLAT_PLATFORM_H__
#define __PLAT_PLATFORM_H__

#define UART8250_REG_WIDTH  1

#ifdef CONFIG_USE_8250_UART
#include <drivers/8250_uart.h>
#else
#include <drivers/sbi_uart.h>
#endif

#endif
