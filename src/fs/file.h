#ifndef FILE_H
#define FILE_H

#include "pparser.h"
#include <stdint.h>

/** Types of Seeking Mode */
typedef unsigned int FILE_SEEK_MODE;
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};


/** Types of file mode so each can be opened at one mode  */
typedef unsigned int FILE_MODE;
enum
{ 
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;
/** Each filesystem defines these pointer to point at its own fs functions */
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk* disk, void* private, uint32_t size, uint32_t nmemb, char* out);
typedef int (*FS_SEEK_FUNCTION)(void* private , uint32_t offset, FILE_SEEK_MODE seek_mode);
// resolve function for each filesystem checks whether provided disk is compatible with it
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);


struct filesystem
{
    // Filesystem should return zero from resolve if the provided disk is using its filesystem
    // pointer to resolve function
    FS_RESOLVE_FUNCTION resolve;
    // pointer to open function
    FS_OPEN_FUNCTION open;
    // pointer to read function
    FS_READ_FUNCTION read;
    // pointer to seek function
    FS_SEEK_FUNCTION seek;
    // filesystem name
    char name[20];
};

/** File Descriptor is an open file */
struct file_descriptor
{
    // The descriptor index
    int index;

    // filesystem associated with this file
    struct filesystem* filesystem;

    // Private data for internal file descriptor
    void* private;

    // The disk that the file descriptor should be used on
    struct disk* disk;
};

void fs_init();
int fopen(const char* filename, const char* mode_str);
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd);
int fseek(int fd , int offset , FILE_SEEK_MODE whence);
void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);
#endif