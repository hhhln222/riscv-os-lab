// vmm.h - 虚拟内存管理接口
#ifndef _VMM_H_
#define _VMM_H_

#include "types.h"
#include "riscv.h"
#include "uart.h"

typedef uint64* pagetable_t;

// 页表管理接口
pagetable_t create_pagetable(void);
int map_page(pagetable_t pt, uint64 va, uint64 pa, int perm);
int unmap_page(pagetable_t pt, uint64 va);
pte_t* walk_create(pagetable_t pt, uint64 va);
pte_t* walk_lookup(pagetable_t pt, uint64 va);
void destroy_pagetable(pagetable_t pt);
void dump_pagetable(pagetable_t pt, int level, uint64 va);

// 内核页表初始化
void kvminit(void);
void kvminithart(void);

#endif