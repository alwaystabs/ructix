# 🚀 RUCTIX

### Minimal RISC-V kernel from scratch. Coding on Arch Linux by 14 y.o. coder.
---
### ❓ What can it do?
  Ructix is in early development, but already includes:

- **UART output** — kernel messages via `uart_puts`
-  **Timer interrupts** — periodic ticks based on `MTIME` / `MTIMECMP`
-  **panic()** — safe kernel halt
- **Basic heap region** — allocated via linker script (no allocator yet, but planned)
---
### 🛠️ Project requires some RISC-V tools:
  - ###### Required:
    - **riscv64-elf-binutils**
    - **riscv64-elf-gcc**
    - **riscv64-elf-ld**
  - ###### Emulation:
    - **qemu-system-riscv**
    - **qemu-system-riscv-firmware**
  - ###### Recommended:
    - **riscv64-elf-gdb (if you are interested in debugging and improving Ructix)**
  ---
  ### 💡 Special `make` commands:
  - **`make clean`** - erases .o files.
  - **`make`** will compile .elf and create a binary copy of it.
  - **`make elf`** will only compile .elf file.
  - **`make run`** emulates compiled .elf file with QEMU.
  - **`make test`** does everything: `make clean`, `make`, `make run`.
  - **`make archivate`** makes an archive with .bin and .elf images of your latest compilation. Be careful, won't work if you don't have any files in /build!
---
---
### ✔️ *You can always download the latest stable version in releases:*
### [Releases](https://github.com/alwaystabs/ructix/releases)
---
### ❗ *Wanna contribute or report a bug?*
### Feel free to open an [**Issue**](https://github.com/alwaystabs/ructix/issues) or a [**Pull Request**](https://github.com/alwaystabs/ructix/pulls)!
---
# ❤️ **Building with curiosity, C, and RISC-V.** 
