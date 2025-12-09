#ifndef _FILE_H_
#define _FILE_H_

#include "types.h"
#include "fs.h"

struct file {
    enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
    int ref; // 引用计数
    char readable;
    char writable;
    struct pipe *pipe; // 管道 (暂未实现)
    struct inode *ip;  // 指向内存 inode
    uint off;          // 读写偏移量
    short major;       // [新增] 主设备号 (用于 FD_DEVICE)
};

// [新增] 设备读写函数表定义
struct devsw {
    int (*read)(int, uint64, int);
    int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

// 内存 Inode (In-memory inode)
struct inode {
    uint dev;           // 设备号
    uint inum;          // Inode 编号
    int ref;            // 引用计数
    int valid;          // 是否从磁盘读取了
    
    short type;         // copy of disk inode
    short major;
    short minor;
    short nlink;
    uint size;
    uint addrs[NDIRECT+1];
};

#endif