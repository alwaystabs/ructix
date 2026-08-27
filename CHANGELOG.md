# Changelog

## [v0.0.1] - 2026-07-28 - INIT.

### Added
- Initial working kernel with UART output.
- Timer interrupts based on `MTIME` / `MTIMECMP`.
- `panic()` function with custom message support.
- `make rebuild` — clean, compile, and run in QEMU.
- `README.md` with project description and build instructions.
- GitHub release with `kernel.elf` and `kernel.bin` files.

### Fixed
- UART output reliability by adding a small delay between characters.
- `panic()` now correctly displays the provided error message from C.

### The story has begun.

---
## [v0.0.5] - 2026-07-30 - Refactoring and else

### Changed
- **Kernel code split into modules**:
  - UART functions moved to `kernel/uart.c`
  - Panic moved to `kernel/panic.c`
  - String/itoa helpers moved to `kernel/string.c`
  - Headers placed in `kernel/include/`
- **Makefile restructured**:
  - Separate build directories: `build/` for ELF/BIN, `temp/` for object files
  - Added `make release` target for creating archives
  - Improved rules for module compilation

### Fixed
- Removed obsolete root files (`boot.S`, `main.c`) after module split

### Added
- `strlen()` function for string length calculation
- `CHANGELOG.md` file for tracking project history
---
## [v0.0.6] - 2026-08-01 - Allocator & Improvements

### Added
- **[!] Bump memory allocator (`allocator.c`, `memory.h`) with `kmalloc_init()`, `kmalloc()`, and stub `kfree()`**
- Panic header (`panic.h`) to declare `panic()` function
- Boot-time banner and structured logging in `kmain()`
- Tick counter test (prints 0–20 ticks)
- Memory allocation test with aligned sizes (8, 16, 32 bytes) and address output
- Out-of-memory panic test (attempt to allocate > heap size)

### Changed
- `boot.S`: panic message now includes visual separator `[   !!!   ]\nPANIC!\n`
- `linker.ld`: added explicit `PHDRS` (text, rodata, data, bss) with proper flags (R-X, R--, RW-)
- `string.c`: `reverse()` now computes length internally via `strlen()` instead of taking a `len` parameter
- `string.h`: updated `reverse()` prototype to match new implementation
- `main.c`: replaced simple tick output with extended test suite (boot log, tick loop, allocator tests, panic trigger)

### Fixed
- Many minor compilation fixes
---
## [v0.0.65] - 2026-08-02 - Improvement Patch

### Changed
- **uart_puts() is now print()**
- Panic message was changed a little: `[   PANIC!!!   ]` instead of `[   !!!   ]\nPANIC!\n`

### Fixed
- `panic()` now correctly shows warning message and panic reason
- `make archive` command was fixed

### Deleted
- `make release` - for further improvements
---
## [v0.0.7] - 2026-08-07 - Major Update: UART Input & Minor Improvements

### Added
- **[!] UART Input: `uart_getchar()` implemented with polling and LSR (Line Status Register) check**
- **Control character handling** — Enter (`\r`) now starts a new line (`\r\n`), Backspace (`\b`) erases the previous character
- **kmalloc debug logging** — allocation calls now print the requested size and the actual aligned size allocated (useful for understanding memory alignment)
- **Global buffer `buf`** — declared `char buf[32]` as a global variable in `init.c` (was previously stack-local in `kmain`)
- **Code documentation** — added detailed comments to many code sections, especially for interrupt setup

### Changed
- **Refactored kernel initialization** — moved boot-time tests (tick test, memory allocation test) from `kmain()` into a dedicated function `init_check()` for better code organization
- **File renamed** — `main.c` → `init.c` (the content of `kmain` was split; the new file contains both `init_check()` and `kmain()`)
- **`uart_putchar` parameter** — changed to `const char` for better const-correctness

### Fixed
- **`kfree()` and `free()` conflict** — resolved duplicate definition in `memory.h`
---
## [v0.1.0] - 2026-08-12 - The Shell Rises - Grand Update

