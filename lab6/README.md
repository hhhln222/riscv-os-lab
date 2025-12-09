# 从零构建操作系统 - 学生实验提交

**完成阶段**：实验6 - 系统调用

## 📌 项目简介

本项目是一个基于 RISC-V 架构从零开始构建的教学用操作系统。在实验6中，我们在实验5（进程调度）的基础上，实现了**系统调用分发框架**和**进程管理/内存管理的基础系统调用**。

✅ **Syscall 框架**：实现了 `syscall` 分发器和参数提取机制 (`argint`, `argaddr`)
✅ **进程管理**：实现了 `sys_exit`, `sys_wait`, `sys_getpid`，支持父子进程生命周期管理
✅ **内存增长**：实现了 `sys_sbrk` 模拟堆内存增长
✅ **安全性**：实现了基础的用户指针边界检查机制

## 🎯 实验目标

1. 理解用户态与内核态的边界及交互方式
2. 掌握系统调用参数传递与安全检查原理
3. 实现进程的阻塞等待 (`wait`) 与资源回收 (`exit`) 机制
4. 模拟进程堆空间的动态调整 (`sbrk`)

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
=== STARTING LAB 6 ADVANCED TESTS ===
...
=== Syscall Lifecycle Test (Wait/Exit) ===
[Parent] Created child PID=2. Waiting...
[Child] PID=2 is running.
[Child] sbrk test PASS: 4096 -> 4196
[Parent] Successfully waited for PID=2
[PASS] Exit status is correct (42).
...
Init process exiting? Don't do that.
System Halted.
```

## 📂 目录结构 (新增部分)
```
.
├── kernel/
│   ├── syscall.c            # [新增] 系统调用分发与参数获取
│   ├── sysproc.c            # [新增] 进程相关系统调用实现
│   ├── sysfile.c            # [新增] 文件相关系统调用实现
│   └── include/
│       └── syscall.h        # [新增] 系统调用号定义
```

## ✨ 功能进度

- [x] 实验1：裸机启动与UART字符输出
- [x] 实验2：内核printf与控制台扩展
- [x] 实验3：物理内存管理与页表系统
- [x] 实验4：中断与异常处理
- [x] 实验5：进程与上下文切换
- [x] 实验6：系统调用
- [ ] 实验7：简易文件系统
