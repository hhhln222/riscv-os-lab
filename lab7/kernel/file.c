#include "types.h"
#include "riscv.h"
#include "param.h"
#include "fs.h"
#include "file.h"
#include "printf.h"

// 引用 fs.c 中的函数
extern void ilock(struct inode *ip);
extern void iput(struct inode *ip);
extern int readi(struct inode *ip, int user_dst, uint64 dst, uint off, uint n);
extern int writei(struct inode *ip, int user_src, uint64 src, uint off, uint n);

// 全局打开文件表
struct {
    struct file file[NFILE];
} ftable;

// 设备读写表 (用于 console 等设备文件)
struct devsw devsw[NDEV];

void fileinit(void) {
    // 初始化文件表锁 (单核暂略)
    printf("fileinit: file table initialized\n");
}

// 分配一个文件结构体
struct file* filealloc(void) {
    struct file *f;
    for(f = ftable.file; f < ftable.file + NFILE; f++){
        if(f->ref == 0){
            f->ref = 1;
            return f;
        }
    }
    return 0;
}

// 增加引用计数
struct file* filedup(struct file *f) {
    if(f->ref < 1) {
        printf("panic: filedup\n");
        while(1);
    }
    f->ref++;
    return f;
}

// 关闭文件
void fileclose(struct file *f) {
    if(f->ref < 1) {
        printf("panic: fileclose\n");
        while(1);
    }
    
    if(--f->ref > 0)
        return;

    if(f->type == FD_INODE){
        // 释放 inode 引用
        iput(f->ip);
    }
    
    f->type = FD_NONE;
}

// 获取文件状态
int filestat(struct file *f, uint64 addr) {
    if(f->type == FD_INODE || f->type == FD_DEVICE){
        ilock(f->ip);
        // stati(f->ip, addr); // 暂未实现 stati，略过
        return 0;
    }
    return -1;
}

// 读文件
int fileread(struct file *f, uint64 addr, int n) {
    int r = 0;

    if(f->readable == 0) return -1;

    if(f->type == FD_INODE){
        ilock(f->ip);
        if((r = readi(f->ip, 1, addr, f->off, n)) > 0)
            f->off += r;
        // iunlock(f->ip); // 简化版 ilock 没有锁机制，不需要 unlock
    } else if(f->type == FD_DEVICE){
        if(f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
            return -1;
        r = devsw[f->major].read(1, addr, n);
    }
    return r;
}

// 写文件
int filewrite(struct file *f, uint64 addr, int n) {
    int r = 0;

    if(f->writable == 0) return -1;

    if(f->type == FD_INODE){
        // max file size check skipped
        ilock(f->ip);
        if((r = writei(f->ip, 1, addr, f->off, n)) > 0)
            f->off += r;
        // iunlock(f->ip);
    } else if(f->type == FD_DEVICE){
        if(f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
            return -1;
        r = devsw[f->major].write(1, addr, n);
    }

    return r;
}