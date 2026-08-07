
CC      = riscv64-elf-gcc
LD      = riscv64-elf-ld
OBJCOPY = riscv64-elf-objcopy
QEMU    = qemu-system-riscv64

CFLAGS  = -march=rv64g -mabi=lp64 -mcmodel=medany -nostdlib -ffreestanding -Wall -Wextra
LDFLAGS = -T linker.ld -no-pie

OBJS = $(TEMP_DIR)/boot.o $(TEMP_DIR)/main.o $(TEMP_DIR)/uart.o $(TEMP_DIR)/string.o $(TEMP_DIR)/memory.o

BUILD_DIR = build
TEMP_DIR = temp
KERNEL_DIR = kernel

TARGET = $(TARGET_ELF)
TARGET_ELF = $(BUILD_DIR)/kernel.elf
TARGET_BIN = $(BUILD_DIR)/kernel.bin


all: $(TARGET_ELF) $(TARGET_BIN)
elf: $(TARGET_ELF)

$(BUILD_DIR) $(TEMP_DIR):
	mkdir -p $@

$(TARGET_ELF): $(OBJS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TEMP_DIR)/boot.o: boot/boot.S | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMP_DIR)/main.o: kernel/main.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMP_DIR)/uart.o: kernel/uart.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMP_DIR)/string.o: kernel/string.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMP_DIR)/memory.o: kernel/allocator.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET_ELF)
	qemu-system-riscv64 -machine virt -bios none -kernel $(TARGET_ELF) -nographic -d cpu_reset,int,guest_errors,unimp -D qemu.log

clean:
	rm -rf $(TEMP_DIR) $(BUILD_DIR)

test: clean all run

archive:
	tar -czf ructix-autorel-$$(date +%d-%m-%Y).tar.gz build

.PHONY: all run clean test release archive