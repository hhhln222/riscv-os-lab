# RISC-V OS Construction Project

## 从零构建操作系统 - 学生实验提交

**完成阶段**：实验3 - 页表与内存管理系统

## 📌 项目简介

本项目是一个基于 RISC-V 架构从零开始构建的教学用操作系统。在实验3中，我们在实验2（内核printf与控制台扩展）的基础上，实现了完整的**物理内存管理**和**虚拟内存系统**，包括：

✅ **物理内存分配器**：管理物理页帧，提供分配/释放接口
✅ **Sv39三级页表**：实现符合RISC-V规范的虚拟内存管理
✅ **内核页表初始化**：为内核代码、数据和设备建立恒等映射
✅ **虚拟内存激活**：配置SATP寄存器，启用分页机制
✅ **全面测试套件**：验证物理内存、页表功能和虚拟内存激活

## 🎯 实验目标

1. 理解RISC-V Sv39虚拟内存架构
2. 实现高效的物理内存分配算法
3. 构建可工作的页表管理系统
4. 安全地激活虚拟内存而不破坏系统状态
5. 通过系统化测试验证实现正确性

## 🛠 开发环境

- **操作系统**：Ubuntu 22.04 LTS / WSL2
- **交叉编译工具链**：`riscv64-unknown-elf-gcc`
- **模拟器**：`qemu-system-riscv64`
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
Hello, RISC-V OS with Physical Memory Management!
Physical memory manager initialized: 32256 pages available
=== STARTING PHYSICAL MEMORY AND PAGE TABLE TESTS ===
--- Test: Physical Memory Management ---
✓ Pages allocated at different addresses
✓ Page1 is page-aligned
✓ Memory write/read works correctly
...
--- Test: Virtual Memory Activation ---
Before enabling paging...
Kernel page table initialized
Virtual memory enabled
After enabling paging...
✓ Virtual memory activated successfully
✓ Kernel code is still accessible
✓ Kernel data is still accessible
UART still works after paging enabled
✓ Device access still works
=== ALL TESTS COMPLETED ===
```

**退出 QEMU**：按 `Ctrl+A`，松开后按 `X`

### 3. 调试（GDB）
```bash
# 终端1：启动 QEMU 并等待 GDB
make qemu-gdb

# 终端2：连接 GDB
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) b map_page
(gdb) c
```

## 📂 目录结构
```
.
├── Makefile                 # 构建脚本
├── README.md                # 项目说明
├── report.md                # 实验报告
└── kernel/
    ├── kernel.ld            # 链接脚本
    ├── entry.S              # 汇编入口
    ├── start.c              # C语言主入口
    ├── uart.c               # UART驱动
    ├── console.c            # 控制台抽象
    ├── printf.c             # 格式化输出
    ├── pmm.c                # 物理内存管理
    ├── vmm.c                # 虚拟内存管理
    ├── test.c               # 测试函数
    └── include/
        ├── riscv.h          # RISC-V架构定义
        ├── types.h          # 基础类型
        ├── stdarg.h         # 可变参数支持
        ├── uart.h           # UART接口
        ├── console.h        # 控制台接口
        ├── printf.h         # 打印接口
        ├── pmm.h            # 物理内存管理接口
        └── vmm.h            # 虚拟内存管理接口
```

## ✨ 功能进度

- [x] 实验1：裸机启动与UART字符输出
- [x] 实验2：内核printf与控制台扩展
- [x] 实验3：物理内存管理与页表系统
- [ ] 实验4：中断与异常处理
- [ ] 实验5：进程与上下文切换
- [ ] 实验6：系统调用
- [ ] 实验7：简易文件系统

## 📊 测试验证

所有核心功能均已通过自动化测试：
- 物理内存分配/释放功能
- 页表创建和地址映射
- 虚拟内存激活和功能验证
- 边界条件和错误处理

---