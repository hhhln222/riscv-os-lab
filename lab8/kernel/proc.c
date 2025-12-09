#include "types.h"
#include "riscv.h"
#include "proc.h"
#include "pmm.h"
#include "printf.h"
#include "console.h"

#define NPROC 64

struct proc proc[NPROC];
struct proc *current_proc = 0; // 当前运行的进程

// 调度器上下文 (每个 CPU 一个，我们是单核)
struct context scheduler_context;

int nextpid = 1;

void proc_init(void) {
    struct proc *p;
    for(p = proc; p < &proc[NPROC]; p++) {
        p->state = UNUSED;
        p->kstack = 0;
    }
    printf("Process system initialized.\n");
}

// 分配一个新的进程结构
struct proc* alloc_proc(void) {
    struct proc *p;

    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->state == UNUSED) {
            goto found;
        }
    }
    return 0;

found:
    p->pid = nextpid++;
    p->state = USED;
    p->parent = 0;
    p->sz = 4096;
    p->priority = PRIO_DEFAULT; 

    // 分配内核栈
    if((p->kstack = alloc_page()) == 0) {
        p->state = UNUSED;
        return 0;
    }

    // 初始化上下文
    // sp 指向栈顶 (高地址)
    p->context.sp = (uint64)p->kstack + PGSIZE;
    
    // ra 设为 0? 不，create_process 会设置它
    return p;
}

// 创建内核线程
int create_kernel_process(char *name, void (*entry)(void)) {
    struct proc *p = alloc_proc();
    if(p == 0) return -1;

    // 设置返回地址 (ra) 为入口函数
    // 当 swtch 恢复这个上下文并执行 ret 时，CPU 就会跳转到 entry
    p->context.ra = (uint64)entry;

    // 如果当前有运行的进程，新进程就是它的子进程
    // 否则(比如在 start 里创建)，没有父进程
    if (current_proc) {
        p->parent = current_proc;
    }
    
    // 复制名字
    for(int i=0; name[i] && i<15; i++) p->name[i] = name[i];
    
    p->state = RUNNABLE;
    
    printf("Created process %d: %s (Parent PID: %d)\n", 
           p->pid, p->name, p->parent ? p->parent->pid : 0);
    return p->pid;
}

// 调度器 (Round-Robin)
// 算法：遍历所有进程，找到 RUNNABLE 中优先级最高的那个
void scheduler(void) {
    struct proc *p;
    struct proc *high_p;
    
    printf("Priority Scheduler started!\n");

    for(;;) {
        intr_on(); // 必须开中断，否则无法响应时钟

        int max_prio = -1;
        high_p = 0;

        // 第一轮扫描：寻找最高优先级
        for(p = proc; p < &proc[NPROC]; p++) {
            if(p->state == RUNNABLE) {
                if (p->priority > max_prio) {
                    max_prio = p->priority;
                    high_p = p; // 暂时选中它
                }
            }
        }

        // 如果找到了最高优先级的进程
        if(high_p != 0) {
            p = high_p;
            
            // 切换到进程
            p->state = RUNNING;
            current_proc = p;
            
            swtch(&scheduler_context, &p->context);
            
            current_proc = 0;
        } else {
            // 没有进程运行
            asm volatile("wfi");
        }
    }
}

// 主动让出 CPU
void yield(void) {
    if(current_proc) {
        current_proc->state = RUNNABLE;
        // 切换回调度器
        swtch(&current_proc->context, &scheduler_context);
    }
}

// 休眠 (阻塞)
// chan: 等待的通道 (通常是一个地址)
// lock: 自旋锁 (单核暂时不用)
void sleep(void *chan, void *lock) {
    if(current_proc == 0) panic("sleep");
    
    current_proc->chan = chan;
    current_proc->state = SLEEPING;
    
    // 切换回调度器
    swtch(&current_proc->context, &scheduler_context);
    
    // 唤醒后从这里继续
    current_proc->chan = 0;
}

// 唤醒
void wakeup(void *chan) {
    struct proc *p;
    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
        }
    }
}

// 让出 CPU 并切换回调度器 (不修改状态为 RUNNABLE)
// 用于 exit 或 sleep
void sched(void) {
    swtch(&current_proc->context, &scheduler_context);
}