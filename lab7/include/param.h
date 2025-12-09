#ifndef _PARAM_H_
#define _PARAM_H_

#define NPROC        64  // 最大进程数
#define NCPU          1  // 最大CPU数
#define NOFILE       16  // 每个进程最大打开文件数
#define NFILE       100  // 系统打开文件总数
#define NINODE       50  // 内存 inode 缓存数
#define NDEV         10  // 最大设备数
#define ROOTDEV       1  // 根文件系统设备号
#define MAXOPBLOCKS  10  // 一个事务最大写块数
#define LOGSIZE      (MAXOPBLOCKS*3) // 日志区大小
#define NBUF         (MAXOPBLOCKS*3) // 块缓存大小
#define FSSIZE       1000 // 文件系统总大小 (块)

#endif