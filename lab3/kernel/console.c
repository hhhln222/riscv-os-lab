#include "console.h"
#include "uart.h"
#include "printf.h"

// 控制台输出字符
void console_putc(int c) {
    // 这里可以处理特殊字符，例如将 \n 转换为 \r\n
    if(c == '\n') {
        uart_putc('\r');
    }
    uart_putc(c);
}

// 输出字符串
void console_puts(char *s) {
    while(*s) {
        console_putc(*s++);
    }
}

// ANSI 转义序列清屏
// \033[2J: 清除整个屏幕
// \033[H:  光标移动到左上角 (1,1)
void console_clear() {
    console_puts("\033[2J\033[H");
}

// [扩展] 光标定位 (ANSI: \033[y;xH)
void console_goto_xy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// [扩展] 清除当前行 (ANSI: \033[2K) 并回车
void console_clear_line() {
    console_puts("\033[2K\r");
}

// 整数转字符串辅助函数
static void _putint(int n) {
    char buf[16];
    int i = 0;
    if(n == 0) { console_putc('0'); return; }
    while(n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    while(--i >= 0) console_putc(buf[i]);
}

// [扩展] 设置颜色
// color code: 30=黑, 31=红, 32=绿, 33=黄, 34=蓝, 35=紫, 36=青, 37=白
// 0=重置
// 使用 ANSI 转义序列: \033[<color>m
void console_set_color(int color) {
    uart_putc(27); // ESC (ASCII 27)
    uart_putc('[');
    
    if (color == 0) {
        uart_putc('0'); // 重置
    } else {
        _putint(color); // 输出颜色代码 (如 31)
    }
    
    uart_putc('m');
}