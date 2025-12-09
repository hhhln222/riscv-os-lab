// start.c - 系统启动入口
#include "types.h"
#include "console.h"
#include "printf.h"
#include "test.h"
#include "trap.h"  
#include "riscv.h" 
#include "proc.h"
#include "param.h"

extern void kernelvec();

__attribute__ ((aligned (16))) char stack0[4096];

void ramdisk_init(void);
void binit(void);
void mkfs(void);
void fsinit(int);
void fileinit(void);

void start() {
    console_clear();
    printf("Hello, RISC-V OS (Lab 7 - File System)!\n");
    
    pmm_init();
    kvminit();
    kvminithart();
    
    // 初始化 FS
    ramdisk_init();
    binit();
    fileinit();
    mkfs();       // 格式化 (创建 superblock 和 root inode)
    fsinit(ROOTDEV); // 挂载
    
    run_all_tests();
    
    while(1);
}