#include "io/io.h"
#include "disk.h"
#include "status.h"
#include "memory/memory.h"
#include "../config.h"
//represents the real primary hard disk
struct disk disk;

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

/** Initialize the disk */
void disk_search_and_init()
{
    memset(&disk , 0 ,sizeof(disk));
    disk.type = BINGOS_DISK_TYPE_REAL;
    disk.sector_size = BINGOS_SECTOR_SIZE;

}

struct disk* disk_get (int index)
{
    if(index!= 0)return 0 ;
    return &disk;
}

int disk_read_block(struct disk* idisk , unsigned int lba , int total , void* buf)
{
    if(idisk != &disk)
    {
        return -EIO;
    }
    return disk_read_sector(lba , total , buf);

}