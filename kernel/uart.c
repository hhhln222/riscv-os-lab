#include "uart.h"

// UART寄存器地址
#define UART_BASE 0x10000000
#define UART_THR  (UART_BASE + 0)  // 发送保持寄存器
#define UART_LSR  (UART_BASE + 5)  // 线路状态寄存器
#define LSR_THRE  0x20             // 发送保持寄存器为空

// 输出一个字符
void uart_putc(char c) {
    // 等待发送缓冲区为空
    while ((*(volatile unsigned char *)UART_LSR & LSR_THRE) == 0);
    // 发送字符，如果是换行则同时发送回车
    if (c == '\n') {
        *(volatile unsigned char *)UART_THR = '\r';
        while ((*(volatile unsigned char *)UART_LSR & LSR_THRE) == 0);
    }
    *(volatile unsigned char *)UART_THR = c;
}

// 输出字符串
void uart_puts(char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}
    