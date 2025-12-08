// vmm.c - 虚拟内存管理实现
#include "vmm.h"
#include "console.h"
#include "printf.h"
#include "pmm.h"

pagetable_t create_pagetable() {
    pagetable_t pt = (pagetable_t)alloc_page();
    if (pt) {
        for (int i = 0; i < 512; i++) {
            pt[i] = 0; // 清零页表
        }
    }
    return pt;
}

pte_t* walk_create(pagetable_t pt, uint64 va) {
    if(va >= 0x100000000000L || va < 0) {
        printf("walk: invalid va %p\n", va);
        return 0;
    }

    for(int level = 2; level > 0; level--) {
        pte_t *pte = &pt[VPN_MASK(va, level)];
        if(*pte & PTE_V) {
            pt = (pagetable_t)PTE_PA(*pte);
        } else {
            uint64 pa = (uint64)alloc_page();
            if(pa == 0)
                return 0;
            *pte = (pa >> 12 << 10) | PTE_V;
            pt = (pagetable_t)pa;
        }
    }
    return &pt[VPN_MASK(va, 0)];
}

pte_t* walk_lookup(pagetable_t pt, uint64 va) {
    for(int level = 2; level > 0; level--) {
        pte_t *pte = &pt[VPN_MASK(va, level)];
        if((*pte & PTE_V) == 0)
            return 0;
        pt = (pagetable_t)PTE_PA(*pte);
    }
    pte_t *pte = &pt[VPN_MASK(va, 0)];
    if((*pte & PTE_V) == 0)
        return 0;
    return pte;
}

int map_page(pagetable_t pt, uint64 va, uint64 pa, int perm) {
    if (va == 0) {
        printf("map_page: NULL virtual address is invalid\n");
        return -1;
    }
    
    if((va % PGSIZE) != 0 || (pa % PGSIZE) != 0) {
        printf("map_page: invalid alignment\n");
        return -1;
    }
    
    pte_t *pte = walk_create(pt, va);
    if(pte == 0) {
        printf("map_page: walk_create failed\n");
        return -1;
    }
    
    if(*pte & PTE_V) {
        printf("map_page: already mapped\n");
        return -1;
    }
    
    *pte = (pa >> 12 << 10) | perm | PTE_V;
    return 0;
}

int unmap_page(pagetable_t pt, uint64 va) {
    pte_t *pte = walk_lookup(pt, va);
    if(pte == 0) {
        printf("unmap_page: not mapped\n");
        return -1;
    }
    
    *pte = 0; // 清除PTE
    return 0;
}

void dump_pagetable(pagetable_t pt, int level, uint64 va) {
    if (level < 0 || level > 2) return;
    
    printf("Page table level %d at 0x%x:\n", level, pt);
    
    for (int i = 0; i < 512; i++) {
        pte_t pte = pt[i];
        if (pte & PTE_V) {
            printf("  [%d]: 0x%x -> 0x%x ", i, 
                   (va & ~((1UL << (12 + 9 * (level + 1))) - 1)) | (i << (12 + 9 * level)),
                   PTE_PA(pte));
            
            if (pte & PTE_R) printf("R");
            if (pte & PTE_W) printf("W");
            if (pte & PTE_X) printf("X");
            if (pte & PTE_U) printf("U");
            printf("\n");
            
            // 如果不是最后一级，递归打印
            if (level > 0) {
                uint64 next_va = va | (i << (12 + 9 * level));
                dump_pagetable((pagetable_t)PTE_PA(pte), level - 1, next_va);
            }
        }
    }
}

void destroy_pagetable(pagetable_t pt) {
    // 递归释放页表
    for (int i = 0; i < 512; i++) {
        if (pt[i] & PTE_V) {
            if ((pt[i] & (PTE_R | PTE_W | PTE_X)) == 0) { // 中间级页表
                destroy_pagetable((pagetable_t)PTE_PA(pt[i]));
            }
        }
    }
    free_page(pt);
}

// 声明链接脚本中的符号，用于定位代码段结束位置
extern char etext[]; 
pagetable_t kernel_pagetable;

void kvminit() {
    kernel_pagetable = create_pagetable();
    
    if (kernel_pagetable == 0) {
        printf("Failed to create kernel pagetable\n");
        return;
    }
    
    // 1. 映射 UART 设备 (R/W)
    map_page(kernel_pagetable, UART0, UART0, PTE_R | PTE_W);
    
    // 2. 映射内核代码段 (Text Segment) -> R/X
    // 从 KERNBASE 到 etext (代码段结束)
    uint64 text_end = (uint64)etext;
    printf("Mapping kernel text: %lx -> %lx\n", KERNBASE, text_end);
    
    for (uint64 va = KERNBASE; va < text_end; va += PGSIZE) {
        map_page(kernel_pagetable, va, va, PTE_R | PTE_X);
    }
    
    // 3. 映射其余物理内存 (Data + Free RAM) -> R/W
    // [关键修复]：从 text_end 一直映射到 PHYSTOP
    // 这样 alloc_page 返回的高地址物理页才能被访问
    uint64 data_start = PGROUNDUP(text_end);
    printf("Mapping kernel data: %lx -> %lx\n", data_start, PHYSTOP);
    
    for (uint64 va = data_start; va < PHYSTOP; va += PGSIZE) {
        map_page(kernel_pagetable, va, va, PTE_R | PTE_W);
    }
    
    printf("Kernel page table initialized\n");
}

// 激活虚拟内存
void kvminithart() {
    w_satp(MAKE_SATP(kernel_pagetable));
    sfence_vma();
    printf("Virtual memory enabled\n");
}