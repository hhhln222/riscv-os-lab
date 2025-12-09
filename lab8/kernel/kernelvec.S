# RISC-V 中断向量表 (汇编实现)
# kernelvec.S

    .section .text
    .globl kernelvec
    .type kernelvec, @function

# 中断向量入口
# 为了简化，我们使用直接模式（Direct Mode）
# 在实际系统中，这通常是通过跳转到中断处理程序来实现的
kernelvec:
    # 保存上下文 (保存所有寄存器)
    addi sp, sp, -256  # 为寄存器分配栈空间
    
    # 保存 a0-a7 (参数寄存器)
    sd a0, 0(sp)
    sd a1, 8(sp)
    sd a2, 16(sp)
    sd a3, 24(sp)
    sd a4, 32(sp)
    sd a5, 40(sp)
    sd a6, 48(sp)
    sd a7, 56(sp)
    
    # 保存 t0-t6 (临时寄存器)
    sd t0, 64(sp)
    sd t1, 72(sp)
    sd t2, 80(sp)
    sd t3, 88(sp)
    sd t4, 96(sp)
    sd t5, 104(sp)
    sd t6, 112(sp)
    
    # 保存 s0-s11 (保存寄存器)
    sd s0, 120(sp)
    sd s1, 128(sp)
    sd s2, 136(sp)
    sd s3, 144(sp)
    sd s4, 152(sp)
    sd s5, 160(sp)
    sd s6, 168(sp)
    sd s7, 176(sp)
    sd s8, 184(sp)
    sd s9, 192(sp)
    sd s10, 200(sp)
    sd s11, 208(sp)
    
    # 保存 ra, gp, tp
    sd ra, 216(sp)
    sd gp, 224(sp)
    sd tp, 232(sp)
    
    # 调用 C 语言中断处理函数
    call kerneltrap
    
    # 恢复上下文 (按相反顺序)
    ld ra, 216(sp)
    ld gp, 224(sp)
    ld tp, 232(sp)
    
    ld s0, 120(sp)
    ld s1, 128(sp)
    ld s2, 136(sp)
    ld s3, 144(sp)
    ld s4, 152(sp)
    ld s5, 160(sp)
    ld s6, 168(sp)
    ld s7, 176(sp)
    ld s8, 184(sp)
    ld s9, 192(sp)
    ld s10, 200(sp)
    ld s11, 208(sp)
    
    ld t0, 64(sp)
    ld t1, 72(sp)
    ld t2, 80(sp)
    ld t3, 88(sp)
    ld t4, 96(sp)
    ld t5, 104(sp)
    ld t6, 112(sp)
    
    ld a0, 0(sp)
    ld a1, 8(sp)
    ld a2, 16(sp)
    ld a3, 24(sp)
    ld a4, 32(sp)
    ld a5, 40(sp)
    ld a6, 48(sp)
    ld a7, 56(sp)
    
    addi sp, sp, 256  # 恢复栈指针
    
    # 返回到中断前的程序
    sret

.size kernelvec, .-kernelvec