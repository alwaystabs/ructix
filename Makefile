CC      = riscv64-elf-gcc
LD      = riscv64-elf-ld
OBJCOPY = riscv64-elf-objcopy
QEMU    = qemu-system-riscv64

DEBUG ?= 0

BUILD_GOALS := all elf test

ifneq ($(filter $(MAKECMDGOALS),$(BUILD_GOALS))$(if $(MAKECMDGOALS),,default),)
    ifeq ($(origin DEBUG), command line)
        $(info ---> [BUILD MODE] $(if $(filter 1,$(DEBUG)),Debug,Release) (forced))
    else
        ifneq ($(shell test -t 0 && echo yes),)
            RESPONSE := $(shell read -p "Build with DEBUG infrastructure? (y/N): " choice && echo $$choice)
            ifneq ($(filter $(RESPONSE),y Y yes YES),)
                DEBUG := 1
                $(info ---> [BUILD INFO] DEBUG build (panic/fault injection INCLUDED))
            else
                $(info ---> [BUILD INFO] Basic build)
            endif
        else
            $(info ---> [BUILD INFO] $(if $(filter 1,$(DEBUG)),Debug,Release) (non-interactive))
        endif
    endif
endif

INCLUDES = -Ikernel/include
CFLAGS   = -march=rv64g -mabi=lp64 -mcmodel=medany -nostdlib -ffreestanding -Wall -Wextra $(INCLUDES)
ASFLAGS  = $(CFLAGS) -x assembler-with-cpp
LDFLAGS  = -T linker.ld -no-pie

ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

BUILD_DIR  = build
TEMP_DIR   = temp
KERNEL_DIR = kernel
ASM_DIR    = kernel/asm

C_SRCS   = $(wildcard $(KERNEL_DIR)/*.c) $(wildcard $(KERNEL_DIR)/secret/*.c)
ASM_SRCS = $(wildcard $(ASM_DIR)/*.S)

OBJS     = $(patsubst %.c,$(TEMP_DIR)/%.o,$(notdir $(C_SRCS))) \
           $(patsubst %.S,$(TEMP_DIR)/%.o,$(notdir $(ASM_SRCS)))

TARGET_ELF = $(BUILD_DIR)/ructix.elf
TARGET_BIN = $(BUILD_DIR)/ructix.bin

all: $(TARGET_ELF) $(TARGET_BIN)

elf: $(TARGET_ELF)

$(BUILD_DIR) $(TEMP_DIR):
	@mkdir -p $@

$(TARGET_ELF): $(OBJS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TEMP_DIR)/%.o: $(ASM_DIR)/%.S | $(TEMP_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@

$(TEMP_DIR)/%.o: $(KERNEL_DIR)/%.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMP_DIR)/%.o: $(KERNEL_DIR)/secret/%.c | $(TEMP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET_ELF)
	$(QEMU) -machine virt -bios none -kernel $(TARGET_ELF) -nographic \
		-d cpu_reset,int,guest_errors,unimp -D qemu.log -serial mon:stdio

clean:
	rm -rf $(TEMP_DIR) $(BUILD_DIR)

test: clean all run

archive:
	tar -czf ructix-autorel-$$(date +%d-%m-%Y).tar.gz $(BUILD_DIR)

code:
	tar -czf ructix-source-$$(date +%d-%m-%Y).tar.gz $(KERNEL_DIR)

.PHONY: all elf run clean test archive