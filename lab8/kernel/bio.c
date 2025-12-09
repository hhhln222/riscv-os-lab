#include "types.h"
#include "param.h"
#include "buf.h"
#include "printf.h"

extern void ramdisk_rw(struct buf *b, int write);

struct {
    struct buf buf[NBUF];
    struct buf head; // LRU 链表头
} bcache;

void binit(void) {
    struct buf *b;
    
    // 初始化循环双向链表
    bcache.head.prev = &bcache.head;
    bcache.head.next = &bcache.head;
    
    for(b = bcache.buf; b < bcache.buf+NBUF; b++){
        b->next = bcache.head.next;
        b->prev = &bcache.head;
        b->dev = -1;
        bcache.head.next->prev = b;
        bcache.head.next = b;
    }
    printf("binit: buffer cache initialized.\n");
}

// 获取缓存块
static struct buf* bget(uint dev, uint blockno) {
    struct buf *b;

    // 1. 查找缓存
    for(b = bcache.head.next; b != &bcache.head; b = b->next){
        if(b->dev == dev && b->blockno == blockno){
            b->refcnt++;
            return b;
        }
    }

    // 2. 分配新块 (LRU: 从尾部找 refcnt=0 的)
    for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
        if(b->refcnt == 0) {
            b->dev = dev;
            b->blockno = blockno;
            b->valid = 0;
            b->refcnt = 1;
            return b;
        }
    }
    
    printf("panic: bget: no buffers\n");
    while(1);
}

// 读取块
struct buf* bread(uint dev, uint blockno) {
    struct buf *b = bget(dev, blockno);
    if(!b->valid) {
        ramdisk_rw(b, 0); // 读磁盘
        b->valid = 1;
    }
    return b;
}

// 写块
void bwrite(struct buf *b) {
    ramdisk_rw(b, 1); // 写磁盘
}

// 释放块
void brelse(struct buf *b) {
    b->refcnt--;
    if (b->refcnt == 0) {
        // 移到头部 (MRU)
        b->next->prev = b->prev;
        b->prev->next = b->next;
        b->next = bcache.head.next;
        b->prev = &bcache.head;
        bcache.head.next->prev = b;
        bcache.head.next = b;
    }
}