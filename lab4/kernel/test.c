// test.c - 测试用例实现
#include "test.h"
#include "riscv.h" // for timer_ticks

// 声明外部变量
extern volatile uint64 timer_ticks;

void test_timer_interrupt() {
    printf("\n--- Test: Timer Interrupt ---\n");
    
    uint64 start = timer_ticks;
    printf("Waiting for 5 ticks (Start: %d)...\n", start);
    
    // 等待 5 个 Tick
    while(timer_ticks < start + 5) {
        // 防止编译器优化掉空循环
        printf("Waiting for interrupt %d...\n", timer_ticks);
        asm volatile("wfi"); // Wait For Interrupt
    }
    
    printf("Ticks updated: %lu -> %lu\n", start, timer_ticks);
    printf("[PASS] Timer interrupt is working!\n");
}

// 异常处理测试
void test_exception_handling(void) {
    printf_color(33, "\n--- Test: Exception Handling ---\n");

    // 测试非法指令异常 (Illegal Instruction)
    printf("Triggering Illegal Instruction...\n");
    // 强制插入一个 4 字节的非法指令 (全1在RISC-V中保留为非法)
    // 这样匹配我们在 trap.c 中 sepc + 4 的逻辑
    asm volatile(".4byte 0xffffffff");
    // 如果 handle_illegal_instruction 正确跳过了指令，程序会继续执行到这里
    printf("[PASS] Survived Illegal Instruction!\n");
    
    // 测试内存访问异常 (Load Page Fault)
    printf("Triggering Load Page Fault...\n");
    volatile uint64 *bad_addr = (volatile uint64*)0x40000000; 
    
    uint64 val = *bad_addr; // 这里应该触发异常并 Panic
    printf("[FAIL] Did not trigger Load Page Fault! Read value: %lx\n", val);
}

// 性能测试
void test_interrupt_overhead(void) {
    printf_color(33, "\n--- Test: Interrupt Overhead ---\n");
    
    uint64 start = r_time();
    
    // 触发 1000 次系统调用异常
    for(int i=0; i<1000; i++) {
        asm volatile("ecall");
    }
    
    uint64 end = r_time();
    uint64 diff = end - start;
    
    printf("1000 Ecalls took %lu cycles.\n", diff);
    printf("Average overhead: %lu cycles/trap.\n", diff / 1000);
}

void run_all_tests() {
    printf_color(36, "=== STARTING TESTS ===\n");
    
    // Lab 4 Timer Test
    test_timer_interrupt();

    // Lab 4 Performance Test
    test_interrupt_overhead();
    
    // Lab 4 Exception Test
    test_exception_handling();
    
    printf_color(36, "=== ALL TESTS COMPLETED ===\n");
}