.section .text
    .global _entry
_entry:
    # 1. 设置栈指针 (Stack Pointer)
    # 栈是向下增长的，所以我们要让 sp 指向栈顶 (高地址)
    # stack0 在 start.c 中定义，这里加载它的地址
    la sp, stack0
    
    # 栈的大小是 4096 字节 (4KB)
    # 将 sp 加上 4096，使其指向栈的顶部
    li a0, 4096
    add sp, sp, a0

    # BSS 清零
    la t0, sbss        # 加载 BSS 起始地址
    la t1, ebss        # 加载 BSS 结束地址
    bge t0, t1, call_start # 如果 起始 >= 结束，直接跳过 (BSS为空)

zero_loop:
    sb zero, 0(t0)     # 写入 0
    addi t0, t0, 1     # 指针移到下一个字节
    blt t0, t1, zero_loop # 如果还未到达结束地址，继续循环

call_start:
    # 2. 跳转到 C 语言入口函数 start()
    call start

    # 3. 如果 start 返回 (理论上不应该发生)，进入死循环
spin:
    j spin