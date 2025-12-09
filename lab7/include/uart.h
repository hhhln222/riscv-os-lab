// uart.h - UART串口驱动接口
#ifndef _UART_H_
#define _UART_H_

#include "types.h"

// QEMU virt 机器的 UART0 基地址
#define UART0 0x10000000L

// UART驱动函数声明
void uart_putc(char c);
void uart_puts(char *s);

#endif