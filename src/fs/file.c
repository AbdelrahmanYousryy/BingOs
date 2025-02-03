#include "file.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../status.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "fat/fat16.h"
struct filesystem* filesystems[BINGOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[BINGOS_MAX_FILE_DESCRIPTORS];


/**  Return the first free filesystem slot to insert new filesystem into*/
static struct filesystem** fs_get_free_filesystem()
{
    int i=0;
    for(i = 0 ; i <BINGOS_MAX_FILESYSTEMS ; i++)
    {
        if(filesystems[i] == 0)return &filesystems[i];
    }

    return 0 ;
}

/** inserts new filesystem `filesystem` into the kernel */
void fs_insert_filesystem(struct filesystem* filesystem)
{
    //if(filesystem == 0)panic();
    // get a free filesystem slot
    struct filesystem** free_fs = fs_get_free_filesystem();
    // check if there is s a slot
    if(!free_fs)
    {
        print("Problem Inserting filesystem");
        while (1);
    }
    // insert the filesystem
    *free_fs = filesystem;

}

/** Loads static filesystems(filesystems build inside the kernel) */
static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());

}

/** load all the availabe filesystems */
void fs_load()
{
    // initialize all the filesystems with 0
    memset(filesystems , 0 , sizeof(filesystems));
    // load the internaly defined filesystem
    fs_static_load();

}
/** Initializing function for file system */
void fs_init()
{
    // initialize the file descriptors
    memset(file_descriptors , 0 , sizeof(file_descriptors));
    
    // load the availabe filesystem
    fs_load();
}

/** Create new file descriptors and returns it into `desc_out` */
static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM;
    for(int i = 0 ; i <BINGOS_MAX_FILE_DESCRIPTORS ; i++)
    {
        // if the file descriptor slot is free
        if(file_descriptors[i]==0)
        {
           // allocate some memory foe the file descriptor
           struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
           // Descriptors index starts at 1
           desc->index = i+1 ;
           // sets the free slot to desc
           file_descriptors[i] = desc;
           // return the newly created file descriptor
           *desc_out = desc;
           res = 0;
           break;
        }
    }

    return res;
}

/** Return a file descriptor from its index `fd` */
static struct file_descriptor* file_get_descriptor(int fd)
{
    //check for vaild fd
    if(fd<= 0 || fd>= BINGOS_MAX_FILE_DESCRIPTORS)return 0 ;
    else{
        // index is lower than fd by 1
        return(file_descriptors[fd-1]);
    }
}

/** Return the filesystem compatable with disk `disk` */
struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs =  0;
    // loop through all of the file systems
    for(int i =  0 ; i < BINGOS_MAX_FILESYSTEMS; i++)
    {
        // if the filesystem is there and is compatable with the disk
        if(filesystems[i]!=0 && filesystems[i]->resolve(disk)==0)
        {
            // return this file system
            fs =  filesystems[i];
            break;
        }
    }
    return fs;

}


int fopen(const char* file_name , const char* mode)
{
    return - EIO;
}