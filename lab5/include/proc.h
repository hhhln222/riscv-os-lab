#ifndef _PROC_H_
#define _PROC_H_

#include "types.h"
#include "riscv.h"
#include "trap.h"

// 进程状态
enum procstate {
    UNUSED,
    USED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};

// 上下文 (Context)：用于 swtch.S 保存/恢复寄存器
// 仅包含 Callee-saved 寄存器 (被调用者保存)
struct context {
    uint64 ra;
    uint64 sp;

    // s0-s11
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

// 进程控制块 (PCB)
struct proc {
    // struct spinlock lock; // 暂时简化，单核不需要自旋锁
    
    enum procstate state;    // 进程状态
    void *chan;              // 休眠等待通道 (用于 sleep/wakeup)
    int killed;              // 是否被杀死
    int xstate;              // 退出状态码
    int pid;                 // 进程ID
    
    // 内存相关
    // pagetable_t pagetable; // 用户页表 (Lab 6 再实现)
    // struct trapframe *trapframe; // 用户陷阱帧 (Lab 6 再实现)
    
    // 内核栈 (物理地址)
    void *kstack;
    
    // 调度上下文
    struct context context;
    
    char name[16];           // 进程名
};

extern struct proc *current_proc;
extern struct context scheduler_context;

// 声明 swtch (在 swtch.S 中定义)
void swtch(struct context *old, struct context *new);

// 声明 proc.c 中的函数
void proc_init(void);
struct proc* alloc_proc(void);
void scheduler(void);
void yield(void);
void sleep(void *chan, void *lock); // lock暂时传0
void wakeup(void *chan);
int create_kernel_process(char *name, void (*entry)(void));

#endif