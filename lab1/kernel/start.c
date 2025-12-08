#include "types.h"

// 声明外部函数
void uart_puts(char *s);

// 声明栈空间
// 这里的 stack0 对应 entry.S 中的符号
// 放在 .bss 段，大小为 4096 字节
__attribute__ ((aligned (16))) char stack0[4096];

void start() {
    // 简单的裸机启动测试
    uart_puts("Hello, RISC-V OS!\n");
    uart_puts("Experiment 1 Completed.\n");

    // 死循环，防止退出
    while(1) {}
}