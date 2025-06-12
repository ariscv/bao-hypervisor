/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <arch/bao.h>
#include <asm_defs.h>
#include <arch/csrs.h>
#include <arch/vm.h>

.text 
#define __DEBUG_EXP

.macro GET_CPU _ret_reg, _t1, _t2, _t3
    /* CPU physical based address */
    la \_ret_reg, _dmem_phys_beg 
    
    /* CPU_X physical base address */
    li \_t1, CPU_SIZE
    csrr \_t3,mhartid //这里会trap到M-mode就是屎....要么处理器实现不trap，要么多加点寄存器给hypervisor放管理用的寄存器
    mul \_t2, \_t3, \_t1
    add \_ret_reg, \_ret_reg, \_t2
.endm

.macro	TRAP_SAVE_AND_SETUP_SP_T0
	/* Swap TP and MSCRATCH */
	csrrw	tp, sscratch, tp

	/* Save t0,t1,t2,t3 in scratch space */
	REG_S	t0, HYP_SCRATCH_CPU_ARCH_TMPT0_OFFSET(tp)

    LOAD    t0, CPU_VCPU_OFF(tp) // t0 = (cpu()->vcpu)
    addi    t0, t0, VCPU_REGS_OFF

	/* Save original SP on exception stack */
	REG_S	sp, HYP_TRAP_REGS_OFFSET(sp)(t0)

	/* Set SP */
	mv	sp, t0

	/* Restore T0 from scratch space */
	REG_L	t0, HYP_SCRATCH_CPU_ARCH_TMPT0_OFFSET(tp)

	/* Save T0 on stack */
	REG_S	t0, HYP_TRAP_REGS_OFFSET(t0)(sp)

	/* Swap TP and MSCRATCH */
	csrrw	tp, sscratch, tp
.endm

.macro PUSH _reg_name
    STORE \_reg_name, -4(sp)
    addi sp, sp, -4
.endm
.macro POP _reg_name
    LOAD \_reg_name, 0(sp)
    addi sp, sp, 4
.endm

.macro PUSH_CSR_T0 _csr_name
    LOAD	t0, HYP_TRAP_REGS_OFFSET(\_csr_name)(sp)
    csrw	\_csr_name, t0
.endm

.macro SAVE_REGS_SET_SP
    TRAP_SAVE_AND_SETUP_SP_T0
    
    /* Save all general regisers except SP and T0 */
	REG_S	zero, HYP_TRAP_REGS_OFFSET(zero)(sp)
	REG_S	ra, HYP_TRAP_REGS_OFFSET(ra)(sp)
	REG_S	gp, HYP_TRAP_REGS_OFFSET(gp)(sp)
	REG_S	tp, HYP_TRAP_REGS_OFFSET(tp)(sp)
	REG_S	t1, HYP_TRAP_REGS_OFFSET(t1)(sp)
	REG_S	t2, HYP_TRAP_REGS_OFFSET(t2)(sp)
	REG_S	s0, HYP_TRAP_REGS_OFFSET(s0)(sp)
	REG_S	s1, HYP_TRAP_REGS_OFFSET(s1)(sp)
	REG_S	a0, HYP_TRAP_REGS_OFFSET(a0)(sp)
	REG_S	a1, HYP_TRAP_REGS_OFFSET(a1)(sp)
	REG_S	a2, HYP_TRAP_REGS_OFFSET(a2)(sp)
	REG_S	a3, HYP_TRAP_REGS_OFFSET(a3)(sp)
	REG_S	a4, HYP_TRAP_REGS_OFFSET(a4)(sp)
	REG_S	a5, HYP_TRAP_REGS_OFFSET(a5)(sp)
	REG_S	a6, HYP_TRAP_REGS_OFFSET(a6)(sp)
	REG_S	a7, HYP_TRAP_REGS_OFFSET(a7)(sp)
	REG_S	s2, HYP_TRAP_REGS_OFFSET(s2)(sp)
	REG_S	s3, HYP_TRAP_REGS_OFFSET(s3)(sp)
	REG_S	s4, HYP_TRAP_REGS_OFFSET(s4)(sp)
	REG_S	s5, HYP_TRAP_REGS_OFFSET(s5)(sp)
	REG_S	s6, HYP_TRAP_REGS_OFFSET(s6)(sp)
	REG_S	s7, HYP_TRAP_REGS_OFFSET(s7)(sp)
	REG_S	s8, HYP_TRAP_REGS_OFFSET(s8)(sp)
	REG_S	s9, HYP_TRAP_REGS_OFFSET(s9)(sp)
	REG_S	s10, HYP_TRAP_REGS_OFFSET(s10)(sp)
	REG_S	s11, HYP_TRAP_REGS_OFFSET(s11)(sp)
	REG_S	t3, HYP_TRAP_REGS_OFFSET(t3)(sp)
	REG_S	t4, HYP_TRAP_REGS_OFFSET(t4)(sp)
	REG_S	t5, HYP_TRAP_REGS_OFFSET(t5)(sp)
	REG_S	t6, HYP_TRAP_REGS_OFFSET(t6)(sp)

