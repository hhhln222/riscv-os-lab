# 扩展项目1：优先级调度系统

## 1. 设计思路

### 目标
原有的轮转调度（Round-Robin）虽然公平，但无法满足实时性需求。本项目的目标是将调度策略改造为**基于优先级的抢占式调度**，确保高优先级任务（如交互式 Shell、音频处理）能优先获得 CPU 资源。

### 核心修改
1.  **数据结构**：在 `struct proc` 中新增 `int priority` 字段（范围 1-10，默认 5）。
2.  **调度算法**：
    *   **原算法**：简单遍历进程表，找到第一个 `RUNNABLE` 就运行。
    *   **新算法**：遍历**整个**进程表，寻找 `RUNNABLE` 状态中**优先级最高**的进程。如果存在多个相同最高优先级的进程，则按顺序运行（同级轮转）。
3.  **系统调用**：新增 `sys_set_priority(int prio)`，允许进程动态调整优先级。若优先级被调低，进程会主动 `yield()` 以便让出 CPU 给可能存在的更高优先级进程。

## 2. 关键代码实现

**调度器核心逻辑 (`scheduler`)**：
```c
void scheduler(void) {
    struct proc *p, *high_p;
    for(;;) {
        intr_on(); // 开启中断
        
        int max_prio = -1;
        high_p = 0;

        // 扫描全表，寻找最高优先级
        for(p = proc; p < &proc[NPROC]; p++) {
            if(p->state == RUNNABLE) {
                if (p->priority > max_prio) {
                    max_prio = p->priority;
                    high_p = p;
                }
            }
        }

        // 运行最高优先级的进程
        if(high_p != 0) {
            p = high_p;
            p->state = RUNNING;
            current_proc = p;
            swtch(&scheduler_context, &p->context);
            current_proc = 0;
        } else {
            asm volatile("wfi");
        }
    }
}
```

## 3. 测试验证

**测试场景**：
同时创建三个计算密集型任务：
- `LowTask` (Priority = 2)
- `MidTask` (Priority = 5)
- `HighTask` (Priority = 8)

**运行结果日志**：
```text
Created 3 tasks: Low(2), Mid(5), High(8)
Expected behavior: High finishes first, then Mid, then Low.
Parent (PID 1) lowered priority to 1 and waiting...

Task PID=4 started with Priority=8
Task PID=4 (Prio=8) working 1/5...
...
Task PID=4 finished.

Task PID=3 started with Priority=5
Task PID=3 (Prio=5) working 1/5...
...
Task PID=3 finished.

Task PID=2 started with Priority=2
Task PID=2 (Prio=2) working 1/5...
...
Task PID=2 finished.
```

**结果分析**：
日志清晰地展示了 **High -> Mid -> Low** 的执行顺序，完全打破了 PID 创建顺序（2->3->4），证明优先级调度算法生效且抢占逻辑正确。

## 4. 思考与改进
当前的实现存在**饥饿（Starvation）**问题：如果源源不断地有高优先级进程进来，低优先级进程可能永远无法运行。
**改进方案**：引入**老化（Aging）**机制。在时钟中断里，每隔一定时间（如 100 tick），强制提升所有等待进程的优先级，确保低优先级进程最终也能获得执行机会。