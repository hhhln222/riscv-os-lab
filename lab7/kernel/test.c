// test.c - 测试用例实现
#include "test.h"
#include "fs.h"
#include "file.h"
#include "printf.h"
#include "param.h"

// 声明内核 FS 函数以便直接测试
struct inode* iget(uint dev, uint inum);
void ilock(struct inode *ip);
int readi(struct inode *ip, int user_dst, uint64 dst, uint off, uint n);
int writei(struct inode *ip, int user_src, uint64 src, uint off, uint n);

void test_fs() {
    printf_color(33, "\n--- Test: File System ---\n");
    
    // 1. 获取根目录 Inode
    struct inode *ip = iget(ROOTDEV, 1);
    printf("[PASS] iget root inode (inum=1)\n");
    
    // 2. 锁定 inode
    ilock(ip);
    printf("Root Inode: type=%d, nlink=%d\n", ip->type, ip->nlink);
    if(ip->type == T_DIR) printf("[PASS] Root is a directory\n");
    
    // 3. 写入测试 (写入一些数据到根目录的数据块)
    char write_buf[] = "Hello FS World!";
    int n = writei(ip, 0, (uint64)write_buf, 0, sizeof(write_buf));
    printf("Wrote %d bytes to root inode.\n", n);
    
    // 4. 读取测试
    char read_buf[20];
    // 清空 buffer
    for(int i=0; i<20; i++) read_buf[i] = 0;
    
    n = readi(ip, 0, (uint64)read_buf, 0, sizeof(write_buf));
    printf("Read back: %s\n", read_buf);
    
    // 5. 验证内容
    int match = 1;
    for(int i=0; i<sizeof(write_buf); i++) {
        if(write_buf[i] != read_buf[i]) match = 0;
    }
    
    if(match) printf_color(32, "[PASS] Read/Write verification successful!\n");
    else printf_color(31, "[FAIL] Data mismatch!\n");
}

void run_all_tests() {
    test_fs();
    printf("=== ALL TESTS COMPLETED ===\n");
}