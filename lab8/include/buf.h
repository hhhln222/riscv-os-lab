#ifndef _BUF_H_
#define _BUF_H_

#include "types.h"
#include "fs.h" // for BSIZE

struct buf {
    int valid;   // 数据是否有效 (已从磁盘读入)
    int disk;    // 是否脏 (需要写回磁盘)
    uint dev;
    uint blockno;
    uint refcnt;
    struct buf *prev; // LRU 链表
    struct buf *next;
    uchar data[BSIZE];
};

#endif