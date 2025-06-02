#include "fat16.h"
#include "../../disk/disk.h"
#include "../../disk/streamer.h"
#include "../string/string.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "../status.h"
#include "kernel.h"
#include <stdint.h>



#define BINGOS_FAT16_SIGNATURE       0x29
#define BINGOS_FAT16_FAT_ENTRY_SIZE  0x02
#define BINGOS_FAT16_BAD_SECTOR      0xFF7
#define BINGOS_FAT16_USUSED          0x00

// Fat Directory entry attributes bitmask(internal rep wont be saved on disk)
typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0   // Directory
#define FAT_ITEM_TYPE_FILE      1   // FILE


// Fat Directory Entry Attributes bitmask that will represt directories
#define FAT_FILE_READ_ONLY      0x01
#define FAT_FILE_HIDDEN         0x02
#define FAT_FILE_SYSTEM         0x04
#define FAT_FILE_VOLUME_LABEL   0x08
#define FAT_FILE_SUBDIRECTORY   0x10
#define FAT_FILE_ARCHIVED       0x20
#define FAT_FILE_DEVICE         0x40
#define FAT_FILE_RESERVED       0x80

/** The Following headers are used to store the header retireved from the drive */
//  extended fat header implementation
struct fat_header_extended
{
    uint8_t  drive_number;
    uint8_t  win_nt_bit;
    uint8_t  signature;
    uint32_t volume_id;
    uint8_t  volume_id_string[11];
    uint8_t  system_id_string[8];
} __attribute__((packed)); // attribute packed assures that the compiler
                          // doessnt re arrange this structure

                          
/* So Basically this struct will hold the fat16 header stored in the drive */
struct fat_header
{
    uint8_t  short_jmp_ins[3]   ;           // to carry jump short start (machine code)
    uint8_t  oem_identifier[8]  ;           // 8 bytes identifier                       
    uint16_t  bytes_per_sector   ;          //512 bytes per sector                     
    uint8_t  sectors_per_cluster;           //128 sector per cluster                   
    uint16_t reserved_sectors   ;           //200 Sectors reserved for the kernel before allocation table      
    uint8_t  fat_copies         ;           //2 Fat Copies ( the original and a backuo)
    uint16_t root_dir_entries   ;           //64 Root Directories entries                
    uint16_t number_of_sectors  ;           //Number of sectors (0 for large disks)    
    uint8_t  media_type         ;           //Media descriptor ( Hard Drive )          
    uint16_t sectors_per_fat    ;           //Number of sectors per FAT table          
    uint16_t sectors_per_track  ;           //32 Sectors per track                     
    uint16_t number_of_heads    ;           //64 heads (CHS addressing)                
    uint32_t hidden_sectors     ;           //Hidden sectors before partiotion         
    uint32_t sectors_big        ;           //large sector count                       
}__attribute__((packed));


// this is both combined the original and the extended and this is the one 
// that will hold the whole header
struct fat_h
{
    struct fat_header primary_header;
    union fat_h_e
    {
        struct fat_header_extended extended_header;
    }shared;
};

/* This Represernt a file or a directory in the fat16 filesystem */
struct fat_directory_item
{
    uint8_t filename[8];  // name of the item
    uint8_t ext[3];       // extension of the item
    uint8_t attribute;   // two bytes for item's attributes defined above
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec; // creation time of the directory item
    uint16_t creation_time;           // creation time
    uint16_t creation_date;           // cration date
    uint16_t last_access;             // last access time
    uint16_t high_16_bits_first_cluster ;
    /* highest 16 bits of where the first cluster of the data for this directory item
     either it holds the data for another directory items if its a directory ot it holds a data for a file*/
    uint16_t lost_mod_time;         // last modification time
    uint16_t lost_mod_date;         // last modification date
    /* lowest 16 bits of where the first cluster of the data for this directory item
     either it holds the data for another directory items if its a directory ot it holds a data for a file*/
    uint16_t low_16_bits_first_cluster ; 
    uint32_t filesize;              //file size

} __attribute__((packed));