### Added
- **[!] Command shell:** Interactive shell with command prompt (`ructix>`).
- **[!] Command parser:** Table-driven command dispatch using `command_t` struct.
- **Built-in commands:**
  - `help` — displays available commands.
  - `status` — shows system status (tick count).
  - `panic` — triggers a kernel panic.
- **Modular structure:** Shell logic separated into `shell.c` with its own header.
- **Static buffers:** Command buffer (`cmd_buf`) is now static to prevent conflicts.

### Changed
- **Boot flow:** Replaced the passive tick loop in `kmain` with an interactive command loop (`cmd_loop()`).
- **File organization:** `main.c` renamed to `init.c`; custom `string.h` renamed to `kstring.h` to avoid conflicts with system headers.

### Fixed
- **Command handling:** Fixed buffer overflow protection and backspace handling.
- **Build system:** Added `shell.o` and `panic.o` to Makefile.

## [v0.1.5] - 2026-08-14 - Improvement Patch

### Added
- **ANSI color support:** Boot log now shows version in green, build info in gray.
- **Prompt protection:** `print_prompt()` function with `\033[K` (clear line) to prevent accidental erasure.
- **Something lives here from now on.**

### Changed
- **Assembly refactor:** Moved `boot.S` to `asm/` and split into `init.S`, `panic.S`, `trap_handler.S`.
- **Build system:** Updated Makefile to compile new assembly files (`panic.o`, `trap_handler.o`).
- **Shell:**
  - `shell_loop()` now uses `print_prompt()`.
  - Unknown command output now shows the exact command: `Unknown command 'xyz'`.
  - Fixed backspace indentation and prompt restoration.
- **Memory:** Added `#include <stdint.h>` and `panic.h` to `allocator.c`.

### Fixed
- **Prompt erasure:** Backspace can no longer delete the `ructix>` prompt.
- **Makefile paths:** Fixed assembly file dependencies.
- **Boot message:** Updated version string to `v0.1.0`.

### Removed
- **Old `boot.S`:** Deleted monolithic file (split into three logical files).

## [v0.2.0] - 2026-08-27 - Major Security & Optimization Update

### Added
- **Emergency stack** — 1 KB dedicated stack in BSS for fatal trap handling, prevents double faults.
- **`fatal_trap_panic`** — assembly wrapper that routes severe exceptions onto the emergency stack and calls C handler.
- **`unhandled_trap_c`** — C handler for fatal traps with register decoding and panic.
- **`print_hex(uint64_t val)`** — print 64-bit values in hexadecimal format (great for debugging).
- **Safe string functions** — `strnlen()` and `strncmp()` with length limits.
- **DEBUG build mode** — optional `make DEBUG=1` with fault injection commands (`panic 1`, `panic 2`, `panic 3`).
- **Debug triggers**:
  - `panic 1` — illegal instruction (`unimp`)
  - `panic 2` — load access fault (read from `0x0`)
  - `panic 3` — stack overflow (recursive function)
- **Automatic source discovery** — Makefile now collects all `.c` and `.S` files recursively.

### Changed
- **Linker script restructured:** stacks and heap now inside `.bss` with explicit 16‑byte alignment.
- **`mtvec` now set in C** via `csrw mtvec, %0` instead of assembly startup.
- **Trap handler split:** timer interrupts handled normally, fatal traps jump to emergency stack.
- **`volatile` qualifiers** added to shell state buffers to prevent compiler optimizations during UART interrupts.
- **Makefile updated** — interactive prompt for DEBUG mode, cleaner pattern rules.

### Fixed
- **`strlen` prototype mismatch** — now returns `size_t`.
- **Off-by-one bug** in shell command length check (`cmd_index < 255`, not `<=`).
- **Global buffer conflicts** — `kernel_buf` and `alloc_buf` separated.
- **Invalid pointer dereference** in shell (`*cmd` → `*str_cmd` with proper casting).
- **UART output reliability** — added `uart_puts` for raw string output.
