.section .rodata
panic_msg_prefix:
    .asciz "[   PANIC!!!   ] "
panic_msg_suffix:
    .asciz "\n"
.section .text
.globl panic
panic:
    csrw mie, zero
    csrci mstatus, 0x8

    mv s0, a0

    la a0, panic_msg_prefix
    call uart_print_string

    mv a0, s0
    call uart_print_string

    la a0, panic_msg_suffix
    call uart_print_string
halt:
    wfi
    j halt

uart_print_string:

    li t1, 0x10000000

1:
    lb t0, 0(a0)
    beqz t0, 2f
    sb t0, 0(t1)
    addi a0, a0, 1
    j 1b

2:
    ret