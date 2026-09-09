#include "../include/ansi.h"
.section .rodata
panic_msg_prefix:
    .asciz ANSI_BOLD ANSI_BG_RED "[   PANIC!!!   ] "
panic_msg_suffix:
    .asciz "\n"
ansi_reset:
    .byte 0x1b, '[', '0', 'm', 0
divider:
    .asciz ANSI_GRAY "============================"
emergency_label:
    .asciz ANSI_BRIGHT_RED ANSI_BOLD "\n[!!!] EMERGENCY STACK ACTIVATED \n" ANSI_RESET
emergency_reason:
    .asciz "Reason: "
emergency_sp:
    .asciz "SP: 0x"
emergency_newline:
    .asciz "\n"
panic_msg:
    .asciz "Stack overflow!"
hardcoded_msg:
    .asciz "STACK OVERFLOW DETECTED!"
.section .text
.globl panic
panic:
    csrw mie, zero
    csrci mstatus, 0x8

    mv s0, a0

    la a0, panic_msg_suffix
    call uart_print_string
    
    la a0, divider
    call uart_print_string

    la a0, panic_msg_suffix     # No need to create another \n alias
    call uart_print_string

    la a0, ansi_reset
    call uart_print_string


    la a0, panic_msg_prefix
    call uart_print_string

    mv a0, s0
    call uart_print_string

    la a0, ansi_reset
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

.globl switch_to_emergency
switch_to_emergency:
    csrw mie, zero
    csrci mstatus, 0x8

    mv s0, sp
    mv s1, a0

    la sp, _emergency_stack_top

    la a0, emergency_label
    call uart_print_string

    la a0, emergency_reason
    call uart_print_string
    mv a0, s1
    call uart_print_string
    la a0, emergency_newline
    call uart_print_string

    la a0, emergency_sp
    call uart_print_string
    mv a0, s0
    call print_hex
    la a0, emergency_newline
    call uart_print_string

    li t0, 100000
    1:
        addi t0, t0, -1
        bnez t0, 1b

    la a0, hardcoded_msg
    call panic