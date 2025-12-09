// test.c - 测试用例实现
#include "test.h"
#include "syscall.h"
#include "printf.h"
#include "proc.h"
#include "param.h"

extern long syscall(long num, long a0, long a1, long a2);

// 包装函数
int set_priority(int prio) {
    return syscall(SYS_set_priority, prio, 0, 0);
}

int wait(int *status) {
    return syscall(SYS_wait, (uint64)status, 0, 0);
}

void exit(int status) {
    syscall(SYS_exit, status, 0, 0);
}

// 这是一个 CPU 密集型任务
void work_task() {
    int pid = current_proc->pid;
    int prio = current_proc->priority;
    
    printf("Task PID=%d started with Priority=%d\n", pid, prio);
    
    // 执行繁重的计算任务，让 CPU 忙碌
    // 增加循环次数以确保抢占效果明显
    for (int i = 0; i < 5; i++) {
        // 打印当前进度
        printf("Task PID=%d (Prio=%d) working %d/5...\n", pid, prio, i+1);
        
        // 忙等待 (模拟计算)
        for (volatile int j = 0; j < 50000000; j++); 
    }
    
    printf("Task PID=%d finished.\n", pid);
    exit(0);
}

void priority_test_entry() {
    printf("\n=== Priority Scheduling Test ===\n");
    
    // 创建三个子进程
    int pid_low, pid_mid, pid_high;
    
    // 1. Low Priority
    pid_low = create_kernel_process("LowTask", work_task);
    for(struct proc *p = proc; p < &proc[64]; p++) {
        if(p->pid == pid_low) p->priority = 2;
    }
    
    // 2. Mid Priority
    pid_mid = create_kernel_process("MidTask", work_task);
    for(struct proc *p = proc; p < &proc[64]; p++) {
        if(p->pid == pid_mid) p->priority = 5;
    }
    
    // 3. High Priority
    pid_high = create_kernel_process("HighTask", work_task);
    for(struct proc *p = proc; p < &proc[64]; p++) {
        if(p->pid == pid_high) p->priority = 8;
    }
    
    printf("Created 3 tasks: Low(2), Mid(5), High(8)\n");
    printf("Expected behavior: High finishes first, then Mid, then Low.\n");
    
    // [关键修改] 不要调用 exit(0)
    // 1. 降低自己的优先级，确保不抢占子进程资源 (哪怕子进程优先级也是5)
    //    虽然我们在内核态，直接改自己结构体最快
    current_proc->priority = 1; 
    printf("Parent (PID 1) lowered priority to 1 and waiting...\n");

    // 2. 等待所有子进程退出
    // 这会让 PID 1 进入 SLEEPING 状态，把 CPU 让给高优先级进程
    while(1) {
        int status;
        int child_pid = wait(&status);
        if (child_pid == -1) break; // 没有子进程了
        // printf("Child %d finished.\n", child_pid); // 可选日志
    }
    
    printf("All tasks completed. System Halted.\n");
    
    // 3. 死循环挂起系统
    while(1) {
        asm volatile("wfi");
    }
}

void run_all_tests() {
    printf_color(36, "=== STARTING PROJECT 1: PRIORITY SCHEDULING ===\n");
    create_kernel_process("PrioTest", priority_test_entry);
}