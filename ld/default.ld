/* deault link scripts */

USER_STACK_ADDR = ORIGIN(SRAM) + LENGTH(SRAM) - USER_STACK_SIZE;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = . ;
        KEEP(*(.isr_vector))
        _eisr_vector = . ;
    } > FLASH
 
    .signature :
    {
        KEEP(*(.mcush_signature))
        KEEP(*(.signature))
    } > FLASH

    .init_script :
    {
        _isdata = . ;
        KEEP(*(.init_script))
        BYTE(0);
    } > FLASH

        __exidx_start = . ;
        .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
        __exidx_end = .;

   .text :
    {
        . = ALIGN(4);
        _stext = . ;
        *(.text*)
        /**(.init)*/
        *(.rodata*)
        . = ALIGN(4);
        _etext = . ;
    } > FLASH

    _sidata = . ;

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = . ;
        *(.data*)
        . = ALIGN(4);
        _edata = . ;
    } > SRAM 

    .bss :
    {
        . = ALIGN(4);
        _sbss = . ;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } > SRAM
    
    .dma :
    {
        . = ALIGN(4);
        *(.dma)
        . = ALIGN(4);
    } > SRAM
        

    .userheap :
    {
        . = ALIGN(4);
        _sheap = . ;
        *(.userheap)
        . = . + USER_HEAP_SIZE ;
        . = ALIGN(4);
        _eheap = . ;
    } > SRAM

    .userstack (USER_STACK_ADDR) :
    {
        . = ALIGN(4);
        _sstack = . ;
        . = . + USER_STACK_SIZE ;
        . = ALIGN(4);
        _estack = . ;
    } > SRAM 

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
        *(.ARM.exidx)
        *(.glue*)
        *(.init)
        *(.fini)
    }

}


ASSERT( _sidata + SIZEOF(.data) <= ORIGIN(FLASH) + LENGTH(FLASH), "flash full, fail to append data image" )
ASSERT( _eheap <= _sstack, "heap size too large" )

