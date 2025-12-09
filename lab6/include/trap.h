// trap.h - 陷阱处理接口
#ifndef _TRAP_H_
#define _TRAP_H_

#include "types.h"

// 全局时钟滴答计数器
// volatile 关键字确保编译器每次都从内存读取，而不是缓存到寄存器
extern volatile uint64 timer_ticks;

// 中断系统初始化 (Lab 4)
void trap_init(void);

// 时钟中断初始化 (Lab 4)
void timer_init(void);

// C语言中断处理主入口 (由 kernelvec.S 调用)
void kerneltrap(void);

// 汇编中断入口 (在 kernelvec.S 中定义)
// 用于在 start.c 中设置 stvec 寄存器
void kernelvec(void);

void handle_exception(uint64 scause);
void panic(char *s);

#endif