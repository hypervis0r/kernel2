/* Tell the linker that we want the symbol _start to be our entry point */
ENTRY(KeMain)
 
/* Define the program headers we want */
PHDRS
{
    null    PT_NULL    FLAGS(0) ;                   /* Null segment */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}
 
SECTIONS
{
    /* We wanna be placed in the higher half */
    . = 0xC000000000000000;
 
    /* Then let's place all the other traditional executable sections afterwards. */
    .rodata ALIGN(4K) : {
        *(.rodata*)
    } :rodata
 
    .text ALIGN(4K) : {
        *(.text*)
    } :text
 
    .data ALIGN(4K) : {
        *(.data*)
    } :data
 
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss*)
    } :data
}
