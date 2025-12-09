// kernel/syscall.c
#include "types.h"
#include "syscall.h"
#include "printf.h"
#include "proc.h"
#include "riscv.h" // PHYSTOP

// [Task 6] 安全检查
// 检查用户提供的地址是否合法 (模拟)
// 在真实 OS 中，这需要检查页表权限和 p->sz
int check_user_ptr(uint64 ptr, uint64 size) {
    if (ptr == 0) return -1; // 空指针
    if (ptr >= PHYSTOP) return -1; // 超过物理内存上限
    // 这里还可以加更多检查，比如是否属于该进程的内存范围
    return 0;
}

// 获取第 n 个系统调用参数 (整数)
// 目前我们的 syscall 函数已经把参数传进来了，这里做一个形式上的封装
int argint(int n, uint64 *ip, uint64 a0, uint64 a1, uint64 a2) {
    switch (n) {
        case 0: *ip = a0; break;
        case 1: *ip = a1; break;
        case 2: *ip = a2; break;
        default: return -1;
    }
    return 0;
}

// 获取第 n 个系统调用参数 (指针/地址) 并检查有效性
int argaddr(int n, uint64 *ip, uint64 a0, uint64 a1, uint64 a2) {
    uint64 addr;
    if (argint(n, (uint64*)&addr, a0, a1, a2) < 0)
        return -1;
    
    // 执行安全检查
    if (check_user_ptr(addr, 1) < 0)
        return -1;
    
    *ip = addr;
    return 0;
}

// 声明外部实现
int sys_getpid(void);
int sys_write(uint64 fd, uint64 buf, uint64 n);
int sys_exit(int status);
int sys_wait(uint64 addr); // [新增]
int sys_sbrk(int n);       // [新增]
extern int sys_set_priority(int n);

// 系统调用分发器 (更新版)
long syscall(long num, long a0, long a1, long a2) {
    long ret = -1;
    
    switch(num) {
        case SYS_getpid:
            ret = sys_getpid();
            break;
        case SYS_write:
            // 在这里可以使用 argaddr 检查 a1 (buf)
            // if (check_user_ptr(a1, a2) < 0) return -1;
            ret = sys_write(a0, a1, a2);
            break;
        case SYS_exit:
            ret = sys_exit(a0);
            break;
        case SYS_wait:
            ret = sys_wait(a0);
            break;
        case SYS_sbrk:
            ret = sys_sbrk(a0);
            break;
        case SYS_set_priority:
            ret = sys_set_priority(a0);
            break;
        default:
            printf("Unknown syscall %ld\n", num);
            ret = -1;
    }
    
    return ret;
}