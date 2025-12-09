// test.h - 测试用例接口
#ifndef _TEST_H_
#define _TEST_H_

#include "types.h"
#include "console.h"
#include "printf.h"
#include "pmm.h"
#include "vmm.h"
#include "uart.h"

void test_physical_memory(void);
void test_pagetable(void);
void test_virtual_memory(void);
void run_all_tests(void);

#endif