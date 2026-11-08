#include "../include/ansi.h"
#include "../include/uart.h"

.section .rodata
panic_msg_prefix:
    .asciz ANSI_BOLD ANSI_BG_RED "[    PANIC!!!    ] "
panic_msg_suffix:
    .asciz "\n"
ansi_reset:
    .byte 0x1b, '[', '0', 'm', 0
divider:
    .asciz ANSI_GRAY "============================\n" ANSI_RESET
emergency_label:
    .asciz ANSI_BRIGHT_RED ANSI_BOLD "\n[!!!] EMERGENCY STACK ACTIVATED \n" ANSI_RESET
emergency_reason:
    .asciz "Reason: "
emergency_sp:
    .asciz "SP:     0x"
emergency_mcause:
    .asciz "mcause: 0x"
emergency_mepc:
    .asciz "mepc:   0x"
emergency_mtval:
    .asciz "mtval:  0x"
emergency_newline:
    .asciz "\n"
msg_stack_overflow:
    .asciz "STACK OVERFLOW DETECTED!"
msg_unknown_panic:
    .asciz "GENERIC PANIC"

.section .text
.extern _stack_bottom
.extern _stack_top
.extern print_hex

.globl panic
panic:
    csrw mie, zero
    csrci mstatus, 0x8

    bnez a0, 1f
    la a0, msg_unknown_panic
1:
    mv s0, a0

    la a0, panic_msg_suffix
    call uart_print_string

    la a0, divider
    call uart_print_string

    la a0, panic_msg_prefix
    call uart_print_string

    mv a0, s0
    call uart_print_string

    la a0, ansi_reset
    call uart_print_string

    la a0, panic_msg_suffix
    call uart_print_string

    call print_trap_csrs

halt:
    wfi
    j halt

.globl switch_to_emergency
switch_to_emergency:
    csrw mie, zero
    csrci mstatus, 0x8

    # Atomic stack switch (mscratch contains emergency stack top)
    csrrw sp, mscratch, sp
    csrr s0, mscratch
    mv s1, a0

    la a0, emergency_label
    call uart_print_string

    # Check sp bounds against stack limits
    la t0, _stack_bottom
    la t1, _stack_top
    bltu s0, t0, 1f
    bgeu s0, t1, 1f

    bnez s1, 2f
    la s1, msg_unknown_panic
    j 2f

1:
    la s1, msg_stack_overflow

2:
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

    call print_trap_csrs

    li t0, 100000
delay_loop:
    addi t0, t0, -1
    bnez t0, delay_loop

    mv a0, s1
    call panic

print_trap_csrs:
    addi sp, sp, -16
    sd ra, 0(sp)

    la a0, emergency_mcause
    call uart_print_string
    csrr a0, mcause
    call print_hex
    la a0, emergency_newline
    call uart_print_string

    la a0, emergency_mepc
    call uart_print_string
    csrr a0, mepc
    call print_hex
    la a0, emergency_newline
    call uart_print_string

    la a0, emergency_mtval
    call uart_print_string
    csrr a0, mtval
    call print_hex
    la a0, emergency_newline
    call uart_print_string

    ld ra, 0(sp)
    addi sp, sp, 16
    ret

.globl uart_print_string
uart_print_string:
    li t1, UART_BASE

1:
    lb t0, 0(a0)
    beqz t0, 2f
    sb t0, 0(t1)
    addi a0, a0, 1
    j 1b

2:
    ret
