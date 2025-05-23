/**
 * SPDX-License-Identifier: Apache-2.0 
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <bao.h>
#include <cpu.h>
#include <arch/sbi.h>
#include <platform.h>

uint8_t bao_cpu_buf[PAGE_SIZE];
cpuid_t CPU_MASTER __attribute__((section(".data")));

void cpu_arch_print(){
    char buf[512]={0};
    sprintk(buf,"CPU_MASTER: %d\n", 1212);
}

extern char _dmem_phys_beg;
inline struct cpu* cpu() {
    /* 
        Warnning：这个功能需要读取mhartid，rv标准规定只能m模式读，
                所以要么opensbi支持，要么处理器实现的时候不要trap，
                不然就不要这个方法了
    */
    return (struct cpu*)&_dmem_phys_beg + CSRR(mhartid);
}

/* Perform architecture dependent cpu cores initializations */
void cpu_arch_init(cpuid_t cpuid, paddr_t load_addr)
{
    if (cpuid == CPU_MASTER) {
        sbi_init();
        for(size_t hartid = 0; hartid < platform.cpu_num; hartid++){
            if(hartid == cpuid) continue;
            struct sbiret ret = sbi_hart_start(hartid, load_addr, 0);
            if(ret.error < 0) {
                WARNING("failed to wake up hart %d", hartid);
            }
        }
    }
}

void cpu_arch_idle()
{
    asm volatile("wfi\n\t" ::: "memory");
    asm volatile("mv sp, %0\n\r"
                 "j cpu_idle_wakeup\n\r" ::"r"(&cpu()->stack[STACK_SIZE]));
    ERROR("returned from idle wake up");
}
