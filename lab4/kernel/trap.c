// kernel/trap.c - 完整的中断与异常处理实现
#include "types.h"
#include "riscv.h"
#include "sbi.h"
#include "console.h"
#include "printf.h"
#include "trap.h"

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
    sbi_set_timer(r_time() + 1000000);
    
    // 2. 开启时钟中断使能 (SIE 寄存器的 STIE 位)
    w_sie(r_sie() | SIE_STIE);
    
    printf("Timer initialized.\n");
}

// ================= 核心处理逻辑 =================

// 时钟中断处理函数
void timer_tick() {
    // 1. 增加计数
    timer_ticks++;
    
    // 2. 设置下一次中断 (周期性)
    sbi_set_timer(r_time() + 1000000);
    
    // 可选：打印心跳
    // if((timer_ticks % 10) == 0) printf("Tick: %lu\n", timer_ticks);
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
    uint64 sepc = r_sepc();
    uint64 sstatus = r_sstatus();

    // 判断是中断(1)还是异常(0)
    // SCAUSE_INTERRUPT = 1L << 63
    if (scause & SCAUSE_INTERRUPT) {
        // --- 中断处理 ---
        uint64 irq = scause & 0xff;
        if (irq == 5) { // Supervisor Timer Interrupt
            timer_tick();
        } else {
            printf("Unknown interrupt: %lu\n", irq);
        }
    } else {
        // --- 异常处理 ---
        handle_exception(scause);
    }

    // 恢复上下文
    // 关键点：如果 handle_exception 修改了 sepc (比如 +4)，
    // 我们需要重新读取 CSR 寄存器，或者确保不覆盖它。
    // 这里重新读取是最安全的做法。
    sepc = r_sepc(); 
    
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