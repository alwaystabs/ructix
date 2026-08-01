OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text.init)
        *(.text)
    } :text

    .rodata : {
        *(.rodata)
    } :rodata

    .data : {
        *(.data)
    } :data

    .bss : {
        *(.bss)
    } :bss

    _stack_bottom = .;
    . += 0x1000;
    _stack_top = .;
    _heap_start = .;
    _heap_end = _heap_start + 0x800000;
}

PHDRS
{
    text PT_LOAD FLAGS(5);  /* R-X */
    rodata PT_LOAD FLAGS(4); /* R-- */
    data PT_LOAD FLAGS(6);  /* RW- */
    bss PT_LOAD FLAGS(6);   /* RW- */
}