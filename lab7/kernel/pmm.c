// pmm.c - 物理内存管理器实现
#include "pmm.h"
#include "console.h"
#include "printf.h"
#include "riscv.h"

struct run {
    struct run *next;
};

static struct run *freelist;
static uint64 num_free_pages = 0;
static uint64 total_pages = 0;

void pmm_init() {
    // 假设物理内存从0x80400000开始到PHYSTOP结束
    uint64 start = 0x80400000; // 避开内核代码和栈
    uint64 end = PHYSTOP;
    
    // 确保起始地址页对齐
    start = PGROUNDUP(start);
    
    freelist = 0;
    num_free_pages = 0;
    
    // 构建空闲页链表
    for (uint64 a = start; a + PGSIZE <= end; a += PGSIZE) {
        struct run *r = (struct run*)a;
        r->next = freelist;
        freelist = r;
        num_free_pages++;
    }
    
    total_pages = num_free_pages;
    printf("Physical memory manager initialized: %d pages available\n", num_free_pages);
}

void* alloc_page() {
    if (freelist == 0) {
        printf("Out of physical memory!\n");
        return 0;
    }
    
    struct run *r = freelist;
    freelist = freelist->next;
    num_free_pages--;
    
    // 清零页面内容
    char *p = (char*)r;
    for (int i = 0; i < PGSIZE; i++) {
        p[i] = 0;
    }
    
    return (void*)r;
}

void free_page(void* page) {
    if (page == 0) return;
    
    // 简单验证地址是否合理
    if ((uint64)page < 0x80200000 || (uint64)page >= PHYSTOP || ((uint64)page & 0xFFF) != 0) {
        printf("Invalid page address for free: 0x%x\n", page);
        return;
    }
    
    struct run *r = (struct run*)page;
    r->next = freelist;
    freelist = r;
    num_free_pages++;
}

void* alloc_pages(int n) {
    if (n <= 0) return 0;
    if (n == 1) return alloc_page();
    
    // 简单实现连续多页分配
    // 这里我们尝试找到连续的n个页面，但不保证物理连续
    // 实际的物理连续分配需要更复杂的算法
    
    void** pages = (void**)alloc_page();  // 临时分配一个页面来存储指针数组
    if (!pages) return 0;
    
    // 分配n个页面
    for (int i = 0; i < n; i++) {
        pages[i] = alloc_page();
        if (!pages[i]) {
            // 如果分配失败，释放已分配的页面
            for (int j = 0; j < i; j++) {
                free_page(pages[j]);
            }
            free_page(pages);
            return 0;
        }
    }
    
    // 在第一个页面的前几个字节存储页面数量和指针数组地址
    // 这样free_pages可以知道如何释放
    *(int*)pages[0] = n;  // 存储页面数量
    *(void***)((char*)pages[0] + sizeof(int)) = pages;  // 存储指针数组地址
    
    void* result = pages[0];  // 返回第一个页面地址
    
    // 释放临时指针数组页面，因为指针数组已经存储在第一个页面中
    free_page(pages);
    
    return result;
}

void pmm_stats() {
    printf("Physical Memory Manager Stats:\n");
    printf("  Total pages: %d\n", total_pages);
    printf("  Free pages: %d\n", num_free_pages);
    printf("  Used pages: %d\n", total_pages - num_free_pages);
}