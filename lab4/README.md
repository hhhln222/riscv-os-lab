# 从零构建操作系统 - 学生实验提交

**完成阶段**：实验4 - 中断处理与时钟管理

## 📌 项目简介

本项目是一个基于 RISC-V 架构从零开始构建的教学用操作系统。在实验4中，我们在实验3（内存管理）的基础上，构建了**内核中断处理框架**和**时钟驱动系统**，实现了操作系统核心的"心跳"机制和异常捕获能力。

✅ **中断向量表**：实现汇编级上下文保存与恢复 (`kernelvec.S`)
✅ **时钟驱动**：基于 OpenSBI 的时钟设置与中断响应
✅ **异常分发**：区分同步异常与异步中断，实现精准分发
✅ **自我保护**：捕获非法指令、系统调用和内存故障
✅ **自动化测试**：验证中断周期性触发和异常恢复逻辑

## 🎯 实验目标

1. 理解 RISC-V 特权级架构（S-mode vs M-mode）
2. 掌握中断上下文切换（Context Switch）原理
3. 实现基于 OpenSBI 的时钟中断驱动
4. 构建健壮的异常处理机制（Trap Handling）
5. 验证系统的自我恢复能力

## 🛠 开发环境

- **操作系统**：Ubuntu 22.04 LTS / WSL2
- **交叉编译工具链**：`riscv64-unknown-elf-gcc`
- **模拟器**：`qemu-system-riscv64` (OpenSBI backend)
- **构建系统**：GNU Make
- **调试器**：`riscv64-unknown-elf-gdb`

## 🚀 编译与运行

### 1. 编译内核
```bash
make clean  # 清理旧产物
make        # 生成 kernel.elf
```

### 2. 在 QEMU 中运行
```bash
make qemu
```

**预期输出**：
```
Hello, RISC-V OS (Lab 4)!
...
Trap system initialized.
Timer initialized.
Enabling interrupts...
=== STARTING TESTS ===
--- Test: Timer Interrupt ---
Waiting for 5 ticks (Start: 5)...
Ticks updated: 5 -> 10
[PASS] Timer interrupt is working!
--- Test: Exception Handling ---
[Exception] Illegal Instruction! scause=2 sepc=...
[PASS] Survived Illegal Instruction!
[Exception] System call (ecall) detected! scause=9
[PASS] Survived Ecall!
=== ALL TESTS COMPLETED ===
```

### 3. 调试（GDB）
```bash
# 终端1
make qemu-gdb

# 终端2
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) b kerneltrap
(gdb) c
```

## 📂 目录结构 (新增部分)
```
.
├── kernel/
│   ├── kernelvec.S          # [新增] 汇编中断入口 (Context Save/Restore)
│   ├── trap.c               # [新增] C语言中断分发与处理逻辑
│   ├── start.c              # [修改] 增加中断初始化流程
│   ├── printf.c             # [修改] 支持 64位整数打印 (%lu)
│   └── include/
│       ├── trap.h           # [新增] 中断接口定义
│       ├── sbi.h            # [新增] SBI 调用接口 (Set Timer)
│       └── riscv.h          # [修改] 补充 CSR 寄存器定义
```

## ✨ 功能进度

- [x] 实验1：裸机启动与UART字符输出
- [x] 实验2：内核printf与控制台扩展
- [x] 实验3：物理内存管理与页表系统
- [x] 实验4：中断与异常处理
- [ ] 实验5：进程与上下文切换
- [ ] 实验6：系统调用
- [ ] 实验7：简易文件系统

## 📊 测试验证

核心功能通过自动化测试验证：
- **时钟中断**：验证 `timer_ticks` 全局变量随时间增加
- **非法指令**：验证 CPU 能捕获并跳过非法指令 (sepc+4)
- **系统调用**：验证 `ecall` 指令能正确触发异常处理流程
- **中断开销**：测量 Trap 进出的时钟周期数

---