.endm

.macro __NOT_USE_OTHER_WAY_SAVE_REGS
    // in this case,any other reg can be used to replace tp
    PUSH tp // include addi sp, sp, -4
#ifndef __DEBUG_EXP
    /* tp <- &(cpu().vcpu.regs[0]) */
    csrr tp, sscratch
    LOAD   tp, CPU_VCPU_OFF(tp) // t0 = (cpu()->vcpu)
    addi tp, tp, VCPU_REGS_OFF

    /* push sp */
    addi sp, sp, 4 // modify sp, reverse "PUSH tp"
	STORE	sp, HYP_TRAP_REGS_OFFSET(sp)(tp)
    addi sp, sp, -4
#else
    POP tp
    STORE sp, (-HYP_TRAP_REGS_SIZE+HYP_TRAP_REGS_OFFSET(sp))(sp)
    addi sp, sp, -HYP_TRAP_REGS_SIZE
    STORE	tp, HYP_TRAP_REGS_OFFSET(tp)(sp)
    mv tp, sp
#endif
    /* Push regs without sp and tp, total 30 */
	STORE	zero, HYP_TRAP_REGS_OFFSET(zero)(tp)
	STORE	ra, HYP_TRAP_REGS_OFFSET(ra)(tp)
	STORE	gp, HYP_TRAP_REGS_OFFSET(gp)(tp)
	STORE	t0, HYP_TRAP_REGS_OFFSET(t0)(tp)
	STORE	t1, HYP_TRAP_REGS_OFFSET(t1)(tp)
	STORE	t2, HYP_TRAP_REGS_OFFSET(t2)(tp)
	STORE	s0, HYP_TRAP_REGS_OFFSET(s0)(tp)
	STORE	s1, HYP_TRAP_REGS_OFFSET(s1)(tp)
	STORE	a0, HYP_TRAP_REGS_OFFSET(a0)(tp)
	STORE	a1, HYP_TRAP_REGS_OFFSET(a1)(tp)
	STORE	a2, HYP_TRAP_REGS_OFFSET(a2)(tp)
	STORE	a3, HYP_TRAP_REGS_OFFSET(a3)(tp)
	STORE	a4, HYP_TRAP_REGS_OFFSET(a4)(tp)
	STORE	a5, HYP_TRAP_REGS_OFFSET(a5)(tp)
	STORE	a6, HYP_TRAP_REGS_OFFSET(a6)(tp)
	STORE	a7, HYP_TRAP_REGS_OFFSET(a7)(tp)
	STORE	s2, HYP_TRAP_REGS_OFFSET(s2)(tp)
	STORE	s3, HYP_TRAP_REGS_OFFSET(s3)(tp)
	STORE	s4, HYP_TRAP_REGS_OFFSET(s4)(tp)
	STORE	s5, HYP_TRAP_REGS_OFFSET(s5)(tp)
	STORE	s6, HYP_TRAP_REGS_OFFSET(s6)(tp)
	STORE	s7, HYP_TRAP_REGS_OFFSET(s7)(tp)
	STORE	s8, HYP_TRAP_REGS_OFFSET(s8)(tp)
	STORE	s9, HYP_TRAP_REGS_OFFSET(s9)(tp)
	STORE	s10, HYP_TRAP_REGS_OFFSET(s10)(tp)
	STORE	s11, HYP_TRAP_REGS_OFFSET(s11)(tp)
	STORE	t3, HYP_TRAP_REGS_OFFSET(t3)(tp)
	STORE	t4, HYP_TRAP_REGS_OFFSET(t4)(tp)
	STORE	t5, HYP_TRAP_REGS_OFFSET(t5)(tp)
	STORE	t6, HYP_TRAP_REGS_OFFSET(t6)(tp)

