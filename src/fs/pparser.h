#ifndef PATHPPARSER_H
#define PATHPARSER_H

/** Struct to contain the Path root of any file  */
struct path_root
{   
    // drive number
    int drive_no;

    // pointer to the next part ( first part of the path)
    struct path_part* first;
};


/** Struct to contain the next path of any file  */
struct path_part
{
    const char* part;
    struct path_part* next;
};

struct path_root* pathparser_parse(const char* path , const char* current_directory_path);

void pathparser_free(struct path_root* root);

#endif