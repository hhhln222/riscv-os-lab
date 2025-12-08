# 从零构建操作系统 - 学生实验提交

**完成阶段**：实验5 - 进程管理与调度

## 📌 项目简介

本项目是一个基于 RISC-V 架构从零开始构建的教学用操作系统。在实验5中，我们在实验4（中断）的基础上，实现了**内核级进程管理**和**抢占式调度器**，系统正式具备了多任务并发能力。

✅ **进程控制块 (PCB)**：设计了 `struct proc` 和 `struct context`
✅ **上下文切换**：实现了汇编级 `swtch`，支持寄存器换入换出
✅ **调度器**：实现了 Round-Robin（轮转）调度算法
✅ **同步原语**：实现了 `sleep` 和 `wakeup` 机制
✅ **内核线程**：成功创建并运行多个并发内核任务

## 🎯 实验目标

1. 理解进程与线程的本质区别（执行流 vs 资源）
2. 掌握上下文切换（Context Switch）的底层实现
3. 实现基于时钟中断的抢占式调度
4. 理解并实现进程间的同步与互斥

## 🛠 开发环境

- **操作系统**：Ubuntu 22.04 LTS / WSL2
- **交叉编译工具链**：`riscv64-unknown-elf-gcc`
- **模拟器**：`qemu-system-riscv64`
- **构建系统**：GNU Make

## 🚀 编译与运行

### 1. 编译内核
```bash
make clean
make
```

### 2. 在 QEMU 中运行
```bash
make qemu
```

**预期输出**：
```
Process system initialized.
...
Scheduler started!
Simple task running (PID=1)
CPU Task 2: iteration 0
CPU Task 3: iteration 0  <-- 交替输出证明并发
...
Producer: produced 4 (count=5)
Consumer: consumed 0 (count=4) <-- 缓冲区满后自动切换
```

## 📂 目录结构 (新增部分)
```
.
├── kernel/
│   ├── proc.c               # [新增] 进程管理与调度器
│   ├── swtch.S              # [新增] 上下文切换汇编
│   ├── trap.c               # [修改] 时钟中断触发 yield()
│   ├── start.c              # [修改] 启动流程对接调度器
│   └── include/
│       └── proc.h           # [新增] 进程结构体定义
```

## ✨ 功能进度

- [x] 实验1：裸机启动与UART字符输出
- [x] 实验2：内核printf与控制台扩展
- [x] 实验3：物理内存管理与页表系统
- [x] 实验4：中断与异常处理
- [x] 实验5：进程与上下文切换
- [ ] 实验6：系统调用
- [ ] 实验7：简易文件系统