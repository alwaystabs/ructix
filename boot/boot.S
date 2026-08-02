.section .rodata
panic_msg:
    .asciz "[   PANIC!!!   ] "
.section .text.init
.globl _start
.extern tick_count
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
.globl trap_handler
trap_handler:
    addi sp, sp, -256
    sd ra, 0(sp)
    sd t0, 8(sp)
    sd t1, 16(sp)
    sd t2, 24(sp)
    sd s0, 32(sp)
    sd s1, 40(sp)
    sd a0, 48(sp)
    sd a1, 56(sp)
    sd a2, 64(sp)
    sd a3, 72(sp)
    sd a4, 80(sp)
    sd a5, 88(sp)
    sd a6, 96(sp)
    sd a7, 104(sp)
    sd s2, 112(sp)
    sd s3, 120(sp)
    sd s4, 128(sp)
    sd s5, 136(sp)
    sd s6, 144(sp)
    sd s7, 152(sp)
    sd s8, 160(sp)
    sd s9, 168(sp)
    sd s10, 176(sp)
    sd s11, 184(sp)
    sd t3, 192(sp)
    sd t4, 200(sp)
    sd t5, 208(sp)
    sd t6, 216(sp)
    csrr t0, mcause
    li t1, 0x8000000000000007
    bne t0, t1, 1f
    la t0, tick_count
    ld t1, 0(t0)
    addi t1, t1, 1
    sd t1, 0(t0)
    li t0, 0x0200BFF8
    ld t1, 0(t0)
    li t2, 416666
    add t1, t1, t2
    li t0, 0x02004000
    sd t1, 0(t0)
    j 2f
1:
    wfi
    j 1b
2:
    ld ra, 0(sp)
    ld t0, 8(sp)
    ld t1, 16(sp)
    ld t2, 24(sp)
    ld s0, 32(sp)
    ld s1, 40(sp)
    ld a0, 48(sp)
    ld a1, 56(sp)
    ld a2, 64(sp)
    ld a3, 72(sp)
    ld a4, 80(sp)
    ld a5, 88(sp)
    ld a6, 96(sp)
    ld a7, 104(sp)
    ld s2, 112(sp)
    ld s3, 120(sp)
    ld s4, 128(sp)
    ld s5, 136(sp)
    ld s6, 144(sp)
    ld s7, 152(sp)
    ld s8, 160(sp)
    ld s9, 168(sp)
    ld s10, 176(sp)
    ld s11, 184(sp)
    ld t3, 192(sp)
    ld t4, 200(sp)
    ld t5, 208(sp)
    ld t6, 216(sp)
    addi sp, sp, 256
    mret
.globl panic
panic:
    csrw mie, zero
    csrci mstatus, 0x8
    li t1, 0x10000000
    
    mv s0, a0
    la a0, panic_msg
    
panic_loop:
    lb t0, 0(a0)
    beqz t0, print_arg
    sb t0, 0(t1)
    li t2, 1000
1:
    addi t2, t2, -1
    bnez t2, 1b
    addi a0, a0, 1
    j panic_loop

print_arg:
    mv a0, s0
    
panic_loop2:
    lb t0, 0(a0)
    beqz t0, halt
    sb t0, 0(t1)
    li t2, 1000
1:
    addi t2, t2, -1
    bnez t2, 1b
    addi a0, a0, 1
    j panic_loop2

halt:
    wfi
    j halt