// start.c - 系统启动入口
#include "types.h"
#include "console.h"
#include "printf.h"
#include "test.h"
#include "trap.h"  
#include "riscv.h" 

__attribute__ ((aligned (16))) char stack0[4096];

void start() {
    console_clear();
    printf("Hello, RISC-V OS (Lab 4)!\n");
    
    // 1. 内存初始化 (Lab 3)
    pmm_init();
    kvminit();
    kvminithart();
    
    // 2. 设置中断向量 (Lab 4)
    w_stvec((uint64)kernelvec);
    trap_init();
    
    // 3. 初始化定时器 (Lab 4)
    timer_init();
    
    // 4. 开启全局中断 (Lab 4)
    printf("Enabling interrupts...\n");
    intr_on();
    
    // 5. 运行测试
    run_all_tests();
    
    printf("All tests completed. Entering main loop...\n");
    
    while(1) {
        // 主循环可以做其他事情，但主要等待中断
        asm volatile("wfi"); // Wait For Interrupt
    }
}