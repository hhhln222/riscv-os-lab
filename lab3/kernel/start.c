// start.c - 系统启动入口
#include "types.h"
#include "console.h"
#include "printf.h"
#include "test.h"

__attribute__ ((aligned (16))) char stack0[4096];

void start() {
    console_clear();
    printf("Hello, RISC-V OS with Physical Memory Management!\n");
    
    // 初始化物理内存管理器
    pmm_init();
    
    // 运行测试
    run_all_tests();
    
    while(1) {}
}
