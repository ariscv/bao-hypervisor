/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <cpu.h>
#include <interrupts.h>
#include <platform.h>
#include <objpool.h>
#include <vm.h>
#include <fences.h>
#include <console.h>

struct cpu_msg_node {
    node_t node;
    struct cpu_msg msg;
};

#define CPU_MSG_POOL_SIZE_DEFAULT (128)
#ifndef CPU_MSG_POOL_SIZE
#define CPU_MSG_POOL_SIZE CPU_MSG_POOL_SIZE_DEFAULT
#endif

OBJPOOL_ALLOC(msg_pool, struct cpu_msg_node, CPU_MSG_POOL_SIZE);

struct cpu_synctoken cpu_glb_sync = {.ready = false};

extern cpu_msg_handler_t ipi_cpumsg_handlers[];
extern uint8_t _ipi_cpumsg_handlers_size;
extern size_t _ipi_cpumsg_handlers_id_start[];
size_t ipi_cpumsg_handler_num;

struct cpuif cpu_interfaces[PLAT_CPU_NUM];

/* void cpu_print(struct cpu* s){
    char buf[256]={0};
    char *p=buf;
    const char *str=NULL;
    
    str="cpuid_t id[0x%08x]";
    p+=printk( "%s[0x%08x]\n", str, s->id);

    str="bool handling_msgs";
    p+=printk( "%s[0x%x]\n", str, s->handling_msgs);
    
    str="struct addr_space as";
    p+=printk( "%s[0x%x]\n", str, s->as);

    str="struct vcpu* vcpu";
    p+=printk( "%s[0x%x]\n", str, s->vcpu);

    str="struct cpu_arch arch";
    p+=printk( "%s[0x%x]\n", str, s->arch);

    str="struct cpuif* interface";
    p+=printk( "%s[0x%x]\n", str, s->interface);

    str="uint8_t stack[STACK_SIZE]"; 
    p+=printk( "%s[0x%x]\n", str, s->stack);
    
    // console_write(buf);
} */
static spinlock_t cpu_print_lock = SPINLOCK_INITVAL;
void cpu_print(struct cpu* s){
    char buf[256]={0};
    char *p=buf;
    const char *str=NULL;
    
    spin_lock(&cpu_print_lock);
   
    p+=sprintk(p, "=CPU %d[0x%x]:\n", s->id,s);

    str="==" "cpuid_t id";
    p+=sprintk(p, "%s=0x%x\n", str, s->id);

    str="==" "bool handling_msgs";
    p+=sprintk(p, "%s=%d\n", str, s->handling_msgs);
    
    str="==" "struct addr_space as";
    p+=sprintk(p, "%s[0x%x]=0x%x,0x%x\n", str, &s->as,s->as,s->as.id);

    p+=print_cpu_addr_space(p,&s->as);

    str="==" "struct vcpu* vcpu";
    p+=sprintk(p, "%s[0x%x]\n", str, s->vcpu);

    p+=print_cpu_vcpu(p,"---",s->vcpu);

    str="==" "struct cpu_arch arch";
    p+=sprintk(p, "%s[0x%x]=0x%x,%d\n", str, &s->arch,s->arch,s->arch.hart_id);

    str="==" "struct cpuif* interface";
    p+=sprintk(p, "%s[0x%x]\n", str, s->interface);

    str="==" "uint8_t stack[STACK_SIZE]"; 
    p+=sprintk(p, "%s[0x%x]\n", str, s->stack);
    
    console_write(buf);

    spin_unlock(&cpu_print_lock);
    return ;
}

void cpu_init(cpuid_t cpu_id, paddr_t load_addr)
{
    cpu()->id = cpu_id;
    cpu()->handling_msgs = false;
    cpu()->interface = cpu_if(cpu()->id);

    cpu_arch_init(cpu_id, load_addr);

    list_init(&cpu()->interface->event_list);

    if (cpu()->id == CPU_MASTER) {
        cpu_sync_init(&cpu_glb_sync, platform.cpu_num);

        ipi_cpumsg_handler_num =
            ((size_t)&_ipi_cpumsg_handlers_size) / sizeof(cpu_msg_handler_t);
        for (size_t i = 0; i < ipi_cpumsg_handler_num; i++) {
            ((size_t*)_ipi_cpumsg_handlers_id_start)[i] = i;
        }
    }

    cpu_sync_barrier(&cpu_glb_sync);
}

void cpu_send_msg(cpuid_t trgtcpu, struct cpu_msg *msg)
{
    struct cpu_msg_node *node = objpool_alloc(&msg_pool);
    if (node == NULL) ERROR("cant allocate msg node");
    node->msg = *msg;
    list_push(&cpu_if(trgtcpu)->event_list, (node_t *)node);
    fence_sync_write();
    interrupts_cpu_sendipi(trgtcpu, IPI_CPU_MSG);
}

bool cpu_get_msg(struct cpu_msg *msg)
{
    struct cpu_msg_node *node = NULL;
    if ((node = (struct cpu_msg_node *)list_pop(&cpu()->interface->event_list)) !=
        NULL) {
        *msg = node->msg;
        objpool_free(&msg_pool, node);
        return true;
    }
    return false;
}

void cpu_msg_handler()
{
    cpu()->handling_msgs = true;
    struct cpu_msg msg;
    while (cpu_get_msg(&msg)) {
        if (msg.handler < ipi_cpumsg_handler_num &&
            ipi_cpumsg_handlers[msg.handler]) {
            ipi_cpumsg_handlers[msg.handler](msg.event, msg.data);
        }
    }
    cpu()->handling_msgs = false;
}

void cpu_idle()
{
    cpu_arch_idle();

    /**
     * Should not return here.
     * cpu should "wake up" from idle in cpu_idle_wakeup
     * with a rewinded stack.
     */
    ERROR("Spurious idle wake up");
}

void cpu_idle_wakeup()
{
    if (interrupts_check(IPI_CPU_MSG)) {
        interrupts_clear(IPI_CPU_MSG);
        cpu_msg_handler();
    }

    if (cpu()->vcpu != NULL) {
        vcpu_run(cpu()->vcpu);
    } else {
        cpu_idle();
    }
}
