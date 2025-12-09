#ifndef _FS_H_
#define _FS_H_

#include "types.h"

// 磁盘布局:
// [ boot block | super block | log | inode blocks | free bit map | data blocks ]
// [      0     |      1      | 2.. |      ...     |      ...     |    ...      ]

#define BSIZE 1024  // 块大小

// 超级块
struct superblock {
    uint magic;        // 魔数
    uint size;         // 文件系统总块数
    uint nblocks;      // 数据块数
    uint ninodes;      // inode 数
    uint nlog;         // 日志块数
    uint logstart;     // 日志区起始块号
    uint inodestart;   // inode 区起始块号
    uint bmapstart;    // 位图起始块号
};

#define FSMAGIC 0x10203040

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// 磁盘 Inode (On-disk inode)
struct dinode {
    short type;           // 文件类型
    short major;          // 主设备号
    short minor;          // 次设备号
    short nlink;          // 硬链接数
    uint size;            // 文件大小 (字节)
    uint addrs[NDIRECT+1]; // 数据块地址
};

// 文件类型
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

// 目录项
#define DIRSIZ 14
struct dirent {
    ushort inum;
    char name[DIRSIZ];
};

#endif