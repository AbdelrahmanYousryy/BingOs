#include "io/io.h"
/** Read `total` sectors from Logical Block Address `lba` to buffer `buf` */
int disk_read_sector(int lba , int total , void* buf)
{
    // or the highest 8 bits of the lba with 0xE0 to select the master drive , then send the
    // value to port 0x1F6
    outb(0x1F6,(lba>>24) | 0xE0);

    // output total sectors to read to port 0x1F2
    outb(0x1F2 , total);

    // send more bits of the lba
    outb(0x1F3,(unsigned char)(lba & 0xff));
    outb(0x1F4,(unsigned char)(lba >> 8));
    outb(0x1F5 , (unsigned char)(lba >> 16));
    // send the read command
    outb(0x1F7 , 0x20);
    
    // cast the buffer to 16 bits pointer to read to bytes at a time
    unsigned short* ptr = (unsigned short*)buf;

    for(int i = 0 ; i < total ; i++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory one sector
        for(int b = 0 ; b < 256 ; b++)
        {
            // read from the specifed port one word 
            *ptr = insw(0x1F0);
             ptr++;

        } 
    
    }



    return 0 ;

}