#ifndef __DEBUG_EXP
    /* pop tp */
    mv t0,tp
    POP tp
    STORE	tp, HYP_TRAP_REGS_OFFSET(tp)(t0)
#else
    mv t0,tp
#endif
    mv  sp,t0
.endm

.macro SAVE_CSRS_SP
    csrr    t0, hstatus
    STORE   t0, HYP_TRAP_REGS_OFFSET(hstatus)(sp)
    csrr    t0, sstatus
    STORE   t0, HYP_TRAP_REGS_OFFSET(sstatus)(sp)
    csrr    t0, sepc
    STORE   t0, HYP_TRAP_REGS_OFFSET(sepc)(sp)

.endm
.macro VM_EXIT
    SAVE_REGS_SET_SP
    
    SAVE_CSRS_SP


    csrr    sp, sscratch
    li      t0, (CPU_STACK_OFF + CPU_STACK_SIZE)
    add     sp, sp, t0
    # GET_CPU t0, t1, t2, t3
    # li      t1, (CPU_STACK_OFF + CPU_STACK_SIZE)
    # add     sp, t1, t0

    .option push
    .option norelax
    la  gp, __global_pointer$
    .option pop   

.endm



.macro POP_CSR_T0 _csr_name
    LOAD	t0, HYP_TRAP_REGS_OFFSET(\_csr_name)(sp)
    csrw	\_csr_name, t0
.endm

.macro GET_SP
    csrr sp,sscratch  // Read HTPIDR (CPU base address)
    LOAD   sp, CPU_VCPU_OFF(sp) // t0 = (cpu()->vcpu)
    addi sp, sp, VCPU_REGS_OFF
.endm

//.macro GET_SP
//    # /* CPU physical based address */
//    # la t0, _dmem_phys_beg 
//    
//    # /* CPU_X physical base address */
//    # li t1, CPU_SIZE
//    # csrr a0,mhartid //这里会trap到M-mode就是屎....要么处理器实现不trap，要么多加点寄存器给hypervisor放管理用的寄存器
//    # mul t2, a0, t1
//    # add t0, t0, t2
//    GET_CPU t0, t1, t2, t3
//
//    mv sp,t0 //csrr sp,sscratch  // Read HTPIDR (CPU base address)
//    LOAD   sp, CPU_VCPU_OFF(sp) // t0 = (cpu()->vcpu)
//    addi sp, sp, VCPU_REGS_OFF
//    csrw  sscratch,sp
//.endm

