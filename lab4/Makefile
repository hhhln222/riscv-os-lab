# 工具链前缀
TOOLPREFIX = riscv64-unknown-elf-

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
QEMU = qemu-system-riscv64

# 编译选项
# -mcmodel=medany: 允许代码在任意地址链接（只要在正负2GB范围内）
# -ffreestanding: 裸机环境，没有标准库
# -nostdlib: 不链接标准库
CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I. -Iinclude

# 链接选项
LDFLAGS = -z max-page-size=4096 --no-warn-rwx-segments

# 源文件列表
SRCS = \
	kernel/entry.S \
	kernel/kernelvec.S \
	kernel/console.c \
	kernel/printf.c \
	kernel/uart.c \
	kernel/pmm.c \
	kernel/vmm.c \
	kernel/test.c \
	kernel/trap.c \
	kernel/start.c

# 目标文件列表
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

# 默认目标
all: kernel.elf

# 编译规则
kernel.elf: $(OBJS) kernel/kernel.ld
	$(LD) $(LDFLAGS) -T kernel/kernel.ld -o kernel.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

# 运行 QEMU
qemu: kernel.elf
	$(QEMU) -machine virt -bios default -kernel kernel.elf -m 128M -smp 1 -nographic

# 调试模式
qemu-gdb: kernel.elf
	$(QEMU) -machine virt -bios default -kernel kernel.elf -m 128M -smp 1 -nographic -S -gdb tcp::1234

clean:
	rm -f *.o kernel/*.o kernel.elf