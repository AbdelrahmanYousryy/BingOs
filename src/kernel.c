#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include"idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
// Pointer to the specific address for writing to screen
uint16_t* video_mem = 0;
// counters to track the current location to print
uint16_t terminal_row =  0;
uint16_t terminal_col =  0;

/**  make the apropriate 2 bytes hex for the character */
uint16_t terminal_make_char (char c , char color)
{
    // reversed due to endiness
    return ( color << 8) | c ;
}

/** display a specific char at a specific location on screen */
void terminal_putchar(int x , int y , char c , char color)
{
    video_mem[(x* VGA_HEIGHT + y)] = terminal_make_char(c,color); 

}

/** Display character without specifying the location */
void terminal_writechar(char c , char color)
{
    // implementing new line
    if(c == '\n')
    {
        terminal_row+=1;
        terminal_col=0;
        return;
    }
    terminal_putchar(terminal_row,terminal_col,c,color);
    terminal_col+=1;
    // checkk whether the row end
    if(terminal_col>=VGA_WIDTH)
    {
        terminal_col=0;
        terminal_row+=1;
    }

}
/** Calculate the length of a string */
size_t strlen(const char* str)
{
    size_t len = 0;
    while(str[len])
    {
        len++;
    }
    return len;
}

/** Print String to Terminal */
void print(const char* str)
{
    for(size_t i = 0 ; i <strlen(str);i++)
    {
        terminal_writechar(str[i],15);
    }
}

/** Clear The Terminal */
void terminal_initialize()
{
    terminal_row =  0;
    terminal_col =  0;
    video_mem = (uint16_t*)(0xB8000);
    for(int y = 0 ; y <VGA_HEIGHT ; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x,y,' ',0);
        }
        
    }

}

static struct paging_4gb_chunck*  kernel_chunk = 0;
void kernel_main()
{
    // initialize terminal
    terminal_initialize();
    
    print("Hello World!\n");
    
    // Initialize the heap
    kheap_init();
    
    //Initialize Interrupt Descriptor Table
    idt_init();

    // Setup paging
    kernel_chunk=paging_new_4gb(PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT);

    // switch to kernel  paging chunck
    paging_switch(paging_4gb_chunck_get_directory(kernel_chunk));

    
    //print(ptr);
    // Enable Paging
    enable_paging();
    char buf[512];
    disk_read_sector(0 , 1 , buf );


    // Enable The system Interrupts
    enable_interrupts();
  

}

