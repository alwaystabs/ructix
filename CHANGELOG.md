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

## [v0.0.65] - 2026-08-02 - Improvement Patch

### Changed
- **uart_puts() is now print()**
- Panic message was changed a little: `[   PANIC!!!   ]` instead of `[   !!!   ]\nPANIC!\n`

### Fixed
- `panic()` now correctly shows warning message and panic reason
- `make archive` command was fixed

### Deleted
- `make release` - for further improvements

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