/* Internal structure to  help us manage the directory item easier */
struct fat_directory
{
    /*  this points to the first item in the directory and can be indexed
        to reach other items */
    struct fat_directory_item* item; 
    int total; // total number of items onsode the directoy
    int sector_pos; // the first sector that contains data for this directory
    int ending_sector_pos; //  the last sector containing data for this directory

};


/* Internal structure to  help us manage the file or directory item easier */
struct fat_item
{
   union 
   {
        /* if it is a file it will access the item structure*/
        struct fat_directory_item* item;
        /* if it is a directory it will access the directory structure*/
        struct fat_directory* directory;
   };

   // declares the fat item type
   FAT_ITEM_TYPE type;
   
};


/* This represents an open file */
struct fat_file_descriptor
{
    struct fat_item* item;  // representation of this file or directpry
    uint32_t pos; // current position of this opened file

};

/* Contatins information to help us manage the file system*/
struct fat_private
{
    struct fat_h header;
    struct fat_directory root_directory;

    // used to stream data clusters
    struct disk_stream* cluster_read_stream;

    // used to stream the file allocation table
    struct disk_stream* fat_read_stream;

    // used to stream the directory
    struct disk_stream* directory_stream;

    /* We Make Multiple streams to allow us to retreive data from the disk easily 
    and seek to different parts of the file system  and read maybe 5 or 10 bytes
    at a time rather than the whole sector */

};

/** return the absolute position of the sector `sector` inside `disk` */
int fat16_sector_to_absolute(struct disk* disk , int sector)
{
    return sector * disk ->sector_size;


}

/** Return Number of items inside directorr the directory that starts with cluster `directory_start_cluster`  */
int fat16_get_total_items_for_directory(struct disk* disk , uint32_t directory_start_sector)
{
    struct fat_directory_item item;
    // create empty item in the stack and initialize it
    struct fat_directory_item empty_item;
    memset(&empty_item,0,sizeof(empty_item));

    // point to the private struct of the disk to extract info
    struct fat_private* fat_private =disk->fs_private;
    
    int res = 0;    // result
    int i = 0;      // counter
    // get the position of the directory
    int directory_start_pos = fat16_sector_to_absolute(disk , directory_start_sector);
    
    // create a streamer to read from this directory and seek to the directory 
    struct disk_stream* dir_stream = fat_private->directory_stream;
    if(diskstream_seek(dir_stream,directory_start_pos)!= BINGOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    // read each directory item 
    while(1)
    {
        if (diskstream_read(dir_stream,&item,sizeof(item))!= BINGOS_ALL_OK)
        {
            res = -EIO;
            goto out;
        }
        // blank item exit the loop
        if(item.filename[0] == 0x00)
        {
            break;

        }
        // the item is unused (in the fat16 manual)
        if(item.filename[0] == 0xE5)
        {
            continue;
        }
        i++;
    }

    res = i;

out:
    return res;

}
/** loads the root directory of a fat16 filesystem within disk `disk` into `directory` */
int fat16_get_root_directory(struct disk* disk , struct fat_private* fat_private , struct fat_directory* directory)
{
    int res = 0;


    struct fat_header* primary_header = &fat_private->header.primary_header;

    // calculate the root directory sector
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat)+ primary_header->reserved_sectors;

    //calculate the root directory entries
    int root_dir_entries = fat_private->header.primary_header.root_dir_entries;

    // calculate the root directory size
    int root_dir_size = root_dir_entries * sizeof(struct fat_directory_item);

    // calculate the number of sectors needed to read the root directory
    int total_sectors = root_dir_size /disk->sector_size;
    if(root_dir_size % disk->sector_size)
    {
        total_sectors+=1;
    }

    // get the total number of items inside the root directory
    int total_items = fat16_get_total_items_for_directory(disk,root_dir_sector_pos);

    // Initialize the direcetroy that will save the root directory
    struct fat_directory_item* dir = kzalloc(root_dir_size);
    if(!dir)
    {
        res = -ENOMEM;
        goto out;
    }

    // Seek to the position of the root directory
    struct disk_stream* stream = fat_private->directory_stream;
    if(diskstream_seek(stream,fat16_sector_to_absolute(disk ,root_dir_sector_pos))!= BINGOS_ALL_OK)
    {
        res = -EIO; 
        goto out;
    }

    // read root directory
    if(diskstream_read(stream,dir,root_dir_size)!= BINGOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    // set the output directory `directory`
    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size/disk->sector_size);
out:
    return res;

}
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


