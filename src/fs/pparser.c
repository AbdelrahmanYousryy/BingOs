#include "pparser.h"
#include "../string/string.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"

/** Check if the path `filename` is valid  */
static int pathparser_path_valid_format(const char* filename)
{
    int len  = strnlen(filename, BINGOS_MAX_PATH);

    // Check if:
    // Length of the path is larger than 3 characters
    // The First character is drive number
    // The Second and third characters are ":/"
    // a valid charcter would be 0:/folder/text.txt    
    return(len >= 3 && isdigit(filename[0]) && memcmp((void*)&filename[1],":/",2) == 0);  
}


/** Get The Drive Number of the path */
static int pathparser_get_drive_by_path (const char** path)
{
    // check if its valid format 
    if(!pathparser_path_valid_format(*path))
    {
        return -EBADPATH;
    }
    /** Return numeric version of the drive number */
    int drive_no = tonumericdigit(*path[0]);

    // add three bytes to skip the drive number part , 0:/
    *path+= 3;

    return drive_no;
}

/** Create the path root of the path */
static struct path_root* pathparser_create_root (int drive_number)
{
    // initialize root path 
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    
    path_r->drive_no = drive_number;
    path_r->first =0;
    return path_r;
}

/** extract the path part */
/* Explanation :
    This Function returns one part from the path 
    for ex: if the path after removing the drive number is bin/bash 
    first call will return bin second call will return bash 
*/
static const char* pathparser_get_path_part(const char** path)
{
    // copy the path so we make sure we have the right to
    // modify it and make sure its on the heap
    char* result_path_part = kzalloc(BINGOS_MAX_PATH);
    int i = 0 ; 
    // copy the path part into result_path_part
    while(**path != '/' && **path != 0x00)
    {
        result_path_part[i] =**path;
        *path += 1 ;
        i++;
    }

    if(**path == '/')
    {
        // skip the forward slash to avoid problems
        *path += 1;
    }

    // free the path part this means nothing is parsed
    if(i == 0)
    {
        kfree(result_path_part);
        result_path_part = 0;
    }
    return result_path_part;
}

/** Construct a path part from `path` and connect it to `last_part` */
struct path_part* pathparser_parse_path_part(struct path_part* last_part , const char ** path)
{
    // get the current part of the path
   const char* path_part_str = pathparser_get_path_part(path);
   // if no path exists return
   if(!path_part_str)
   {
    return 0;
   }
   // create a struct to hold the extracted part
   struct path_part* part = kzalloc(sizeof(struct path_part));
   part->part = path_part_str;
   part->next = 0x00;

    // connect the last part to the curent part 
    // for ex if the last part was bin
    // we connect them to be bin -> bash
    // so the final path will be 0 > bin > bash
    if(last_part)
    {
        last_part->next = part;
    }

    // retutn the configured part
    return part;
}


/** free all the memory taken */
void pathparser_free(struct path_root* root)
{
    struct path_part* part = root-> first;
    while(part)
    {
       struct  path_part* next_part = part -> next;
       kfree((void*)part->part);
       kfree((void*)part);
       part = next_part;
    }
    kfree(root);
}

/** Parse the given `path` */
struct path_root* pathparser_parse(const char* path , const char* current_directory_path)
{
    int res = 0;
    // copy path to manipulate it
    const char* tmp_path = path;
    // initialize the path root
    struct path_root* path_root = 0;
    // check if path is biger than the limut
    if(strlen(path) > BINGOS_MAX_PATH)
    {
        goto out;
    }

    // get the drive number
    res = pathparser_get_drive_by_path(&tmp_path);
    if(res < 0)
    {
        goto out;
    }
    // construct the path root
    path_root = pathparser_create_root(res);
    if(!path_root)
    {
        goto out;
    }
    // construct the first part
    struct path_part* first_part = pathparser_parse_path_part(NULL,&tmp_path);
    if(!first_part)
    {
        goto out;
    }
    path_root -> first = first_part;
    // continue configuringg the path until path is finished
    struct path_part* part = pathparser_parse_path_part(first_part,&tmp_path);
    while(part)
    {
        part = pathparser_parse_path_part(part , &tmp_path);
    }

    // return the root of the path
    return path_root;


out:
    return path_root;

}