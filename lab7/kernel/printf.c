// printf.c - 格式化输出实现
#include "printf.h"
#include "console.h"

static char digits[] = "0123456789abcdef";

// 打印整数
static void printint(int xx, int base, int sign) {
    char buf[20];
    int i;
    unsigned long x;

    if(sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while((x /= base) != 0);

    if(sign)
        buf[i++] = '-';

    while(--i >= 0)
        console_putc(buf[i]);
}

static void printptr(uint64 x) {
    int i;
    console_putc('0');
    console_putc('x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        console_putc(digits[x >> 60]);
}

void vprintf(char *fmt, va_list ap) {
    int i, c;
    char *s;

    for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
        if(c != '%'){
            console_putc(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        
        if(c == 'l') {
            c = fmt[++i] & 0xff;
        }

        if(c == 0) break;

        switch(c){
        case 'd':
            printint(va_arg(ap, long), 10, 1);
            break;
        case 'u': // 支持 %u
            printint(va_arg(ap, long), 10, 0);
            break;
        case 'x':
            printint(va_arg(ap, long), 16, 0);
            break;
        case 'p':
            printptr(va_arg(ap, uint64));
            break;
        case 's':
            if((s = va_arg(ap, char*)) == 0)
                s = "(null)";
            for(; *s; s++)
                console_putc(*s);
            break;
        case '%':
            console_putc('%');
            break;
        default:
            console_putc('%');
            console_putc(c);
            break;
        }
    }
}

// 标准 printf
void printf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

// [新增] 带颜色的 printf
// color: 30-37 (前景色), 0 (重置)
void printf_color(int color, char *fmt, ...) {
    va_list ap;
    
    // 1. 设置颜色
    console_set_color(color);
    
    // 2. 打印内容
    va_start(ap, fmt);
    vprintf(fmt, ap); // 调用核心逻辑
    va_end(ap);
    
    // 3. 重置颜色
    console_set_color(0); 
}