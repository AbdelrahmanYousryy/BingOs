ENTRY(_start) /*starting point of the code */
OUTPUT_FORMAT(binary) /* Output Format */

SECTIONS
{
    . = 1M; /* Kernel to be loaded in 1MB memory address*/
    .text : ALIGN(4096)/* Define the text section */
    {
        *(.text)
    }
    /* Define The Read only section */
    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }
    /* Define The Initialized Data section */
    .data : ALIGN(4096)
    {
        *(.data)
    }
    /* Define The Uninitialized Data section */
    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)

    }
    /* Define The Assembly Section */
    .asm : ALIGN(4096)
    {
        *(.asm)
    }
}