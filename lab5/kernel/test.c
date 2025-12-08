// test.c - 测试用例实现
#include "test.h"
#include "proc.h"
#include "riscv.h"

// 声明外部变量 (如果需要)
// extern volatile uint64 timer_ticks;

// --- 测试任务函数 ---

void simple_task() {
    printf("Simple task running (PID=%d)\n", current_proc->pid);
    // 任务结束，在这里死循环或者让出
    // 真实的 OS 会调用 exit()，这里简单处理
    current_proc->state = ZOMBIE; 
    swtch(&current_proc->context, &scheduler_context);
}

void cpu_intensive_task() {
    int pid = current_proc->pid;
    for (int i = 0; i < 5; i++) { // 增加循环次数到 10
        printf("CPU Task %d: iteration %d\n", pid, i);
        // 加大延时，确保它跑不完一个时间片
        for(volatile int j=0; j<10000000; j++); 
    }
    printf("CPU Task %d finished\n", pid);
    current_proc->state = ZOMBIE;
    swtch(&current_proc->context, &scheduler_context);
}

// 生产者-消费者测试
#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

void producer_task() {
    for (int i = 0; i < 10; i++) {
        // 如果缓冲区满了，休眠
        while (count == BUFFER_SIZE) {
            sleep(&count, 0); // 睡在 count 变量的地址上
        }
        
        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Producer: produced %d (count=%d)\n", i, count);
        
        // 唤醒消费者
        wakeup(&count);
    }
    printf("Producer finished\n");
    current_proc->state = ZOMBIE;
    swtch(&current_proc->context, &scheduler_context);
}

void consumer_task() {
    for (int i = 0; i < 10; i++) {
        // 如果缓冲区空了，休眠
        while (count == 0) {
            sleep(&count, 0);
        }
        
        int data = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer: consumed %d (count=%d)\n", data, count);
        
        // 唤醒生产者
        wakeup(&count);
    }
    printf("Consumer finished\n");
    current_proc->state = ZOMBIE;
    swtch(&current_proc->context, &scheduler_context);
}

// --- 测试入口 ---

void test_process_creation() {
    printf_color(33, "\n--- Test: Process Creation ---\n");
    int pid = create_kernel_process("Simple", simple_task);
    if(pid > 0) printf("[PASS] Created process PID=%d\n", pid);
    else printf("[FAIL] Failed to create process\n");
}

void test_scheduler() {
    printf_color(33, "\n--- Test: Scheduler (Round Robin) ---\n");
    create_kernel_process("CPU 1", cpu_intensive_task);
    create_kernel_process("CPU 2", cpu_intensive_task);
    printf("Created 2 CPU tasks. They should run concurrently.\n");
}

void test_synchronization() {
    printf_color(33, "\n--- Test: Synchronization (Sleep/Wakeup) ---\n");
    create_kernel_process("Prod", producer_task);
    create_kernel_process("Cons", consumer_task);
}

// 注意：Lab 5 的测试与 Lab 4 不同。
// Lab 4 是在 start() 里直接调用的。
// Lab 5 需要把这些测试本身作为"初始进程"或者在 start() 启动调度器前创建好。
// 我们修改 run_all_tests，只创建进程，不等待结果 (因为没有 wait 系统调用)

void run_all_tests() {
    printf_color(36, "=== STARTING PROCESS TESTS ===\n");
    
    test_process_creation();
    test_scheduler();
    test_synchronization();
    
    printf("Tests queued. Scheduler will run them now.\n");
}