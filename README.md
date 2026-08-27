# 🚀 RUCTiX

**A minimal RISC-V kernel built from scratch.**  
Crafted with curiosity, C, and RISC-V on Arch Linux by a 14-year-old developer.

---

## ✨ Features

Ructix has evolved from a simple bootloader into an interactive kernel with a command shell:

### Core System
- **UART I/O** — full-duplex serial communication (`print()` / `uart_getchar()`)
- **Timer interrupts** — periodic ticks using `MTIME` / `MTIMECMP`
- **panic()** — safe kernel halt with error message display
- **Bump memory allocator** (`kmalloc`) — 8-byte alignment with OOM panic and debug logging
- **Structured boot log** — ANSI-colored header with version and build info
- **Explicit memory permissions** in linker script (R-X, R--, RW-)

### String Utilities
- `itoa()` — 64-bit integer to string conversion
- `strlen()` — string length
- `reverse()` — string reversal
- `strcmp()` — string comparison

### 🐚 Interactive Shell
- Command prompt: `ructix>`
- Table-driven command dispatch using `command_t` struct
- **Built-in commands:**
  - `help` — display available commands
  - `status` — show system status (tick count)
  - `panic` — trigger a kernel panic
- **Input handling:**
  - Backspace support with prompt protection
  - Enter (`\r`) starts new line (`\r\n`)
  - Command buffer with overflow protection (255 chars max)
- **Easter egg:** Empty command occasionally triggers `rux_nothing_to_execute()` 😉

---

## 🛠️ Toolchain Setup

### Required tools
- `riscv64-elf-binutils`
- `riscv64-elf-gcc`
- `riscv64-elf-ld`

### For emulation
- `qemu-system-riscv64`
- `qemu-system-riscv-firmware` (if needed)

### Optional but recommended
- `riscv64-elf-gdb` — for debugging and deeper exploration

> **Arch Linux users:**
> ```bash
> sudo pacman -S riscv64-elf-gcc riscv64-elf-binutils qemu-system-riscv
> ```

---

## 📋 Build & Run

### Makefile commands

| Command | Description |
|---------|-------------|
| `make clean` | Remove all object files |
| `make` | Compile `.elf` and create a binary copy (`.bin`) |
| `make elf` | Compile only the `.elf` file |
| `make run` | Run the kernel in QEMU with serial output |
| `make test` | Clean → Build → Run (full cycle) |
| `make archivate` | Archive `.bin` and `.elf` from `/build` |

### Quick start
```bash
git clone https://github.com/alwaystabs/ructix.git
cd ructix
make test
```
## Project Structure
``` ructix/
├── kernel/
│   ├── asm/                    # Assembly files
│   │   ├── init.S             # Boot & _start
│   │   ├── panic.S            # panic() implementation
│   │   └── trap_handler.S     # Interrupt/exception handling
│   ├── include/                # Headers
│   │   ├── kstring.h
│   │   ├── memory.h
│   │   ├── panic.h
│   │   ├── shell.h
│   │   ├── timer.h            # MTIME/MTIMECMP definitions
│   │   └── uart.h
│   ├── secret/                 # Experimental/easter egg code
│   │   └── rux.c / rux.h
│   ├── allocator.c            # Bump memory allocator
│   ├── init.c                 # kmain(), init_check()
│   ├── kstring.c              # String utilities
│   ├── shell.c                # Interactive shell
│   ├── uart.c                 # UART I/O
│   └── unhandled_trap.c       # Default trap handler
├── build/                      # Compiled ELF and binary images
├── temp/                       # Object files (build artifacts)
├── linker.ld                   # Linker script
├── Makefile
└── README.md
```
---
## 🎯 Recent Milestones
### v0.1.5 — Improvement Patch
- ANSI color support — boot log with version in green, build info in gray

- Assembly refactor — `boot.S `split into `init.S`, `panic.S`, `trap_handler.S`

- Prompt protection — backspace can't delete the ructix> prompt

- Better error handling — unknown commands show the exact input

### v0.1.0 — The Shell Rises
- Interactive command shell with `ructix>` prompt

- Command parser with table-driven dispatch

- Built-in commands: `help`, `status`, `panic`

- Modular structure — shell logic in `shell.c`

v0.0.7 — UART Input
- `uart_getchar()` with polling and LSR check

- Backspace and Enter handling

- `kmalloc` debug logging

Code documentation
### ✔️ *You can always download the latest stable version in releases:*
### [Releases](https://github.com/alwaystabs/ructix/releases)
---
### ❗ *Wanna contribute or report a bug?*
### Feel free to open an [**Issue**](https://github.com/alwaystabs/ructix/issues) or a [**Pull Request**](https://github.com/alwaystabs/ructix/pulls)!
---
## 🙏 Special thanks
To the one who had no hands, but had a path — for helping very much build this project from zero to running kernel.
# ❤️ **Building with curiosity, C, and RISC-V.** 