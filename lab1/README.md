# RISC-V OS Construction Project
> 从零构建操作系统 - 学生实验提交
**完成阶段**：实验1 - RISC-V 引导与裸机启动

## 📌 项目简介
本项目是基于 RISC-V 架构从零构建的教学用操作系统。
当前阶段（实验1）实现了最小内核的引导过程，包括：
1.  **链接脚本配置**：将内核加载到 QEMU 指定的物理地址 `0x80000000`。
2.  **汇编引导**：初始化栈指针 (SP)。
3.  **硬件驱动**：基于 MMIO (内存映射I/O) 的 UART 串口驱动。
4.  **裸机输出**：成功在控制台打印 "Hello, RISC-V OS!"。

## 🛠 开发环境
*   **操作系统**：Ubuntu 22.04 LTS / WSL2
*   **编译器**：`riscv64-unknown-elf-gcc`
*   **模拟器**：`qemu-system-riscv64`
*   **构建工具**：Make

## 🚀 编译与运行

### 1. 编译内核
```bash
# 清理旧的构建产物
make clean

# 编译生成 kernel.elf
make
```

### 2. 运行 (QEMU)
```bash
# 启动 QEMU 运行 OS，并在终端显示输出
make qemu

# 预期输出：
# Hello, RISC-V OS!
# Experiment 1 Completed.

# 退出方法：按下 Ctrl+A，松开后按 X
```

### 3. 调试 (GDB)
```bash
# 终端 1：启动 QEMU 并等待 GDB 连接
make qemu-gdb

# 终端 2：启动 GDB 并连接
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) b start
(gdb) c
```

## 📂 目录结构
```text
.
├── Makefile              # 自动化构建脚本
├── README.md             # 项目说明文档
├── report.md             # 实验报告
└── kernel/
    ├── kernel.ld         # 链接脚本 (定义内存布局)
    ├── entry.S           # 汇编启动入口 (设置栈)
    ├── start.c           # 内核 C 语言入口
    ├── uart.c            # UART 串口驱动
    └── include/
        └── types.h       # 基础类型定义
```

## ✨ 功能进度
- [x] **实验1**：裸机启动与 UART 输出
- [ ] **实验2**：内核格式化输出
- [ ] **实验3**：内存管理
- [ ] **实验4**：中断处理
- [ ] **实验5**：进程管理
- [ ] **实验6**：系统调用
- [ ] **实验7**：文件系统