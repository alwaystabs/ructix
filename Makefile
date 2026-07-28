
CC      = riscv64-elf-gcc
LD      = riscv64-elf-ld
OBJCOPY = riscv64-elf-objcopy
QEMU    = qemu-system-riscv64

CFLAGS  = -march=rv64g -mabi=lp64 -mcmodel=medany -nostdlib -ffreestanding -Wall -Wextra
LDFLAGS = -T linker.ld -no-pie

OBJS = boot.o main.o
TARGET = kernel.elf

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf -nographic -d cpu_reset,int,guest_errors,unimp -D qemu.log


clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all run

.PHONY: all run clean rebuild