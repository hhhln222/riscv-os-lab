#include "types.h"

// --- 1. 定义 ANSI 颜色代码 ---
#define COLOR_RESET   0
#define COLOR_RED     31
#define COLOR_GREEN   32
#define COLOR_YELLOW  33
#define COLOR_BLUE    34
#define COLOR_MAGENTA 35
#define COLOR_CYAN    36
#define COLOR_WHITE   37

// --- 2. 声明外部函数 ---
void printf(char *fmt, ...);
void printf_color(int color, char *fmt, ...);
void console_set_color(int color);
void console_goto_xy(int x, int y);
void console_clear_line();

// --- 3. 测试用例 ---

// 1. 基础输出测试 (保持不变)
void test_printf_basic() {
    printf("\n--- Test: Basic Printf ---\n");
    printf("Testing integer: %d\n", 42);
    printf("Testing negative: %d\n", -123);
    printf("Testing zero: %d\n", 0);
    printf("Testing hex: 0x%x\n", 0xABC);
    printf("Testing string: %s\n", "Hello");
    printf("Testing char: %c\n", 'X');
    printf("Testing percent: %%\n");
}

// 2. 边界条件测试 (保持不变)
void test_printf_edge_cases() {
    printf("\n--- Test: Edge Cases ---\n");
    printf("INT_MAX: %d\n", 2147483647);
    printf("INT_MIN: %d\n", -2147483648);
    printf("NULL string: %s\n", (char*)0);
    printf("Empty string: %s\n", "");
}

// 3. 扩展功能测试 
void test_extensions() {
    printf("\n--- Test: Extensions ---\n");
    
    // 方式 A: 手动设置颜色
    console_set_color(COLOR_RED);
    printf("This should be RED (Manual Set).\n");
    
    console_set_color(COLOR_YELLOW);
    printf("This should be YELLOW (Manual Set).\n");
    
    console_set_color(COLOR_RESET);
    
    // 方式 B: 使用新的 printf_color 封装
    printf_color(COLOR_GREEN, "This should be GREEN (Using printf_color).\n");
    printf_color(COLOR_BLUE,  "This should be BLUE (Using printf_color).\n");

    printf("Testing Clear Line in 3 seconds...");
    
    // 简单的延时循环
    for(volatile int i=0; i<100000000; i++); // 稍微调大一点以便肉眼观察
    
    // 清行
    console_clear_line(); 
    printf("Line cleared! (And cursor assumes start of line)\n");
}

// 运行所有测试
void run_all_tests() {
    printf_color(COLOR_CYAN, "=== STARTING AUTOMATED TESTS ===\n");
    
    test_printf_basic();
    test_printf_edge_cases();
    test_extensions();
    
    printf_color(COLOR_CYAN, "=== ALL TESTS COMPLETED ===\n");
    // 函数返回后，start.c 中的 while(1) 会接管系统，保持死循环
}