// riscv.h - RISC-V架构相关定义
#ifndef _RISCV_H_
#define _RISCV_H_

#include "types.h"
#include "uart.h"

// 页大小定义
#define PGSIZE 4096
#define PGSHIFT 12

// 页表项权限位
#define PTE_V (1L << 0) // 有效位
#define PTE_R (1L << 1) // 读权限
#define PTE_W (1L << 2) // 写权限
#define PTE_X (1L << 3) // 执行权限
#define PTE_U (1L << 4) // 用户态访问权限

// 页表项类型
typedef uint64 pte_t;

// 地址操作宏
#define PGROUNDUP(sz) (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

// 从页表项提取物理地址
#define PTE_PA(pte) (((pte) >> 10) << 12)

// 从虚拟地址提取页表索引
#define VPN_SHIFT(level) (12 + 9 * (level))
#define VPN_MASK(va, level) (((va) >> VPN_SHIFT(level)) & 0x1FF)

// 获取VPN[0], VPN[1], VPN[2]
#define VPN0(va) (((va) >> 12) & 0x1FF)
#define VPN1(va) (((va) >> 21) & 0x1FF)
#define VPN2(va) (((va) >> 30) & 0x1FF)

// SATP寄存器相关
#define SATP_SV39 (8L << 60) // Sv39模式
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)(pagetable)) >> 12))

// 内存布局定义
#define KERNBASE 0x80000000 // 内核基地址
#define PHYSTOP  0x88000000 // 物理内存上限

// 读写CSR寄存器的内联函数
static inline uint64 r_satp() {
    uint64 x;
    asm volatile("csrr %0, satp" : "=r" (x) );
    return x;
}

static inline void w_satp(uint64 x) {
    asm volatile("csrw satp, %0" : : "r" (x) );
}

static inline void sfence_vma() {
    asm volatile("sfence.vma zero, zero");
}

#endif