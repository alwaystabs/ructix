.section .text.init
.globl _start
.extern kmain
.align 4
_start:
    la sp, _stack_top
    la t0, trap_entry
    csrw mtvec, t0
    li t0, 0x80
    csrw mie, t0
    csrsi mstatus, 0x8
    call kmain
loop:
    wfi
    j loop