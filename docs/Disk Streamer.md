# Disk Streamer Implementation

## Overview  
This document summarizes the progress made in Implementing the filesystem in our kernel.  

### Functionality 
A Disk Streamer allows us to retrieve and stream data from disk and it should contain
- seeking functionality
- reading functionality
- close the stream  

### Implementation

**struct disk_stream:**
this is the main struct and the representation of the stream , it contains to elements:
- int position  , This represents the position we are currently in inside the disk.
- struct disk* disk , This represents the disk that this stream is bind to.
    
**struct disk_stream* diskstream_new (int disk_id):**
this is the initialization function of the disk stream and binds the stream to disk with id *disk_id* as follows:
- first it retrieves the disk with id `disk_id`
- checks for validity of the disk
- creates a new diskstream with kzalloc
- binds the newly created diskstream to the retrieved disk
- returns this disk_stream


**int disksteram_seek(struct disk_stream* stream ,int pos):**
Changes the Position of the stream inside the disk by changing `position` inside `disk_stream`

**int diskstream_read(struct disk_stream* stream , void* out , int total):**
this reads a `total` number of bytes from disk stream `stream` into buffer `out` as follows:
- calculate the sector and offset to read, from the positon fo the stream
- initialize the buffer to read data into
- reads one sector to this buffer
- if the `total` number of bytes smaller than the sector size?
    - use a for loop to only read the `total` amount of bytes
- if the `total` number of bytes bigger than the sector size?
    - read only one sector form disk
    - re do the process again with `total` = `total` - `BINGOS_SECTOR_SIZE`
 
**void diskstream_close(struct disk_stream* stream):**
Closes the Stream