## SPDX-License-Identifier: Apache-2.0
## Copyright (c) Bao Project and Contributors. All rights reserved.

# Architecture definition
ARCH:=riscv32
# CPU definition
CPU:=

# drivers := sbi_uart
drivers := 8250_uart

platform_description:=virt_desc.c

ifeq ($(drivers),8250_uart)
# $(error 8250_uart driver not supported for this platform)
uart-macro-cflags := -DCONFIG_USE_8250_UART
endif

platform-cppflags = $(uart-macro-cflags)
platform-cflags = $(uart-macro-cflags)
platform-asflags = $(uart-macro-cflags)
platform-ldflags =
