// kernel/trap.c - 完整的中断与异常处理实现
#include "types.h"
#include "riscv.h"
#include "sbi.h"
#include "console.h"
#include "printf.h"
#include "trap.h"
#include "proc.h"

// 全局 Tick 计数器
volatile uint64 timer_ticks = 0;

// 前向声明异常处理辅助函数
void handle_syscall(void);
void handle_illegal_instruction(void);
void handle_instruction_page_fault(void);
void handle_load_page_fault(void);
void handle_store_page_fault(void);
void panic(char *s);

// ================= 初始化相关 =================

void trap_init() {
    printf("Trap system initialized.\n");
}

void timer_init() {
    // 1. 设置第一次时钟中断
    sbi_set_timer(r_time() + 100000);
    
    // 2. 开启时钟中断使能 (SIE 寄存器的 STIE 位)
    w_sie(r_sie() | SIE_STIE);
    
    printf("Timer initialized.\n");
}

// ================= 核心处理逻辑 =================

// 时钟中断处理函数
void timer_tick() {
    timer_ticks++;
    sbi_set_timer(r_time() + 100000); // 10ms
    
    // [Lab 5] 如果有进程在运行，且是时钟中断，则强制切换
    // 注意：这里没有检查是否处于内核态/用户态，
    // 因为我们目前只有内核线程，所以总是触发。
    if (current_proc != 0 && current_proc->state == RUNNING) {
        yield();
    }
}

// 异常分发函数
void handle_exception(uint64 scause) {
    switch (scause) {
        case 2: // 非法指令
            handle_illegal_instruction();
            break;
        case 8: // 用户态系统调用
        case 9: // 内核态系统调用 (supervisor ecall)
            handle_syscall();
            break;
        case 12: // 指令页故障
            handle_instruction_page_fault();
            break;
        case 13: // 加载页故障
            handle_load_page_fault();
            break;
        case 15: // 存储页故障
            handle_store_page_fault();
            break;
        default:
            printf("Unhandled Exception: scause=%lu sepc=%lx stval=%lx\n", 
                   scause, r_sepc(), r_stval());
            panic("Unknown exception");
    }
}

// C 语言中断/异常主入口
void kerneltrap() {
    uint64 scause = r_scause();
    uint64 sepc = r_sepc(); // 1. 这里保存了被中断时的正确地址
    uint64 sstatus = r_sstatus();

    // 判断是否是中断 (最高位为1)
    if(scause & SCAUSE_INTERRUPT) {
        uint64 cause = scause & 0xff;
        if(cause == 5) { 
            timer_tick(); // 可能会发生任务切换
        } else {
            printf("Unknown interrupt: %lu\n", cause);
        }
    } else {
        // 异常处理
        handle_exception(scause);
        
        // 针对系统调用/非法指令，我们需要更新 sepc
        // 这里需要手动更新本地变量，以便最后写回
        if (scause == 8 || scause == 9 || scause == 2) {
            sepc += 4;
        }
    }

    // 2. 恢复上下文
    // 必须使用局部变量 sepc (它保存在栈上，是安全的)
    // 绝对不能再次调用 r_sepc()！
    w_sepc(sepc);
    w_sstatus(sstatus);
}

// ================= 具体异常处理函数 =================

void panic(char *s) {
    printf("panic: %s\n", s);
    while(1);
}

// 系统调用处理
void handle_syscall() {
    // 打印日志方便调试
    printf("[Exception] System call (ecall) detected!\n");
    
    // 关键：将 SEPC + 4，跳过 ecall 指令
    // 否则返回后会重新执行 ecall，导致死循环
    w_sepc(r_sepc() + 4);
}

// 非法指令处理
void handle_illegal_instruction() {
    printf("[Exception] Illegal Instruction! sepc=%lx\n", r_sepc());
    
    // 关键：跳过非法指令
    // 注意：压缩指令集下指令可能是 2 字节，但我们在测试中强制使用了 4 字节非法指令
    w_sepc(r_sepc() + 4); 
}

// 指令页故障
void handle_instruction_page_fault() {
    printf("[Exception] Instruction Page Fault! stval=%lx\n", r_stval());
    panic("IPF: Killing process");
}

// 加载页故障
void handle_load_page_fault() {
    printf("[Exception] Load Page Fault! Address=%lx\n", r_stval());
    panic("LPF: Killing process");
}

// 存储页故障
void handle_store_page_fault() {
    printf("[Exception] Store Page Fault! Address=%lx\n", r_stval());
    panic("SPF: Killing process");
}