OUTPUT_ARCH(riscv)
ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);   /* R-X */
    rodata PT_LOAD FLAGS(4); /* R-- */
    data PT_LOAD FLAGS(6);   /* RW- */
    bss PT_LOAD FLAGS(6);    /* RW- */
}

SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text.init)
        *(.text*)
    } :text

    . = ALIGN(16);
    .rodata : {
        *(.rodata*)
    } :rodata

    . = ALIGN(16);
    .data : {
        *(.data*)
    } :data

    . = ALIGN(16);
    .bss : {
        *(.bss*)
        *(COMMON)

        /* Main stack */
        . = ALIGN(16);
        _stack_bottom = .;
        . += 0x1000;
        _stack_top = .;
        
        /* Emergency stack */
        . = ALIGN(16);
        _emergency_stack_bottom = .; 
        . += 1024;
        _emergency_stack_top = .;
        
        /* Heap start. 8 MB for now */
        . = ALIGN(16);
        _heap_start = .;
        . += 0x800000;
        _heap_end = .;
    } :bss

    . = ALIGN(16);
    _kernel_end = .;
}