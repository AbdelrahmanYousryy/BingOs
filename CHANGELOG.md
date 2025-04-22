# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]
### Added
- Placeholder for upcoming features.

### Changed
- Placeholder for updates to existing functionality.

### Fixed
- Placeholder for bug fixes.

---

## [2025-1-8]  (PATH PARSER - STRING LIB) Implementaion 
### Added
- static int pathparser_path_valid_format(const char* filename) .
- static int pathparser_get_drive_by_path (const char** path).
- static const char* pathparser_get_path_part(const char** path) . 
- struct path_part* pathparser_parse_path_part(struct path_part* last_part , const char ** path) .
- void   pathparser_free(struct path_root* root) .
- struct path_root* pathparser_parse(const char* path , const char* current_directory_path) .
- struct path_root .
- struct path_part .
- int memcmp(void* s1 , void* s2 , int count) .
- int strlen(const char* ptr) .
- bool isdigit(char c) .
- int tonumericdigit(char c) . 
- int strnlen(const char* ptr , int max) .


## [2025-1-10]  (Disk Streamer) Implementaion 
### Added
- struct disk_stream;
- struct disk_stream* diskstream_new (int disk_id);
- int disksteram_seek(struct disk_stream* stream ,int pos);
- int diskstream_read(struct disk_stream* stream , void* out , int total);
- void diskstream_close(struct disk_stream* stream);



## [2025-1-19]  (FAT16 Header) Implementaion 
### Added
- FAT16 Header in boot.asm;
- extended BPB in boot.asm;

### Changed 
- Changed make all to mount the filesystem on /mnt/ for testing
 
## [2025-1-23]  (Virtual Filesystem ) Implementaion 
### Added
- Configurations for FILESYSTEM
- added filesystem member to disk struct
- static struct filesystem** fs_get_free_filesystem()
- void fs_insert_filesystm(struct filesystem* filesystem)
- static int file_new_descriptor(struct file_descriptor** desc_out)
- static struct file_descriptor* file_get_descriptor(int fd)
- struct filesystem* fs_resolve(struct disk* disk)
- defined pointer to open and resolve function to be used by every filesystem 
- implementaion of struct filesystem and struct file descriptor

### fixed  
- Documentation Typos


## [2025-2-1]  (Documentation) 
### Added
- More Comments Added for the whole project



## [2025-2-3]  (FAT16 Core) Implementation 
### Added
- strcpy funtion to be used in fat16 implementation.
- struct filesystem fat16_fs (implementation of the fat16 filesystem struct).
- struct filesystem* fat16_init() ( Initializes and returns the fat16 filesystem)
- int fat16_resolve(struct disk* disk) ( Definition )
- void* fat16_open(struct disk* disk , struct path_part* path , FILE_MODE mode)
- CHANGELOG.md 

### fixed
- typo in fs_insert_filesystem(fat16_init())

### notes 
- So now we can insert fat16 filesystem into kernel with fs_insert_filesystem(fat16_init())
- void disk_search_and_init() initializes the main disk and searches for appropriate filesystem to attach to it



## [2025-3-19]  (FAT16 Core) Implementation 
### Added
- struct fat_header_extended 
- struct fat_header
- struct fat_h 
- struct fat_directory_item
- struct fat_directory
- struct fat_item
- struct fat_private
- int fat16_sector_to_absolute(struct disk* disk , int sector)
- int fat16_get_total_items_for_directory(struct disk* disk , uint32_t directory_start_sector)
- int fat16_get_root_directory(struct disk* disk , struct fat_private* fat_private , struct fat_directory* directory)
- istatic void fat16_init_private(struct disk* disk, struct fat_private* private)
- int fat16_resolve(struct disk* disk) (implementation)
- CHANGELOG.md 

### fixed
- typo in diskstream_seek()

### notes 
- now we can bind the fat16 filesytem to a disk using the resolve function