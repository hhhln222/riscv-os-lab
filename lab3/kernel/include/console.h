// console.h - 控制台接口定义
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"
#include "uart.h"

// ANSI颜色代码定义
#define COLOR_RESET   0
#define COLOR_BLACK   30
#define COLOR_RED     31
#define COLOR_GREEN   32
#define COLOR_YELLOW  33
#define COLOR_BLUE    34
#define COLOR_MAGENTA 35
#define COLOR_CYAN    36
#define COLOR_WHITE   37

// 控制台功能函数声明
void console_putc(int c);
void console_puts(char *s);
void console_clear(void);
void console_goto_xy(int x, int y);
void console_clear_line(void);
void console_set_color(int color);

#endif