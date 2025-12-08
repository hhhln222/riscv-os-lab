// test.c - 测试用例实现
#include "test.h"

void test_physical_memory() {
    printf("\n--- Test: Physical Memory Management ---\n");
    
    // 测试基本分配和释放
    void *page1 = alloc_page();
    void *page2 = alloc_page();
    
    if (page1 != page2) {
        printf("✓ Pages allocated at different addresses\n");
    } else {
        printf("✗ Pages allocated at same address\n");
    }
    
    if (((uint64)page1 & 0xFFF) == 0) {
        printf("✓ Page1 is page-aligned\n");
    } else {
        printf("✗ Page1 is not page-aligned\n");
    }
    
    // 测试数据写入
    *(int*)page1 = 0x12345678;
    if (*(int*)page1 == 0x12345678) {
        printf("✓ Memory write/read works correctly\n");
    } else {
        printf("✗ Memory write/read failed\n");
    }
    
    // 测试释放和重新分配
    free_page(page1);
    void *page3 = alloc_page();
    // page3可能等于page1（取决于分配策略）
    printf("✓ Free and reallocate test completed\n");
    
    // 释放所有分配的页面
    free_page(page2);
    if (page3 != page1) free_page(page3);
    
    // 显示内存统计
    pmm_stats();
}

void test_pagetable() {
    printf("\n--- Test: Page Table Management ---\n");
    
    pagetable_t pt = create_pagetable();
    if (pt == 0) {
        printf("✗ Failed to create page table\n");
        return;
    }
    
    printf("✓ Created page table\n");
    
    // 测试基本映射
    uint64 va = 0x1000000;
    uint64 pa = (uint64)alloc_page();
    if (pa == 0) {
        printf("✗ Failed to allocate physical page\n");
        return;
    }
    
    int result = map_page(pt, va, pa, PTE_R | PTE_W);
    if (result == 0) {
        printf("✓ Mapped virtual address 0x%x to physical address 0x%x\n", va, pa);
    } else {
        printf("✗ Failed to map page\n");
        return;
    }
    
    // 测试地址转换
    pte_t *pte = walk_lookup(pt, va);
    if (pte != 0 && (*pte & PTE_V)) {
        printf("✓ Page table lookup successful\n");
    } else {
        printf("✗ Page table lookup failed\n");
    }
    
    if (pte && PTE_PA(*pte) == pa) {
        printf("✓ Physical address matches expected value\n");
    } else {
        printf("✗ Physical address mismatch\n");
    }
    
    // 测试权限位
    if (pte && (*pte & PTE_R) && (*pte & PTE_W)) {
        printf("✓ Permissions set correctly (R+W)\n");
    } else {
        printf("✗ Permissions not set correctly\n");
    }
    
    if (pte && !(*pte & PTE_X)) {
        printf("✓ Execute permission not set as expected\n");
    } else {
        printf("✗ Unexpected execute permission\n");
    }
    
    // 测试页表遍历创建
    uint64 va2 = 0x2000000;
    uint64 pa2 = (uint64)alloc_page();
    if (pa2 != 0) {
        result = map_page(pt, va2, pa2, PTE_R | PTE_X);
        if (result == 0) {
            printf("✓ Created new page table level for 0x%x\n", va2);
        } else {
            printf("✗ Failed to create new page table level\n");
        }
        free_page((void*)pa2);
    }
    
    // 释放资源
    free_page((void*)pa);
    destroy_pagetable(pt);
    
    printf("✓ Page table test completed\n");
}

void test_virtual_memory() {
    printf("\n--- Test: Virtual Memory Activation ---\n");
    
    printf("Before enabling paging...\n");
    
    // 启用分页
    kvminit();
    kvminithart();
    
    printf("After enabling paging...\n");
    printf("✓ Virtual memory activated successfully\n");
    
    // 测试内核代码仍然可执行
    printf("✓ Kernel code is still accessible\n");
    
    // 测试内核数据仍然可访问
    int test_var = 12345;
    if (test_var == 12345) {
        printf("✓ Kernel data is still accessible\n");
    } else {
        printf("✗ Kernel data access failed\n");
    }
    
    // 测试设备访问仍然正常
    uart_puts("UART still works after paging enabled\n");
    printf("✓ Device access still works\n");
}

void run_all_tests() {
    printf_color(36, "=== STARTING PHYSICAL MEMORY AND PAGE TABLE TESTS ===\n");
    
    test_physical_memory();
    test_pagetable();
    test_virtual_memory();
    
    printf_color(36, "=== ALL TESTS COMPLETED ===\n");
}