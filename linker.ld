/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

MEMORY
{
    TCM (rwx) : ORIGIN = 0x00100000, LENGTH = 192K
    SYSRAM (rwx) : ORIGIN = 0x22000000, LENGTH = 64K
    FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 1M
}

/* The data and BSS regions can be placed in TCM or SYSRAM. The code and read-only regions can
   be placed in TCM, SYSRAM, or FLASH. See
   https://docs.microsoft.com/en-us/azure-sphere/app-development/memory-latency for information
   about which types of memory which are available to real-time capable applications on the
   MT3620, and when they should be used. */
REGION_ALIAS("CODE_REGION", TCM);
REGION_ALIAS("RODATA_REGION", FLASH);
REGION_ALIAS("DATA_REGION", TCM);
REGION_ALIAS("BSS_REGION", TCM);

ENTRY(ExceptionVectorTable)

SECTIONS
{
    /* The exception vector's virtual address must be aligned to a power of two,
       which is determined by its size and set via CODE_REGION.  See definition of
       ExceptionVectorTable in main.c.

       When the code is run from XIP flash, it must be loaded to virtual address
       0x10000000 and be aligned to a 32-byte offset within the ELF file. */
    .text : ALIGN(32) {
        KEEP(*(.vector_table))
        *(.text)
        *(.text*)

        /* These are the C++ global constructors.  Stick them all here and
         * then walk through the array in main() calling them all.
         */
        _init_array_start = .;
        KEEP (*(SORT(.init_array*)))
        _init_array_end = .;

        . = ALIGN(4);
        _etext = .;
    } >CODE_REGION

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >CODE_REGION

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } >CODE_REGION
    __exidx_end = .;

    .rodata : {
        *(.rodata)
        *(.rodata*)
    } >RODATA_REGION

    .data : {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >DATA_REGION

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >BSS_REGION

    /* Add this to satisfy reference to symbol 'end' from libnosys.a(sbrk.o)
     * to denote the HEAP start.
     */
    end = .;

    StackTop = ORIGIN(TCM) + LENGTH(TCM);
}
