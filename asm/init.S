.section .text.init
.globl _start
.extern kmain
_start:
    la sp, _stack_top
    la t0, trap_handler
    csrw mtvec, t0
    li t0, 0x80
    csrw mie, t0
    csrsi mstatus, 0x8
    call kmain
loop:
    wfi
    j loop
.align 4