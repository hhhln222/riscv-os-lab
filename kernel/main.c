#include "uart.h"
// 添加kmain函数原型声明
void kmain(void);

// 使用全局字符数组代替字符串常量，避免某些编译器的浮点处理
static char msg1[] = "=== Minimal Kernel Booted ===\r\n";
static char msg2[] = "Copyright (c) 2023 Minimal OS\r\n\r\n";
static char msg3[] = "System Information:\r\n";
static char msg4[] = "  Architecture: RISC-V\r\n";
static char msg5[] = "  Kernel Base: 0x80000000\r\n";
static char msg6[] = "  UART: 16550 Compatible\r\n\r\n";
static char msg7[] = "Hello, World!\r\n";
static char msg8[] = "This is a minimal operating system.\r\n";
static char msg9[] = "\r\nSystem initialization complete.\r\n";
static char msg10[] = "Entering idle loop...\r\n";

// 内核主函数
void kmain() {
    // 输出启动信息
    uart_puts(msg1);
    uart_puts(msg2);
    uart_puts(msg3);
    uart_puts(msg4);
    uart_puts(msg5);
    uart_puts(msg6);
    uart_puts(msg7);
    uart_puts(msg8);
    uart_puts(msg9);
    uart_puts(msg10);
    
    // 进入无限循环
    while (1) {
        // 空循环
    }
}
    