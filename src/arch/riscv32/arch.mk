## SPDX-License-Identifier: Apache-2.0
## Copyright (c) Bao Project and Contributors. All rights reserved.

CROSS_COMPILE ?=  riscv64-linux-gnu-

arch-cppflags = 
arch-cflags = -fno-pic \
				-mstrict-align -mcmodel=medany \
				-march=rv32ima_zicsr -mabi=ilp32 

# arch-cflags = -fno-pic \
# 				-mstrict-align -mcmodel=medany \
# 				-march=rv64ima_zicsr -mabi=lp64 

arch-asflags =
arch-ldflags = -melf32lriscv

arch_mem_prot:=mmu
PAGE_SIZE:=0x1000
