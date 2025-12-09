#include "types.h"
#include "riscv.h"
#include "param.h"
#include "fs.h"
#include "buf.h"
#include "printf.h"

// 在内存中开辟一块空间作为磁盘
static uchar disk[FSSIZE * BSIZE];

void ramdisk_init(void) {
    printf("ramdisk: initialized (%d blocks).\n", FSSIZE);
}

// 读写接口
void ramdisk_rw(struct buf *b, int write) {
    uint64 offset = b->blockno * BSIZE;
    
    if (offset >= sizeof(disk)) {
        printf("panic: ramdisk_rw: block out of range\n");
        while(1);
    }

    if (write) {
        // 写入：从 buffer 复制到 disk
        for(int i=0; i<BSIZE; i++) disk[offset+i] = b->data[i];
    } else {
        // 读取：从 disk 复制到 buffer
        for(int i=0; i<BSIZE; i++) b->data[i] = disk[offset+i];
    }
}

// 格式化磁盘 (mkfs)
// 简单的文件系统初始化：写入 Superblock 和根目录
void mkfs(void) {
    struct superblock sb;
    sb.magic = FSMAGIC;
    sb.size = FSSIZE;
    sb.nblocks = 900; // 剩下的给数据
    sb.ninodes = 100;
    sb.nlog = 0; // 暂时不用日志
    sb.logstart = 2; 
    sb.inodestart = 2; // Block 2 开始是 inodes
    sb.bmapstart = 2 + (sb.ninodes / (BSIZE / sizeof(struct dinode))) + 1;

    // 写入 Superblock (Block 1)
    // Block 0 是 boot block，跳过
    struct buf b; // 临时构造一个 buffer
    b.blockno = 1;
    // 复制数据
    uchar *p = (uchar*)&sb;
    for(int i=0; i<sizeof(sb); i++) b.data[i] = p[i];
    ramdisk_rw(&b, 1);

    // 初始化根目录 (Inode 1)
    struct dinode root;
    root.type = T_DIR;
    root.nlink = 1;
    root.size = 0;
    root.addrs[0] = sb.bmapstart + 10; // 分配一个数据块给根目录
    
    // 计算 Inode 1 的位置
    // Inode 0 不用。Inode 1 在 inode block 的第 1 个位置 (offset 64)
    uint64 offset = (2 * BSIZE) + sizeof(struct dinode);
    p = (uchar*)&root;
    for(int i=0; i<sizeof(root); i++) disk[offset+i] = p[i];

    printf("ramdisk: filesystem formatted.\n");
}