/** initialize private structure and binds it to the disk `disk` */
static void fat16_init_private(struct disk* disk, struct fat_private* private)
{
    // initialize the private structure
    memset(private,0, sizeof(struct fat_private));
    private->cluster_read_stream = diskstream_new(disk->id);
    private->fat_read_stream = diskstream_new(disk->id);
    private->directory_stream = diskstream_new(disk->id);
    
}
/**  The Resolve Function for fat16 filesystem responsible for determining
 *   if this disk has fat16 filesystem and it should be managed by fat16
*/
int fat16_resolve(struct disk* disk)
{
    int res = 0;

    // initialize fat_private and bind to to the disk so we can 
    // access information inside this disk
    struct fat_private* fat_private = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk,fat_private);

    // this says yes the filesystem is fat16 and saves the private into the disk
    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;

    // create a stream to read the header
    struct disk_stream* stream = diskstream_new(disk->id);
    if(!stream)
    {
        res = -ENOMEM;
        goto out;
    }
    // read the header (first thing in the disk)
    if(diskstream_read(stream ,&fat_private->header , sizeof(fat_private->header))!=BINGOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }
    // check if the header we loaded have fat16 signature
    if(fat_private->header.shared.extended_header.signature!= 0x29)
    {
        //  not us (another filesystem)
        res = -EFSNOTUS;
        goto out;
    }

    // get the root directory 
    if(fat16_get_root_directory(disk,fat_private , &fat_private->root_directory)!= BINGOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    
out:
    
    // close the stream if its open
    if(stream)
    {
        diskstream_close(stream);
    }
    // unbind
    if(res<0)
    {
        kfree(fat_private);
        disk->fs_private = 0;
    }
    // return the result
    return res;

}
/** delete the space padding of a filename */
void fat16_to_proper_string(char** out , const char* in)
{
    while (*in != 0x00 && *in != 0x20)
    {
        **out = *in;
        *out += 1;
        in +=1;
    }
    if ( *in == 0x20)
    {
        **out = 0x00;
    }
}

/**this resolves the file name and the extension into one file name
Test ABC -> Test.ABC

*/
void fat16_get_full_relative_filename(struct fat_directory_item* item , char* out , int max_len)
{
    // initialize the output string
    memset(out,0x00,max_len);
    char* out_temp = out;
    // return the filename
    fat16_to_proper_string(&out_temp,(const char*)item->filename);
    if(item->ext[0] != 0x00 && item->ext[0] != 0x20)
    {
        // we have an extension
        *out_temp++  = '.';
        fat16_to_proper_string(&out_temp ,(const char*)item->ext);
    }

}

/** clone `item` and return the copy  */
struct fat_directory_item* fat16_clone_directory_item(struct fat_directory_item* item , int size)
{
    struct fat_directory_item* item_copy = 0;
    if(size < sizeof(struct fat_directory_item))
    {
        return 0 ;
    }
    item_copy = kzalloc(size);
    
    if(!item_copy)
    {
        return 0;
    }

    memcpy(item_copy,item , size);
    return item_copy;

}
/** return the first cluster representing an item  */
static uint32_t fat16_get_first_cluster(struct fat_directory_item* item)
{
   return (item->high_16_bits_first_cluster || item->low_16_bits_first_cluster); 

}

