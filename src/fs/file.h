#ifndef FILE_H
#define FILE_H
#include "pparser.h"
#include <stdint.h>
typedef unsigned int FILE_SEEK_MODE;
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};


// Define File mode so each file can be opened in one file mode
typedef unsigned int FILE_MODE;
enum{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};
struct disk;


// each filesystem will declare this pointers to its internal functions
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk , struct path_part* path , FILE_MODE mode);
// nmemb: how many blocks needed to be read
typedef int (*FS_READ_FUNCTION)(struct disk* disk , void* private , uint32_t size, uint32_t nmemb, char* out);

// resolve function for every filesystem checks if the provided disk is compatible with it 
typedef int(*FS_RESOLVE_FUNCTION)(struct disk* disk);

// Filesystem Definition
struct filesystem
{
    // Filesystem should return 0 from resolve if the provided disk is using its filesystem
    
    // filesystem resolve function
    FS_RESOLVE_FUNCTION resolve;
    // file system open function
    FS_OPEN_FUNCTION open;
    // filesystem read function
    FS_READ_FUNCTION read;
    // file system name
    char name[20];

};

// File Descriptor Definition
struct file_descriptor
{
    /** A file Descriptor is basically an opened file */
    // The descriptor index
    int index;
    
    // file system associated with this file
    struct filesystem* filesysem;

    // private data for internal file descriptor
    // when we do fopen it returns some private data and we will use this info 
    // to pass this file to fread or fwrite for example
    void* private_ptr;


    // disk associated with the file
    struct disk* disk;
};

void fs_init();
int fopen(const char* filename , const char* mode_str);
int fread(void* ptr , uint32_t size , uint32_t nmemb, int fd);
void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);



#endif