// start.c - 系统启动入口
#include "types.h"
#include "console.h"
#include "printf.h"
#include "test.h"
#include "trap.h"  
#include "riscv.h" 
#include "proc.h" // [新增]

extern void kernelvec();

__attribute__ ((aligned (16))) char stack0[4096];

void start() {
    console_clear();
    printf("Hello, RISC-V OS (Lab 5 - Process)!\n");
    
    // 1. 基础初始化
    pmm_init();
    kvminit();
    kvminithart();
    trap_init();
    w_stvec((uint64)kernelvec);
    timer_init();
    
    // 2. 进程初始化 [Lab 5]
    proc_init();
    
    // 3. 开启中断
    // 注意：调度器里也会开中断，但提前开也没事
    intr_on();
    
    // 4. 创建测试进程
    // 我们把 run_all_tests 里的逻辑放到这里，或者让 run_all_tests 只负责创建
    run_all_tests();
    
    // 5. 启动调度器 (永不返回)
    scheduler();
}