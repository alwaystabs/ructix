OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text.init)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    _stack_bottom = .;
    . += 0x1000;
    _stack_top = .;
    _heap_start = .;
    _heap_end = _heap_start + 0x800000;
}