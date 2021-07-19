/* Tell the linker that we want the symbol _start to be our entry point */
ENTRY(KeMain)
 
SECTIONS
{
    /* We wanna be placed in the higher half */
    . = 0x10000000;
 
    /* Then let's place all the other traditional executable sections afterwards. */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }
 
    .text ALIGN(4K) : {
        *(.text)
    }
 
    .data ALIGN(4K) : {
        *(.data)
    }
 
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }
}