/** Returns the first sector in a cluster */
static int fat16_cluster_to_sector(struct fat_private* private , int cluster)
{
    // starting from the ending sector of the root directory , add the cluster * sectorss per cluster
    return private->root_directory.ending_sector_pos +((cluster-2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private* private)
{
    // the file allocation table comes directly after the reserved sectors
    return private->header.primary_header.reserved_sectors;
}


/** get the FAT entry of `cluster` */
static int fat16_get_fat_entry(struct disk* disk , int cluster)
{
    int res = -1;
    // retreive the private struct
    struct fat_private* private = disk->fs_private;
    // retreive the header reading stream
    struct disk_stream* stream = private->fat_read_stream;
    if(!stream)
    {
        goto out;
    }

    // get the file allocation table position
    uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
    // seek the stream to the wanted cluster entry
    res= diskstream_seek(stream, fat_table_position + (cluster * BINGOS_FAT16_FAT_ENTRY_SIZE));
    if(res < 0)
    {
        goto out;
    }
    uint16_t result = 0;
    // read the entry into result
    res = diskstream_read(stream, &result, sizeof(result));
    if(res < 0)
    {
        goto out;
    }
    // return result
    res = result;

out:
    return res;

}

/** get the correct cluster to use based on the starting cluster and offset */
static int fat16_get_cluster_for_offset(struct disk* disk , int starting_cluster , int offset)
{
    int res = 0;
    // retrieve the private struct of the disk
    struct fat_private* private = disk -> fs_private;
    // get size of cluster in bytes
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    // starting cluster
    int cluster_to_use = starting_cluster;
    // calculate number of clusters ahead needed
    int clusters_ahead = offset /  size_of_cluster_bytes;
    for(int i=0 ; i < clusters_ahead ; i++ )
    {
        // get the entry of the  cluster
        int entry = fat16_get_fat_entry(disk,cluster_to_use);

        if(entry == 0xFF8 || entry == 0XFFF)
        {
            // last entry of the file
            res = -EIO;
            goto out;
        }

        // sector is marked bad
        if(entry == BINGOS_FAT16_BAD_SECTOR)
        {
            res = -EIO;
            goto out;
        }
        // reserved
        if(entry == 0xFF0 || entry == 0xFF6)
        {
            res = -EIO;
            goto out;
        }
        // no cluster
        if(entry == 0x00)
        {
            res = -EIO;
            goto out;
        }
        // else the entry contains the next cluster
        cluster_to_use = entry;
    }

    res = cluster_to_use;
out:
    return res;


}

/** read internal regardless of the clusters */
static int fat16_read_internal_from_stream(struct disk* disk , struct disk_stream* stream , int cluster , int offset, int total, void* out)
{
    int res =0;
    // retreive the disk private struct
    struct fat_private* private = disk->fs_private;
    // get the size of clusters in bytes
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    // get the right cluster after calulcating offset to start from
    int cluster_to_use = fat16_get_cluster_for_offset(disk,cluster,offset);
    if(cluster_to_use < 0)
    {
        res= cluster_to_use;
        goto out;
    }

    // get the offset in this cluster
    int offset_from_cluster = offset % size_of_cluster_bytes; 
    // get the starting position to read from
    int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
    // get totaal to read
    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
    res = diskstream_seek(stream, starting_pos);
    if(res != BINGOS_ALL_OK)
    {
        goto out;
    }
    res = diskstream_read(stream, out , total_to_read);

    total -= total_to_read;
    if ( total > 0)
    {
        // we still have more to read
        // recursion with adjusting the offsset and the output mem
        res = fat16_read_internal_from_stream(disk , stream , cluster , offset + total_to_read , total, out+total_to_read);
    }
out:
    return res;


}

/** This function reads `total` bytes starting from `starting_cluster` into `out` */
static int fat16_read_internal(struct disk* disk , int starting_cluster , int offset , int total , void* out)
{
    // Retreive the private struct of the disk
    struct fat_private* fs_private = disk->fs_private;
    // retreive the cluster reading stream
    struct disk_stream* stream = fs_private->cluster_read_stream;
    // read from the stream
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);


}

/** free directory struct */
void fat16_free_directory(struct fat_directory* directory)
{
    if(!directory)
    {
        return;
    }
    if(directory->item)
    {
        kfree(directory->item);
    }
    kfree(directory);

}

/** ree item*/
void fat16_fat_item_free(struct fat_item* item)
{
    if(item->type == FAT_ITEM_TYPE_DIRECTORY)
    {
        fat16_free_directory(item->directory);
    }
    else if (item->type == FAT_ITEM_TYPE_FILE)
    {
        kfree (item->item);
    }
    kfree(item);

}
/** Load Directory item into fat_directory item */
struct fat_directory* fat16_load_fat_directory ( struct disk* disk , struct fat_directory_item* item)
{
    // initialize result
    int res = 0;
    // define directory to load the directory into
    struct fat_directory* directory = 0;
    // point to the fat_private of the disk
    struct fat_private* fat_private = disk->fs_private;

    // check that the item is not file
    if(!(item->attribute & FAT_FILE_SUBDIRECTORY))
    {
        res = -EINVARG;
        goto out;
    }

    // initialize the directory
    directory = kzalloc(sizeof(struct fat_directory));
    if(!directory)
    {
        res = - ENOMEM;
        goto out;
    }

    // get the first cluster of the item
    int cluster        = fat16_get_first_cluster(item);
    // convert the cluster to the sector
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    // get the total items of the directory
    int total_items    = fat16_get_total_items_for_directory(disk , cluster_sector);
    // assign the total items to directory
    directory->total   =  total_items;
    // calculate the directory size
    int directory_size = directory->total * sizeof(struct fat_directory_item);
    // initialize the item  inside the directory struct
    directory->item = kzalloc(directory_size);
    if(!directory->item)
    {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk , cluster , 0x00 , directory_size , directory->item);
    if(res != BINGOS_ALL_OK)
    {
        goto out;
    }
out:
    if ( res != BINGOS_ALL_OK)
    {
        fat16_free_directory(directory);
    }
    return directory;

}



/* create and initialze the `fat_item` for a `direcctory_fat_item` inside `disk` */
struct fat_item* fat16_new_fat_item_for_directory_item(struct disk* disk , struct fat_directory_item* item)
{
    // create and initialize the fati_item
    struct fat_item* f_item = kzalloc(sizeof(struct fat_item));
    if(!f_item)
    {
        return 0;
    }
    
    // this file represents a sub directory ?
    if(item->attribute & FAT_FILE_SUBDIRECTORY )
    {
        f_item->directory = fat16_load_fat_directory(disk , item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;

    }
    f_item->type =FAT_ITEM_TYPE_FILE;
    f_item->item = fat16_clone_directory_item(item , sizeof(struct fat_directory_item));
    return f_item;

}

/** searches for the file inside the directory and returns the `item` representation of it if availabele */
struct fat_item* fat16_find_item_in_directory(struct disk* disk , struct fat_directory* directory, const char* name)
{
    struct fat_item* f_item = 0;
    char tmp_filename[BINGOS_MAX_PATH];
    for(int i = 0 ; i < directory->total ; i++)
    {
        // this resolves the file name and the extension into one file name
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename , sizeof(tmp_filename));
        if (istrncmp(tmp_filename , name , sizeof(tmp_filename))==0)
        {
            // found the file
            // create 
            f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return f_item;

}
// returns the item struct that represents `path` if found
struct fat_item* fat16_get_directory_entry(struct disk* disk , struct path_part* path)
{

    struct fat_private* fat_private = disk ->fs_private;
    struct fat_item*    current_item = 0;
    // this returns the item of the root of this path 0:/(abc)/test.txt
    struct fat_item*    root_item = fat16_find_item_in_directory(disk , &fat_private->root_directory, path->part);
    if(!root_item)
    {
        
        goto out;
    }
    struct path_part* next_part = path->next;
    current_item = root_item;
    // is there more items in the path ?
    while(next_part!= 0)
    {
        if(current_item->type != FAT_ITEM_TYPE_DIRECTORY)
        {
            current_item =0;
            break;
        }
        // find the next item
        struct fat_item* tmp_item = fat16_find_item_in_directory(disk , current_item->directory, next_part->part);
        // free the old one
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;   
    }
out:
    return current_item;

}
/* the open function fot fat16 */
void* fat16_open(struct disk* disk , struct path_part* path , FILE_MODE mode)
{
    // check for the file mode
    if (mode != FILE_MODE_READ)
    {
        return ERROR(-ERDONLY);
    }

    // initialize the file descriptor
    struct fat_file_descriptor* descriptor = 0;
    descriptor = kzalloc(sizeof(struct fat_file_descriptor));
    if(!descriptor)
    {
        return ERROR(-ENOMEM);
    }
    // assign the file item to the descriptor
    descriptor->item = fat16_get_directory_entry(disk,path);
    if(!descriptor->item)
    {
        return ERROR(-EIO);

    }
    // initialize the file position
    descriptor->pos = 0;
    return descriptor ;
}