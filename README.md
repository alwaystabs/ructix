# 🚀 RUCTiX

### Minimal RISC-V kernel from scratch. Coding on Arch Linux by 14 y.o. coder.
---
### ❓ What can it do?

Ructix is in active development. It has already grown from a simple bootloader into a minimal operating system with:

- **UART input & output** — interactive shell with command prompt (`ructix>`)
- **Command parser** — table-driven dispatch with `help`, `status`, `panic`, `clear`
- **Interactive shell** — command history, backspace handling, ANSI colors
- **Timer interrupts** — periodic ticks based on `MTIME` / `MTIMECMP`
- **panic()** — safe kernel halt (assembly-level, works even with corrupted stack)
- **Bump memory allocator** (`kmalloc`) — with planned upgrade to `free` list allocator
- **Static buffers** — per-module isolation to prevent memory corruption
- **64-bit integer to string conversion** (`itoa`), `strlen`, `reverse`, `strcmp`

---
### 🛠️ Project requires some RISC-V tools:
  - ###### Required:
    - **riscv64-elf-binutils**
    - **riscv64-elf-gcc**
    - **riscv64-elf-ld**
  - ###### Emulation:
    - **qemu-system-riscv**
  - ###### Recommended:
    - **riscv64-elf-gdb** (for debugging and improving Ructix)

---
### 💡 Special `make` commands:
  - **`make clean`** — removes object files and temporary build artifacts
  - **`make`** — builds the kernel ELF and creates a binary copy
  - **`make elf`** — compiles only the ELF file
  - **`make run`** — launches the kernel in QEMU with UART output
  - **`make test`** — `clean`, `make`, and `run` in one command
  - **`make archive`** — packs `.elf` and `.bin` into a release archive

---
### 🔮 Roadmap (what's coming next)

- **v0.2.0** — ELF loader (load and execute programs from the kernel)
- **v0.3.0** — Transition to Supervisor mode (S‑mode) with virtual memory
- **v0.4.0** — Process management and context switching
- **v0.5.0+** — Preemptive multitasking and parallel execution

---
### ✔️ *You can always download the latest stable version in releases:*
### [Releases](https://github.com/alwaystabs/ructix/releases)

---
### ❗ *Wanna contribute or report a bug?*
### Feel free to open an [**Issue**](https://github.com/alwaystabs/ructix/issues) or a [**Pull Request**](https://github.com/alwaystabs/ructix/pulls)!

---
## 🙏 Special thanks
To the one who had no hands, but had a path — for helping very much build this project from zero to running kernel.

# ❤️ **Building with curiosity, C, and RISC‑V.**