.macro VM_ENTRY
    GET_SP

    /* csrs */
    # POP_CSR_T0 sepc
    # POP_CSR_T0 sstatus
    # POP_CSR_T0 hstatus
    LOAD	t0, HYP_TRAP_REGS_OFFSET(sepc)(sp)
    csrw	sepc, t0
    LOAD	t0, HYP_TRAP_REGS_OFFSET(sstatus)(sp)
    csrw	sstatus, t0
    LOAD	t0, HYP_TRAP_REGS_OFFSET(hstatus)(sp)
    csrw	hstatus, t0

    LOAD	ra, HYP_TRAP_REGS_OFFSET(ra)(sp)
	LOAD	gp, HYP_TRAP_REGS_OFFSET(gp)(sp)
	LOAD	tp, HYP_TRAP_REGS_OFFSET(tp)(sp)
	LOAD	t0, HYP_TRAP_REGS_OFFSET(t0)(sp)
	LOAD	t1, HYP_TRAP_REGS_OFFSET(t1)(sp)
	LOAD	t2, HYP_TRAP_REGS_OFFSET(t2)(sp)
	LOAD	s0, HYP_TRAP_REGS_OFFSET(s0)(sp)
	LOAD	s1, HYP_TRAP_REGS_OFFSET(s1)(sp)
	LOAD	a0, HYP_TRAP_REGS_OFFSET(a0)(sp)
	LOAD	a1, HYP_TRAP_REGS_OFFSET(a1)(sp)
	LOAD	a2, HYP_TRAP_REGS_OFFSET(a2)(sp)
	LOAD	a3, HYP_TRAP_REGS_OFFSET(a3)(sp)
	LOAD	a4, HYP_TRAP_REGS_OFFSET(a4)(sp)
	LOAD	a5, HYP_TRAP_REGS_OFFSET(a5)(sp)
	LOAD	a6, HYP_TRAP_REGS_OFFSET(a6)(sp)
	LOAD	a7, HYP_TRAP_REGS_OFFSET(a7)(sp)
	LOAD	s2, HYP_TRAP_REGS_OFFSET(s2)(sp)
	LOAD	s3, HYP_TRAP_REGS_OFFSET(s3)(sp)
	LOAD	s4, HYP_TRAP_REGS_OFFSET(s4)(sp)
	LOAD	s5, HYP_TRAP_REGS_OFFSET(s5)(sp)
	LOAD	s6, HYP_TRAP_REGS_OFFSET(s6)(sp)
	LOAD	s7, HYP_TRAP_REGS_OFFSET(s7)(sp)
	LOAD	s8, HYP_TRAP_REGS_OFFSET(s8)(sp)
	LOAD	s9, HYP_TRAP_REGS_OFFSET(s9)(sp)
	LOAD	s10, HYP_TRAP_REGS_OFFSET(s10)(sp)
	LOAD	s11, HYP_TRAP_REGS_OFFSET(s11)(sp)
	LOAD	t3, HYP_TRAP_REGS_OFFSET(t3)(sp)
	LOAD	t4, HYP_TRAP_REGS_OFFSET(t4)(sp)
	LOAD	t5, HYP_TRAP_REGS_OFFSET(t5)(sp)
	LOAD	t6, HYP_TRAP_REGS_OFFSET(t6)(sp)

    LOAD    sp, HYP_TRAP_REGS_OFFSET(sp)(sp)
    
    sret
    j   .

.endm



.balign 0x4
.global _hyp_trap_vector	
_hyp_trap_vector:
    //j _hyp_trap_before_boot
    VM_EXIT
    csrr    t0, scause
    bltz    t0, 1f
    call    sync_exception_handler
    j       2f
1:
    call    interrupts_arch_handle
2:
.global vcpu_arch_entry
vcpu_arch_entry:
    VM_ENTRY


#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)


#define LOAD  lw
#define STORE sw
#define XLEN  4


#define REGS_LO16(f) \
    f(0)  f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15)
#define REGS_HI16(f) \
                                    f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)
#define NR_REGS 32


#define REGS(f) REGS_LO16(f) REGS_HI16(f)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((NR_REGS + 3 + 1) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  ((NR_REGS + 0) * XLEN)
#define OFFSET_STATUS ((NR_REGS + 1) * XLEN)
#define OFFSET_EPC    ((NR_REGS + 2) * XLEN)
.global _hyp_trap_before_boot_handler
_hyp_trap_before_boot:
    MAP(REGS, PUSH)
    mv  a0, sp
    call  _hyp_trap_before_boot_handler
    mv  sp, a0
    MAP(REGS, POP)