#ifndef DISK_H
#define DISK_H

typedef unsigned int BINGOS_DISK_TYPE;

// represents a real physical hard disk
#define BINGOS_DISK_TYPE_REAL 0

struct disk
{
    BINGOS_DISK_TYPE type;
    int sector_size;

};

void disk_search_and_init();
struct disk* disk_get (int index);
/** Read form Disk `disk` in LBA `lba` `total` sectors in buffer `buf` */
int disk_read_block(struct disk* idisk , unsigned int lba , int total , void* buf);
#endif