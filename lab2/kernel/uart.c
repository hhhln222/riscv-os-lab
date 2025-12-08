#include "types.h"

// QEMU virt 机器的 UART0 基地址
#define UART0 0x10000000L

// 发送一个字符
void uart_putc(char c) {
    // 直接向寄存器写数据
    // volatile 关键字告诉编译器不要优化这行代码，必须真正写入内存
    *(volatile uint8 *)UART0 = c;
}

// 发送字符串
void uart_puts(char *s) {
    while(*s){
        uart_putc(*s++);
    }
}