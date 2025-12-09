// sbi.h - SBI接口定义
#ifndef _SBI_H_
#define _SBI_H_

#include "types.h"

// 使用 Legacy SBI (EID=0) 设置定时器
static inline void sbi_set_timer(uint64 stime_value) {
    register uint64 a0 asm("a0") = stime_value;
    register uint64 a7 asm("a7") = 0x00; // Extension ID = 0 (Set Timer)
    asm volatile("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

#endif