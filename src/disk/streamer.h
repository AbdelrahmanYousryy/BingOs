/**
 * Description : Disk Streamer That allows us to read bytes smaller than a sector
 */
#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "disk.h"

struct disk_stream
{
    int pos ; // current position to read in the disk `disk` (byte position)
    struct disk* disk;
};

struct disk_stream* diskstream_new (int disk_id);
int disksteram_seek(struct disk_stream* stream ,int pos);
int diskstream_read(struct disk_stream* stream , void* out , int total);
void diskstream_close(struct disk_stream* stream);

#endif