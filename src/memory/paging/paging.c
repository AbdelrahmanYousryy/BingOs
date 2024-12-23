#include "paging.h"
#include "status.h"
#include  "memory/heap/kheap.h"

extern void paging_load_directory(uint32_t* directory);

// static current directory used in paging switch and not to be accessable outside this file
static uint32_t* current_directory = 0;

/** Implementing Paging Linearly without manipulating the real addresses using flags `flags` */
struct paging_4gb_chunck* paging_new_4gb(uint8_t flags)
{
    // get the space for the  directory table
    uint32_t* directory = kzalloc(sizeof(uint32_t)*PAGING_TOTAL_ENTRIES_PER_TABLE);
    

    int  offset = 0;
    // initialize the directory entries
    for(int i  = 0 ; i < PAGING_TOTAL_ENTRIES_PER_TABLE ; i++)
    {
        // get space for each page table
        uint32_t* entry = kzalloc(sizeof(uint32_t)*PAGING_TOTAL_ENTRIES_PER_TABLE);

        for(int b = 0 ; b < PAGING_TOTAL_ENTRIES_PER_TABLE ; b++)
        {
            // initialize each page table entry
            entry[b]= (offset +(b * PAGING_PAGE_SIZE))| flags;
        }

        // This indicates the offset due to change in the page table
        offset+=(PAGING_TOTAL_ENTRIES_PER_TABLE*PAGING_PAGE_SIZE);

        // Assign This Page Table to the page directory
        directory[i] = (uint32_t) entry | flags | PAGING_IS_WRITABLE ;
    }

    // allocating memory for the directory structure
    struct paging_4gb_chunck* chunck_4gb = kzalloc(sizeof(struct paging_4gb_chunck));
    
    // assign the directory we initialized to the memory allocated
    chunck_4gb->directory_entry = directory;
    
    // return initialized structure
    return  chunck_4gb;
}

/** Funcion to switch between pages in `directory` */
void paging_switch (uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;

}


/** get the address of the directory `chunck` */
uint32_t* paging_4gb_chunck_get_directory(struct paging_4gb_chunck* chunck)
{
    return chunck->directory_entry;
}

/** Check if `address` is alligned with `PAGING_PAGE_SIZE` */
bool paging_is_alligned(void* address)
{
    return ((uint32_t)address % PAGING_PAGE_SIZE == 0);
}


/** return the page directory index and page table index for `virtual_address` in `directory_index_out` and `table_index_out` */
int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out , uint32_t* table_index_out)
{
    int res = 0;
    if(!paging_is_alligned(virtual_address))
    {
        res = -EINVARG;
        goto out;

    }

    /* This Gets the index inside the page directory because (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) gets the total size of one directory 
        so for  example : directory[2] = 2 * (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)
                          directory[i] = i * (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)
                          so, i = directory[i] /(PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) */          
    *directory_index_out = ((uint32_t)virtual_address /(PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));

    /* This gets the index inside the page table because virtual address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) gives the remainder
     which is  the offset inside the page table and dividing it by (PAGING_PAGE_SIZE) gives the index of the page inside the page table
      */
    *table_index_out    = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res ;
}

/** Sets virtual address `virt` to physical address `val` in page directory `directory`
 *  note that `val` is the physical address combined with the flags 
 */
int paging_set(uint32_t* directory , void* virt , uint32_t val)
{
    // check if `virt` is alligned
    if (!paging_is_alligned(virt))
    {
        return -EINVARG;

    }
    // get indexes of the virtual address `virt`
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virt ,&directory_index , &table_index);
    if(res < 0)
    {
        return res;
    }
    // this points to the page table 
    uint32_t entry = directory[directory_index];
    // extract only the address without the flags 
    uint32_t* table = (uint32_t*) (entry & 0xFFFFF000); 
    // assign pyhsical address to table index `table_index`
    table[table_index]=val;
    return res;


}