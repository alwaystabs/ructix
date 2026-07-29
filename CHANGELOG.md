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
