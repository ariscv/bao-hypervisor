/**
 * SPDX-License-Identifier: Apache-2.0 
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <vm.h>
#include <page_table.h>
#include <arch/csrs.h>
#include <arch/vplic.h>
#include <arch/instructions.h>
#include <string.h>
#include <config.h>

#define pse_printf(format, ...) p+=sprintk(p,format, ## __VA_ARGS__)
#define SBI_TRAP_LOG(format, ...)\
  pse_printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ## __VA_ARGS__)
const char *regs_names[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
	"epc","mstatus","mstatusH"
};

int print_cpu_vcpu_arch_regs(char* buf,char* prefix,struct arch_regs *arch_regs){
    char*  p=buf;

	// SBI_TRAP_LOG("trap stack info:");
	for(int i=0;i<35;i++){
		
        switch(i%4){
            case 0:
                pse_printf("%s%s=0x%08x ",prefix,regs_names[i],((uint32_t*)(&arch_regs->x[0]))[i]);
                break;
            case 3:
                pse_printf("%s=0x%08x \n",regs_names[i],((uint32_t*)(&arch_regs->x[0]))[i]);
                break;
            default:
                pse_printf("%s=0x%08x ",regs_names[i],((uint32_t*)(&arch_regs->x[0]))[i]);
                break;
        }
	}
    pse_printf("\n");
    return p-buf;
}

void vm_arch_init(struct vm *vm, const struct vm_config *config)
{
    // paddr_t root_pt_pa;
    // mem_translate(&cpu()->as, (vaddr_t)vm->as.pt.root, &root_pt_pa);

    // unsigned long hgatp = (root_pt_pa >> PAGE_SHIFT) | (HGATP_MODE_DFLT) |
    //                       ((vm->id << HGATP_VMID_OFF) & HGATP_VMID_MSK);

    // CSRW(CSR_HGATP, hgatp);

    // vplic_init(vm, platform.arch.plic_base);
}

void vcpu_arch_init(struct vcpu *vcpu, struct vm *vm) {
    vcpu->arch.sbi_ctx.lock = SPINLOCK_INITVAL;
    vcpu->arch.sbi_ctx.state = vcpu->id == 0 ?  STARTED : STOPPED;
}

void vcpu_arch_reset(struct vcpu *vcpu, vaddr_t entry)
{
    memset(&vcpu->regs, 0, sizeof(struct arch_regs));
    
    // CSRW(sscratch, &vcpu->regs);

    vcpu->regs.hstatus = HSTATUS_SPV /* | HSTATUS_VSXL_64 */;
    vcpu->regs.sstatus = SSTATUS_SPP_BIT | SSTATUS_FS_DIRTY | SSTATUS_XS_DIRTY;
    vcpu->regs.sepc = entry;
    vcpu->regs.a0 = vcpu->arch.hart_id = vcpu->id;
    vcpu->regs.a1 = 0;  // according to sbi it should be the dtb load address

    CSRW(CSR_HCOUNTEREN, HCOUNTEREN_TM);
    CSRW(CSR_HTIMEDELTA, 0);
    CSRW(CSR_VSSTATUS, /* SSTATUS_SD | */ SSTATUS_FS_DIRTY | SSTATUS_XS_DIRTY);
    CSRW(CSR_HIE, 0);
    CSRW(CSR_VSTVEC, 0);
    CSRW(CSR_VSSCRATCH, 0);
    CSRW(CSR_VSEPC, 0);
    CSRW(CSR_VSCAUSE, 0);
    CSRW(CSR_VSTVAL, 0);
    CSRW(CSR_HVIP, 0);
    CSRW(CSR_VSATP, 0);
}

unsigned long vcpu_readreg(struct vcpu *vcpu, unsigned long reg)
{
    if ((reg <= 0) || (reg > 31)) return 0;
    return vcpu->regs.x[reg];
}

void vcpu_writereg(struct vcpu *vcpu, unsigned long reg, unsigned long val)
{
    if ((reg <= 0) || (reg > 31)) return;
    vcpu->regs.x[reg] = val;
}

unsigned long vcpu_readpc(struct vcpu *vcpu)
{
    return vcpu->regs.sepc;
}

void vcpu_writepc(struct vcpu *vcpu, unsigned long pc)
{
    vcpu->regs.sepc = pc;
}

void vcpu_arch_run(struct vcpu *vcpu){

    if(vcpu->arch.sbi_ctx.state == STARTED){
        printk("cpu %d: vm cpu started\n",cpu()->id);
        vcpu_arch_entry();
    } else {
        cpu_idle();
    }    

}
