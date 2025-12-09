#include "types.h"
#include "printf.h"
#include "console.h"

// 简化的 write 实现，只支持写 stdout/stderr
int sys_write(uint64 fd, uint64 buf, uint64 n) {
    char *str = (char*)buf;
    
    // 1 = stdout, 2 = stderr
    if (fd == 1 || fd == 2) {
        for(int i=0; i<n; i++) {
            console_putc(str[i]);
        }
        return n;
    }
    return -1;
}