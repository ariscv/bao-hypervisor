/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <bao.h>
#include <cpu.h>
#include <vm.h>
#include <platform.h>
#include <arch/plic.h>
#include "arch/cpu.h"

void cpu_defines() __attribute__((used));
void cpu_defines()
{
    DEFINE_SIZE(CPU_SIZE, struct cpu);

    DEFINE_OFFSET(CPU_STACK_OFF, struct cpu, stack);
    DEFINE_SIZE(CPU_STACK_SIZE, ((struct cpu*)NULL)->stack);

    DEFINE_OFFSET(CPU_VCPU_OFF, struct cpu, vcpu);

    DEFINE_OFFSET(CPU_ARCH_OFF, struct cpu, arch);
}

void vcpu_defines() __attribute__((used));
void vcpu_defines()
{
    DEFINE_SIZE(VCPU_ARCH_SIZE, struct vcpu_arch);
    DEFINE_OFFSET(VCPU_REGS_OFF, struct vcpu, regs);
    DEFINE_SIZE(VCPU_REGS_SIZE, struct arch_regs);
}

void cpu_arch_defines() __attribute__((used));
void cpu_arch_defines()
{
    DEFINE_OFFSET(CPU_ARCH_TMPT0_OFF, struct cpu_arch, tmpt0);
}