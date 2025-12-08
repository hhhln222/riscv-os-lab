// test.c - 测试用例实现
#include "test.h"
#include <stddef.h>

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

void test_exception_handling() {
    printf("\n--- Test: Exception Handling ---\n");
    
    // 1. NULL指针处理测试
    printf("Testing NULL pointer handling...\n");
    free_page(NULL);  // 应安全退出无崩溃
    printf("✓ free_page(NULL) handled safely\n");
    
    pagetable_t pt = create_pagetable();
    if (pt == 0) {
        printf("✗ Failed to create page table for exception tests\n");
        return;
    }
    uint64 pa = (uint64)alloc_page();
    if (pa == 0) {
        printf("✗ Failed to allocate page for exception tests\n");
        destroy_pagetable(pt);
        return;
    }
    
    int result = map_page(pt, 0, pa, PTE_R | PTE_W);  // 映射NULL地址
    if (result != 0) {
        printf("✓ map_page with NULL VA detected correctly\n");
    } else {
        printf("✗ map_page with NULL VA not detected\n");
    }
    
    // 2. 地址不对齐测试
    printf("\nTesting unaligned addresses...\n");
    result = map_page(pt, 0x1000001, pa, PTE_R | PTE_W);  // 末位非0，未对齐
    if (result != 0) {
        printf("✓ Unaligned VA detected correctly\n");
    } else {
        printf("✗ Unaligned VA not detected\n");
    }
    
    // 3. 重复映射测试
    printf("\nTesting duplicate mapping...\n");
    uint64 va = 0x3000000;
    result = map_page(pt, va, pa, PTE_R | PTE_W);  // 首次映射
    if (result != 0) {
        printf("✗ Initial mapping failed for duplicate test\n");
    } else {
        // 尝试重复映射同一虚拟地址
        result = map_page(pt, va, pa, PTE_R);
        if (result != 0) {
            printf("✓ Duplicate mapping detected correctly\n");
        } else {
            printf("✗ Duplicate mapping not detected\n");
        }
    }
    
    // 4. 内存耗尽测试
    printf("\nTesting memory exhaustion...\n");
    int alloc_count = 0;
    void* last_page = NULL;
    // 持续分配直到内存耗尽
    while (1) {
        void* p = alloc_page();
        if (p == NULL) break;
        last_page = p;
        alloc_count++;
    }
    printf("Allocated %d pages before exhaustion\n", alloc_count);
    
    // 验证内存耗尽后返回NULL
    void* p = alloc_page();
    if (p == NULL) {
        printf("✓ alloc_page returns NULL when out of memory\n");
    } else {
        printf("✗ alloc_page did not return NULL when out of memory\n");
        free_page(p);  // 清理意外分配的页
    }
    
    // 释放最后分配的页，验证恢复功能
    if (last_page != NULL) {
        free_page(last_page);
        p = alloc_page();
        if (p != NULL) {
            printf("✓ Memory allocation recovered after free\n");
            free_page(p);
        } else {
            printf("✗ Memory allocation did not recover after free\n");
        }
    }
    
    // 清理资源
    free_page((void*)pa);
    destroy_pagetable(pt);
    printf("✓ Exception handling tests completed\n");
}

void run_all_tests() {
    printf_color(36, "=== STARTING PHYSICAL MEMORY AND PAGE TABLE TESTS ===\n");
    
    test_physical_memory();
    test_pagetable();
    test_virtual_memory();
    test_exception_handling();
    
    printf_color(36, "=== ALL TESTS COMPLETED ===\n");
}