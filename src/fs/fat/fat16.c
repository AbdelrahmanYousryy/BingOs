#include "fat16.h"
#include "../string/string.h"
#include "../status.h"



int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk , struct path_part* path , FILE_MODE mode);
struct filesystem fat16_fs =
{
    // assign the fat16 resolve function
    .resolve = fat16_resolve,
    .open = fat16_open
};



struct filesystem* fat16_init()
{
    /* Assign Name Fat 16 to the filesystem*/
    strcpy(fat16_fs.name,"FAT16");
    // return the Fat 16 filesystem
    return &fat16_fs;
}

/**  The Resolve Function for fat16 filesystem */
int fat16_resolve(struct disk* disk)
{
    return 0;
}

/* the open function fot fat16 */
void* fat16_open(struct disk* disk , struct path_part* path , FILE_MODE mode)
{

    return 0 ;
}