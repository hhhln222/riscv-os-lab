// printf.h - 格式化输出接口
#ifndef _PRINTF_H_
#define _PRINTF_H_

#include "types.h"
#include "stdarg.h"
#include "uart.h"

// 格式化输出函数声明
void vprintf(char *fmt, va_list ap);
void printf(char *fmt, ...);
void printf_color(int color, char *fmt, ...);

#endif