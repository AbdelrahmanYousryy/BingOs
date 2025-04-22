# Phase 3: File System Implementation

## Overview  
This document summarizes the process of  Implementing the filesystem in our kernel.  


### FAT16 Implementation


- Implmenting tha Fat16 strutures needed , first the extended one (fat_header_extended) and the original (fat_header) , these structures will be used to store the loaded fat 16 header implemented in the drive by boot.asm .


- Impemented fat_h , basically its a combined struct to contain both the extended and the original header

- now to represent an item inside a directory we implemted a struct fat_directory_item this could represent a file or a directory based on the FAT_ITEM_TYPE inside the struct fat_item 

- we declare some structs internally meaning it wont be loaded into anything , only us will be using it in coding to help us manage the FAT system including fat_directory , this struct manages the directory items only 

- fat_item_descriptor is used to represent an open file 

- to help us retrieve data easily form the disk we implmented an internal struct called fat_private , basically it contains multiple disk_stream structs that we can read data from , example one for the fat table and one for the directories and one for clusters reading

- now to implement the resolving function we need to implement some helper functions as:
    - fat16_sector_to_absolute  // converts the sector number to absolute position in int
    - fat16_get_total_items_for_directory // gets the number of items inside the directories
    - fat16_get_root_directory // reads the root directory of a fat16 filesystem
    - fat16_init_private // defines a private struct annd binds it to  a disk

- implementation of the resolver function:
    first we need to retreive data from the disk we are trying to check if its runs on fat16 filesystem so we define a fat_private struct so we can retrive info easily from the disk then we bind this fat_private to the disk then we load the header of the disk into the fat_private->header then check the signature inside the header if it matches the signature of a fat16 fs (0x29) then its a fat16 fs
    and we load the root directory of the disk into the private struct of the disk that we defined , so what we did is we checked if this disk has fat16 and if yes we define and complete the private struct of the disk then binf it to the disk