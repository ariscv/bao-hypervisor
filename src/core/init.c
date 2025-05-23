/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <bao.h>

#include <cpu.h>
#include <mem.h>
#include <interrupts.h>
#include <console.h>
#include <printk.h>
#include <platform.h>
#include <vmm.h>
struct cpu *cpu_m = NULL;
void init(cpuid_t cpu_id, paddr_t load_addr)
{
    /* 
        在进入虚拟机之前，bao里面不应该，也不能产生trap，
        因为trap的上下文是基于vcpu的，启动前的vcpu还未初始化。
    */
    /**
     * These initializations must be executed first and in fixed order.
     */

    cpu_init(cpu_id, load_addr);
    mem_init(load_addr);

    /* -------------------------------------------------------------- */

    console_init();
    
    if (cpu()->id == CPU_MASTER) {
        printk("Bao Hypervisor\n\r");
        // struct vcpu vcpu_m={0};
        // cpu_m->vcpu=&vcpu_m;
        // cpu_m = cpu();
        
    }
    *(volatile uint32_t* )0x0c000000 = 0xdeadbeef;
    BAO_LOG("0x0c000000=%lx\n\r",*(volatile uint32_t* )0x0c000000);
    printk("CPU %d initialized\n\r", cpu()->id);
    
    // struct vcpu vcpu_m={0};
    cpu_m = cpu();
        // cpu_m->vcpu=&vcpu_m;
        
    // cpu_print(cpu_m);
    // BAO_LOG("WFI");
    //     asm volatile("wfi");

    // interrupts_init();
// BAO_LOG("id=%d\n\r",CSRR(mhartid));
// asm volatile("wfi");
    vmm_init();

    /* Should never reach here */
    while (1);
}
