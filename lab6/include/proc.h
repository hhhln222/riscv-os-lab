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

// 陷阱帧 (Trapframe)
// 用于在 trampoline.S 中保存用户态寄存器
// 虽然我们还没有 trampoline.S，但结构体必须先定义好
struct trapframe {
    /*   0 */ uint64 kernel_satp;   // kernel page table
    /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
    /*  16 */ uint64 kernel_trap;   // usertrap()
    /*  24 */ uint64 epc;           // saved user program counter
    /*  32 */ uint64 kernel_hartid; // saved kernel tp
    /*  40 */ uint64 ra;
    /*  48 */ uint64 sp;
    /*  56 */ uint64 gp;
    /*  64 */ uint64 tp;
    /*  72 */ uint64 t0;
    /*  80 */ uint64 t1;
    /*  88 */ uint64 t2;
    /*  96 */ uint64 s0;
    /* 104 */ uint64 s1;
    /* 112 */ uint64 a0; // 系统调用参数/返回值
    /* 120 */ uint64 a1; // 系统调用参数/返回值
    /* 128 */ uint64 a2; // 系统调用参数
    /* 136 */ uint64 a3;
    /* 144 */ uint64 a4;
    /* 152 */ uint64 a5;
    /* 160 */ uint64 a6;
    /* 168 */ uint64 a7; // 系统调用号
    // ... 其他寄存器 (s2-s11, t3-t6) 可根据需要添加
};

// 调度上下文 (Kernel Context)
struct context {
    uint64 ra;
    uint64 sp;
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

// 进程控制块
struct proc {
    enum procstate state;    
    void *chan;              
    int killed;              
    int xstate;              // 退出状态
    int pid;                 
    
    // [新增] 父子关系与内存
    struct proc *parent;     // 父进程指针
    uint64 sz;               // 进程内存大小 (用于 sbrk)

    void *kstack;            
    struct context context;  
    char name[16];           
};

extern struct proc *current_proc;
extern struct context scheduler_context;

void swtch(struct context *old, struct context *new);
void proc_init(void);
struct proc* alloc_proc(void);
void scheduler(void);
void sched(void); // [新增]
void yield(void);
void sleep(void *chan, void *lock);
void wakeup(void *chan);
int create_kernel_process(char *name, void (*entry)(void));

#endif