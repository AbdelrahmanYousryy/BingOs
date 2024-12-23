#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#ifndef PAGING_H
#define PAGING_H


#define PAGING_CACHE_ENABLED    0b00010000
#define PAGING_WRITE_THROUGH    0b00001000
#define PAGING_ACCESS_FROM_ALL  0b00000100
#define PAGING_IS_WRITABLE      0b00000010
#define PAGING_IS_PRESENT       0b00000001


#define PAGING_TOTAL_ENTRIES_PER_TABLE  1024
#define PAGING_PAGE_SIZE                4096
struct paging_4gb_chunck
{
    uint32_t* directory_entry;

};


void paging_switch (uint32_t* directory);
uint32_t* paging_4gb_chunck_get_directory(struct paging_4gb_chunck* chunck);
struct paging_4gb_chunck* paging_new_4gb(uint8_t flags);
int paging_set(uint32_t* directory , void* virt , uint32_t val);
int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out , uint32_t* table_index_out);
bool paging_is_alligned(void* address);


/** Enable  Paging
 *  Note : Has to be called after setting the page and switching to it
 */
void enable_paging();
#endif