# RISC-V OS Construction Project  
> 从零构建操作系统 - 学生实验提交
**完成阶段**：实验2 - 内核 printf 与清屏功能实现

## 📌 项目简介

本项目是一个基于 RISC-V 架构从零开始构建的教学用操作系统。在**实验2**中，我们在实验1（裸机启动 + UART 输出）的基础上，实现了功能完整的内核级 `printf` 系统，并扩展了控制台能力，支持 ANSI 转义序列，包括：

- ✅ **格式化输出**：支持 `%d`、`%x`、`%s`、`%p`、`%%` 等常用格式符；
- ✅ **边界值处理**：正确打印 `INT_MAX` 和 `INT_MIN`；
- ✅ **ANSI 控制台扩展**：
  - 清屏（`\033[2J`）
  - 光标定位（`\033[y;xH`）
  - 清除当前行（`\033[2K`）
  - 文本颜色设置（红、黄、绿、蓝等）
- ✅ **便捷接口**：提供 `printf_color()` 封装函数，自动管理颜色作用域；
- ✅ **自动化测试**：包含全面的测试用例，覆盖基础功能、边界条件和扩展特性。


## 🛠 开发环境

- **操作系统**：Ubuntu 22.04 LTS / WSL2  
- **交叉编译工具链**：`riscv64-unknown-elf-gcc`  
- **模拟器**：`qemu-system-riscv64`  
- **构建系统**：GNU Make  


## 🚀 编译与运行

### 1. 编译内核
```bash
make clean    # 清理旧产物
make          # 生成 kernel.elf
```

### 2. 在 QEMU 中运行
```bash
make qemu
```
**预期输出**：
- 彩色标题 `=== STARTING AUTOMATED TESTS ===`
- 多组测试结果，包括整数、十六进制、字符串、负数、边界值
- 使用 ANSI 颜色（红/黄/绿/蓝）打印的文本
- 成功执行 `console_clear_line()` 操作
- 彩色结尾 `=== ALL TESTS COMPLETED ===`

> **退出 QEMU**：按 `Ctrl+A`，松开后按 `X`

### 3. 调试（GDB）
```bash
# 终端1：启动 QEMU 并等待 GDB
make qemu-gdb

# 终端2：连接 GDB
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) b printf
(gdb) c
```

---

## 📂 目录结构

```text
.
├── Makefile                # 构建脚本
├── README.md               # 说明文件
├── report.md               # 实验报告
└── kernel/
    ├── kernel.ld           # 链接脚本
    ├── entry.S             # 汇编入口
    ├── start.c             # C 语言主入口
    ├── uart.c              # UART 驱动（硬件层）
    ├── console.c           # 控制台抽象
    ├── printf.c            # 格式化输出核心
    └── include/
        └── types.h         # 基础类型定义
```

## ✨ 功能进度

- [x] **实验1**：裸机启动与 UART 字符输出  
- [x] **实验2**：内核 printf 与控制台扩展
- [ ] 实验3：物理内存管理  
- [ ] 实验4：中断与异常处理  
- [ ] 实验5：进程与上下文切换  
- [ ] 实验6：系统调用  
- [ ] 实验7：简易文件系统  
