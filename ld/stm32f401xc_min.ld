/* STM32F401xC, 256kB FLASH, 64kB SRAM */

USER_HEAP_SIZE = 30K;
USER_STACK_SIZE = 2K;
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 256K
    SRAM (rw)    : ORIGIN = 0x20000000, LENGTH = 64K 
}

INCLUDE default.ld


