/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef __ARCH_VM_H__
#define __ARCH_VM_H__

#include <bao.h>

#define REG_RA (1)
#define REG_SP (2)
#define REG_GP (3)
#define REG_TP (4)
#define REG_T0 (5)
#define REG_T1 (6)
#define REG_T2 (7)
#define REG_S0 (8)
#define REG_S1 (9)
#define REG_A0 (10)
#define REG_A1 (11)
#define REG_A2 (12)
#define REG_A3 (13)
#define REG_A4 (14)
#define REG_A5 (15)
#define REG_A6 (16)
#define REG_A7 (17)
#define REG_S2 (18)
#define REG_S3 (19)
#define REG_S4 (20)
#define REG_S5 (21)
#define REG_S6 (22)
#define REG_S7 (23)
#define REG_S8 (24)
#define REG_S9 (25)
#define REG_S10 (26)
#define REG_S11 (27)
#define REG_T3 (28)
#define REG_T4 (29)
#define REG_T5 (30)
#define REG_T6 (31)

/* below is learning from opensbi */
/* clang-format off */

/** Index of zero member in sbi_trap_regs */
#define HYP_TRAP_REGS_zero			0
/** Index of ra member in sbi_trap_regs */
#define HYP_TRAP_REGS_ra			1
/** Index of sp member in sbi_trap_regs */
#define HYP_TRAP_REGS_sp			2
/** Index of gp member in sbi_trap_regs */
#define HYP_TRAP_REGS_gp			3
/** Index of tp member in sbi_trap_regs */
#define HYP_TRAP_REGS_tp			4
/** Index of t0 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t0			5
/** Index of t1 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t1			6
/** Index of t2 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t2			7
/** Index of s0 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s0			8
/** Index of s1 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s1			9
/** Index of a0 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a0			10
/** Index of a1 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a1			11
/** Index of a2 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a2			12
/** Index of a3 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a3			13
/** Index of a4 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a4			14
/** Index of a5 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a5			15
/** Index of a6 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a6			16
/** Index of a7 member in sbi_trap_regs */
#define HYP_TRAP_REGS_a7			17
/** Index of s2 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s2			18
/** Index of s3 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s3			19
/** Index of s4 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s4			20
/** Index of s5 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s5			21
/** Index of s6 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s6			22
/** Index of s7 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s7			23
/** Index of s8 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s8			24
/** Index of s9 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s9			25
/** Index of s10 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s10			26
/** Index of s11 member in sbi_trap_regs */
#define HYP_TRAP_REGS_s11			27
/** Index of t3 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t3			28
/** Index of t4 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t4			29
/** Index of t5 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t5			30
/** Index of t6 member in sbi_trap_regs */
#define HYP_TRAP_REGS_t6			31
#define HYP_TRAP_REGS_hstatus		32
#define HYP_TRAP_REGS_sstatus		33
#define HYP_TRAP_REGS_sepc			34
/** Last member index in sbi_trap_regs */
#define HYP_TRAP_REGS_last			35

/* clang-format on */

/** Get offset of member with name 'x' in sbi_trap_regs */
#define HYP_TRAP_REGS_OFFSET(x) ( REGLEN * (HYP_TRAP_REGS_##x) )
/** Size (in bytes) of sbi_trap_regs */
#define HYP_TRAP_REGS_SIZE HYP_TRAP_REGS_OFFSET(last)

#define HYP_SCRATCH_CPU_ARCH_TMPT0_OFFSET ( CPU_ARCH_OFF + CPU_ARCH_TMPT0_OFF )
#ifndef __ASSEMBLER__

#include <arch/vplic.h>
#include <arch/sbi.h>

struct arch_vm_platform {
    paddr_t plic_base;
};

struct vm_arch {
    struct vplic vplic;
};

struct vcpu_arch {
    vcpuid_t hart_id;
    struct sbi_hsm sbi_ctx;
};

struct arch_regs {
    union {
        unsigned long x[32];
        struct {
            unsigned long zero;
            unsigned long ra;
            unsigned long sp;
            unsigned long gp;
            unsigned long tp;
            unsigned long t0;
            unsigned long t1;
            unsigned long t2;
            unsigned long s0;
            unsigned long s1;
            unsigned long a0;
            unsigned long a1;
            unsigned long a2;
            unsigned long a3;
            unsigned long a4;
            unsigned long a5;
            unsigned long a6;
            unsigned long a7;
            unsigned long s2;
            unsigned long s3;
            unsigned long s4;
            unsigned long s5;
            unsigned long s6;
            unsigned long s7;
            unsigned long s8;
            unsigned long s9;
            unsigned long s10;
            unsigned long s11;
            unsigned long t3;
            unsigned long t4;
            unsigned long t5;
            unsigned long t6;
        };
    };

    unsigned long hstatus;
    unsigned long sstatus;
    unsigned long sepc;

    // unsigned long vsstatus;
    // unsigned long vsie;
    // unsigned long vstvec;
    // unsigned long vsscratch;
    // unsigned long vsepc;
    // unsigned long vscause;
    // unsigned long vstval;
    // unsigned long vsip;
    // unsigned long vsatp;

} __attribute__((__packed__, aligned(sizeof(unsigned long))));

void vcpu_arch_entry();

static inline void vcpu_arch_inject_hw_irq(struct vcpu *vcpu, uint64_t id)
{
    vplic_inject(vcpu, id);
}

static inline void vcpu_arch_inject_irq(struct vcpu *vcpu, uint64_t id)
{
    vplic_inject(vcpu, id);
}

#endif /* __ASSEMBLER__ */

#endif /* __ARCH_VM_H__ */
