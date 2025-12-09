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
	kernel/swtch.S \
	kernel/console.c \
	kernel/printf.c \
	kernel/uart.c \
	kernel/pmm.c \
	kernel/vmm.c \
	kernel/test.c \
	kernel/trap.c \
	kernel/proc.c \
	kernel/syscall.c \
	kernel/sysproc.c \
	kernel/sysfile.c \
	kernel/ramdisk.c \
    kernel/bio.c \
    kernel/fs.c \
    kernel/file.c \
	kernel/start.c

# 定义 build 目录
BUILD_DIR = build

# 目标文件列表 (带 build/ 前缀)
# 将 .c 替换为 .o，将 .S 替换为 .o
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
OBJS := $(OBJS:%.S=$(BUILD_DIR)/%.o)

# 默认目标
all: $(BUILD_DIR)/kernel.elf

# 创建 build 目录结构
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/kernel

# 编译 C 文件到 build 目录
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# 编译汇编文件到 build 目录
$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# 链接生成 kernel.elf 到 build 目录
# --- [关键修改] ---
# 这里将 $^ 改为了 $(OBJS)
# 因为 $^ 会包含 kernel/kernel.ld，导致链接器报错说脚本出现了两次
$(BUILD_DIR)/kernel.elf: $(OBJS) kernel/kernel.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -T kernel/kernel.ld -o $@ $(OBJS)

# 运行 QEMU
qemu: $(BUILD_DIR)/kernel.elf
	$(QEMU) -machine virt -bios default -kernel $< -m 128M -smp 1 -nographic

# 调试模式
qemu-gdb: $(BUILD_DIR)/kernel.elf
	$(QEMU) -machine virt -bios default -kernel $< -m 128M -smp 1 -nographic -S -gdb tcp::1234

# 清理 build 目录
clean:
	rm -rf $(BUILD_DIR)