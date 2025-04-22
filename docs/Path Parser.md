# Path Parser Implementation

## Overview  
This document summarizes the Implementation of the path parser and the string library

### Functionality 
- Path Parser Helps us retreieve and construct data from paths  
- string library helps dealing with strings

### Implementation

**struct path_root**
this struct represents the path root of any file
it contains:
- int drive_no; // drive number in which the file is in
- struct path_part* first; // pointer to the next part ( first part of the path)

**struct path_part**
this struct represents a part inside a path root of any file
- const char* part; // name of the part
- struct path_part* next; // pointer to the next part

**static int pathparser_path_valid_format(const char* filename)**
- Check if:
- Length of the path is larger than 3 characters
- The First character is drive number
- The Second and third characters are ":/"
- a valid charcter would be 0:/folder/text.txt 


**static int pathparser_get_drive_by_path (const char\** path)**
extract the drive number of the path

**static struct path_root* pathparser_create_root (int drive_number)**
create and initialize a path root 
 
**static const char* pathparser_get_path_part(const char\** path)**
- extract the path part 
- Explanation :
    This Function returns one part from the path 
    for ex: if the path after removing the drive number is bin/bash 
    first call will return bin second call will return bash 

**struct path_part* pathparser_parse_path_part(struct path_part* last_part , const char ** path)**
- Construct a path part from `path` and connect it to `last_part`

**struct path_root* pathparser_parse(const char* path , const char* current_directory_path)**
- parse and construct the path root and every path part into the path_root 