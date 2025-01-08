#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"

/** Creates a new disk stream */
struct disk_stream* diskstream_new (int disk_id)
{
    struct disk* disk = disk_get(disk_id);
    if(!disk)
    {
        return 0 ;
    }

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

/** Reposition the position of the disk stream */
int disksteram_seek(struct disk_stream* stream ,int pos)
{
    stream -> pos = pos;
    return 0 ;
}

/** Read from disk stream `stream` amount of bytes `total` and read it into `out`
 * Returns: 0 if ok, 1 if fails
 */
int diskstream_read(struct disk_stream* stream , void* out , int total)
{
    // get the sector we will read
    int sector = stream->pos / BINGOS_SECTOR_SIZE;
    // get the offset inside the sector we will read
    int offset = stream ->pos % BINGOS_SECTOR_SIZE;
    char buf[BINGOS_SECTOR_SIZE];

    // read one sector into buf
    int res = disk_read_block(stream->disk , sector , 1 , buf);
    if (res < 0 )
    {
        goto out;
    }

    int total_to_read = total > BINGOS_SECTOR_SIZE ? BINGOS_SECTOR_SIZE : total; 
    // copy the bytes we want to read into out
    for(int i = 0 ; i < total_to_read ; i++)
    {
        *(char*)out++ = buf[offset+i];
    }

    // adjust the stream
    stream->pos += total_to_read;

    // if we want to read more than one sector we do this process again 
    if (total > BINGOS_SECTOR_SIZE)
    {
        res = diskstream_read(stream , out , total - BINGOS_SECTOR_SIZE);
    }

out:
    return res;

}

void diskstream_close(struct disk_stream* stream)
{
    kfree(stream);
}