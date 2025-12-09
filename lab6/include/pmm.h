// pmm.h - 物理内存管理接口
#ifndef _PMM_H_
#define _PMM_H_

#include "types.h"
#include "uart.h"

// 物理内存管理器接口
void pmm_init(void);
void* alloc_page(void);
void free_page(void* page);
void* alloc_pages(int n);
void pmm_stats(void);

#endif