#include "types.h"

// 声明外部函数
void uart_puts(char *s);
void console_init();
void console_clear();
void printf(char *fmt, ...);
void run_all_tests();

// 声明栈空间
// 这里的 stack0 对应 entry.S 中的符号
// 放在 .bss 段，大小为 4096 字节
__attribute__ ((aligned (16))) char stack0[4096];

void start() {
    // 测试清屏
    console_clear();

    // 测试基础输出
    printf("Hello, RISC-V OS!\n");

    // 运行测试
    run_all_tests();

    while(1) {}
}