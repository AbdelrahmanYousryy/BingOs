#ifndef DISK_H
#define DISK_H

#include "fs/file.h"
typedef unsigned int BINGOS_DISK_TYPE;

// represents a real physical hard disk
#define BINGOS_DISK_TYPE_REAL 0

/* Representation of the disk*/
struct disk
{
    // id of the disk can be used for better implementation to handle multple disks
    int id;
    // type of the disk
    BINGOS_DISK_TYPE type;
    // sector size of the disk
    int sector_size;
    // filesystem binded to this disk
    struct filesystem* filesystem;

    // private data of our filesystem
    void* fs_private;

};

void disk_search_and_init();
struct disk* disk_get (int index);
/** Read form Disk `disk` in LBA `lba` `total` sectors in buffer `buf` */
int disk_read_block(struct disk* idisk , unsigned int lba , int total , void* buf);
#endif