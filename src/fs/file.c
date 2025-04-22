#include "file.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../status.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "disk/disk.h"
#include "fat/fat16.h"
#include "string/string.h"

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

/** convert string `str` to FILE MODE */
FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if(strncmp(str,"r",1)==0)
    {
        mode = FILE_MODE_READ;
    }
    else if (strncmp(str,"w",1)==0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (strncmp(str,"a",1)==0)
    {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

// responsible for locating the correct file system to open the file
/* Basically it determines which filesystem this file belongs to and call its open function*/
int fopen(const char* filename , const char* mode_str)
{
    int res = 0;
    // extract the root path of the file 
    struct path_root* root_path = pathparser_parse(filename,NULL);
    if(!root_path)
    {
        res = -EINVARG;
        goto out;
    }

    // if its only a root path  without a file
    // ex : 0://
    if(!root_path->first)
    {
        res = -EINVARG;
        goto out;
    }

    // get the disk of the file and check if its exists
    struct disk* disk = disk_get(root_path->drive_no);
    if(!disk)
    {
        res = -EIO;
        goto out;
    }

    //  check for a filesystem
    if(!disk->filesystem)
    {
        res = -EIO;
        goto out;

    }

    // convert mode from string to FILE_MODE and check it
    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if  ( mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }

    // call the right filesystem's open function
    void* descriptor_private_data = disk->filesystem->open(disk,root_path->first,mode);
    if (ISERR(descriptor_private_data))
    {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    // initialize the file descriptor struct for this file
    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&desc);
    desc->filesysem = disk->filesystem;
    desc->private_ptr = descriptor_private_data;
    desc->disk = disk;
    // if everything worked return the index of the file
    res = desc->index;

out:
// fopen dont return negative values
    if (res < 0)
    {
        res = 0;
    }
    return res;
}