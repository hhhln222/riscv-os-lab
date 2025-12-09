// test.c - 测试用例实现
#include "test.h"
#include "syscall.h"
#include "printf.h"
#include "proc.h"

extern long syscall(long num, long a0, long a1, long a2);

// --- 包装函数 ---
int wait(int *status) {
    return syscall(SYS_wait, (uint64)status, 0, 0);
}

void exit(int status) {
    syscall(SYS_exit, status, 0, 0);
}

int sbrk(int n) {
    return syscall(SYS_sbrk, n, 0, 0);
}

// --- 子任务代码 ---
void child_task() {
    printf("[Child] PID=%d is running.\n", current_proc->pid);
    
    // 测试 sbrk
    int old_sz = sbrk(0);
    sbrk(100);
    int new_sz = sbrk(0);
    if (new_sz == old_sz + 100) {
        printf("[Child] sbrk test PASS: %d -> %d\n", old_sz, new_sz);
    } else {
        printf("[Child] sbrk test FAIL\n");
    }

    printf("[Child] Exiting with status 42...\n");
    exit(42);
}

// --- 父任务代码 ---
void parent_task() {
    printf("\n=== Syscall Lifecycle Test (Wait/Exit) ===\n");
    printf("[Parent] PID=%d creating child...\n", current_proc->pid);
    
    // 创建子进程 (模拟 fork)
    // 注意：create_kernel_process 会自动将 current_proc 设为父进程
    int child_pid = create_kernel_process("Child", child_task);
    
    printf("[Parent] Created child PID=%d. Waiting...\n", child_pid);
    
    int status = 0;
    // 等待子进程退出
    int waited_pid = wait(&status);
    
    if (waited_pid == child_pid) {
        printf("[Parent] Successfully waited for PID=%d\n", waited_pid);
        if (status == 42) {
            printf("[PASS] Exit status is correct (42).\n");
        } else {
            printf("[FAIL] Wrong exit status: %d\n", status);
        }
    } else {
        printf("[FAIL] Wait returned wrong PID: %d\n", waited_pid);
    }
    
    // 测试安全检查 (Task 6)
    printf("\n=== Security Check Test ===\n");
    // 尝试 wait 一个空指针地址
    // 我们的 syscall wait 实现里应该能处理，或者至少不崩溃
    // 目前的实现如果 addr=0 会忽略写回，这是安全的。
    // 如果要测试报错，可以传一个非法大地址。
    
    printf("Tests completed. Parent exiting.\n");
    exit(0);

    printf("Tests completed.\n");
    printf("System Halted.\n");
    
    while(1) {
        // 主动让出 CPU，让调度器运行空闲循环
        // 但不要退出，保持进程存在
        // yield(); // 或者 sleep
        
        // 或者直接在这里死循环
        asm volatile("wfi");
    }
}

void run_all_tests() {
    printf_color(36, "=== STARTING LAB 6 ADVANCED TESTS ===\n");
    
    // 只启动父进程，子进程由父进程创建
    create_kernel_process("Parent", parent_task);
    
    printf("Tests queued.\n");
}