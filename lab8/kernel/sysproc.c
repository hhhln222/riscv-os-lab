// kernel/sysproc.c
#include "types.h"
#include "proc.h"
#include "printf.h"
#include "syscall.h"
#include "param.h"

extern struct proc proc[];
extern void sched(void);

int sys_getpid(void) {
    return current_proc->pid;
}

// 调整进程内存大小 (模拟)
int sys_sbrk(int n) {
    uint64 addr = current_proc->sz;
    // 简单的增长，暂不分配物理页
    // 在真实 OS 中这里需要 kalloc 并 mappages
    if (n > 0) {
        current_proc->sz += n;
    } else if (n < 0) {
        current_proc->sz += n; // 缩减
    }
    return addr;
}

// 退出进程
int sys_exit(int status) {
    if(current_proc->pid == 1) {
        printf("Init process exiting? Don't do that.\n");
        return -1;
    }

    printf("[Syscall] Process %d exiting with status %d.\n", current_proc->pid, status);

    // 1. 如果有父进程，唤醒父进程 (因为父进程可能在 wait)
    if(current_proc->parent) {
        wakeup(current_proc->parent);
    }

    // 2. 将子进程过继给 init (PID 1)
    // (由于我们还没有完善的 init 进程，这步先跳过，只做简单的僵尸处理)
    
    current_proc->xstate = status;
    current_proc->state = ZOMBIE;
    
    sched(); // 调度出去，不再返回
    return 0;
}

// 等待子进程退出
// addr: 接收子进程 exit status 的指针地址
int sys_wait(uint64 addr) {
    struct proc *np;
    int havekids, pid;
    struct proc *p = current_proc;

    // 死循环等待
    for(;;) {
        havekids = 0;
        // 遍历进程表找我的子进程
        for(np = proc; np < &proc[64]; np++) {
            if(np->parent == p) {
                havekids = 1;
                
                // 找到一个僵尸子进程
                if(np->state == ZOMBIE) {
                    pid = np->pid;
                    
                    // 如果提供了地址，把退出状态写回去
                    if(addr != 0 && addr != -1) {
                        *(int*)addr = np->xstate; 
                    }
                    
                    // 回收资源
                    // 实际 OS 还需要 kfree(kstack) 等
                    np->pid = 0;
                    np->parent = 0;
                    np->name[0] = 0;
                    np->killed = 0;
                    np->state = UNUSED;
                    
                    return pid;
                }
            }
        }

        // 如果没有子进程，立即返回错误
        if(!havekids || p->killed) {
            return -1;
        }

        // 有子进程但都没退出，休眠等待
        // 这里的通道是 p 自己，对应 exit 里的 wakeup(parent)
        sleep(p, 0); 
    }
}

// 设置当前进程优先级的系统调用
int sys_set_priority(int new_prio) {
    if (new_prio < PRIO_MIN || new_prio > PRIO_MAX) {
        return -1; // 非法优先级
    }
    
    int old_prio = current_proc->priority;
    current_proc->priority = new_prio;
    
    printf("[System] PID %d priority changed: %d -> %d\n", 
           current_proc->pid, old_prio, new_prio);
           
    // 如果优先级降低了，主动让出 CPU，让调度器重新评估
    // 因为可能存在比新优先级更高的其他进程
    if (new_prio < old_prio) {
        yield(); 
    }
    
    return old_prio; // 返回旧优先级
}