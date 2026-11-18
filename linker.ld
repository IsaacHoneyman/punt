OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)

/* Program headers: the loadable chunks Limine maps, with permissions
   (4 = read, 2 = write, 1 = execute) */
PHDRS {
    limine_requests PT_LOAD FLAGS(6);  /* rw-: Limine writes responses */
    text            PT_LOAD FLAGS(5);  /* r-x */
    rodata          PT_LOAD FLAGS(4);  /* r-- */
    data            PT_LOAD FLAGS(6);  /* rw- */
}

SECTIONS {
    /* Top 2 GiB of the address space, which -mcmodel=kernel expects */
    . = 0xffffffff80000000;

    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :limine_requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));
    .text : { *(.text .text.*) } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));
    .rodata : { *(.rodata .rodata.*) } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));
    .data : { *(.data .data.*) } :data

    .bss : { *(.bss .bss.*) *(COMMON) } :data

    /DISCARD/ : { *(.eh_frame*) *(.note .note.*) }
}