#include "heap.h"
#include "kernel.h"
#include <stdbool.h>
#include "status.h"
#include "memory/memory.h"

/** Validate The Heap Table */
static int heap_validate_table(void * ptr , void* end , struct heap_table* table)
{
    int res = 0;
    size_t calculated_total_blocks = (size_t)(end - ptr)/BINGOS_HEAP_BLOCK_SIZE; // calculate the needed blocks
    if(table->total != calculated_total_blocks)                                   // compare the needed clock with the calculated
    {
        res = -EINVARG;
        goto out;
    }

out:
    return res;


}

/** Validate The Allignment of the Heap */
static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % BINGOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap , void* ptr , void* end , struct heap_table* table)
{
    int res = 0;

    // first check the valid arguments
    if(!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
    {
        res = -EINVARG;
        goto out;
    }

    // initialize the heap structure to 0 
    memset(heap,0,sizeof(struct heap));

    // setup the heap structure
    heap->saddr = ptr;
    heap->table = table;

    // validate the heap table
    res = heap_validate_table(ptr , end , table);
    if (res <0)
    {
        res = -EINVARG;
        goto out;
    }

    // calculate the table size
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total ;
    // initialize the heap table to Free
    memset(table->entries,HEAP_BLOCK_TABLE_ENTRY_FREE,table_size);
    


out:    
    return res;



}

/** Allign the requested size to be multiple of Block size */
static uint32_t heap_align_value_to_upper(uint32_t val)
{
    if(val % BINGOS_HEAP_BLOCK_SIZE == 0)return val;

    val = (val - (val % BINGOS_HEAP_BLOCK_SIZE));
    val+= BINGOS_HEAP_BLOCK_SIZE;
    return val;

}

/** Return Whether this block is free or taken */
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f;
}

/** Return the First Block with enough total_blocks in series to occupy */
int heap_get_start_block(struct heap* heap , uint32_t total_blocks)
{
    struct heap_table* table = heap->table;
    int bc = 0;
    int bs = -1;
    for (size_t i = 0; i < table->total; i++)
    {
       if( heap_get_entry_type(table->entries[i])!= HEAP_BLOCK_TABLE_ENTRY_FREE )
       {
            bc = 0;
            bs = -1;
            continue;
       }

        // found free block
       if(bs == -1)
       {
            bs = i;
       }
       bc++;

       // found start block with enough totalblocks to occupy
       if(bc == total_blocks)break;

    }
    if(bs == -1)
    {
        return -ENOMEM;
    }
    return bs;
    
}

/** Get the Absolute address of the start block */
void* heap_block_to_address(struct heap* heap ,uint32_t block)
{
    void* address = ((heap->saddr)+(block*BINGOS_HEAP_BLOCK_SIZE));
    return address;

}
/** Sets The Right values for taken blocks */
void heap_mark_blocks_taken(struct heap* heap , int start_block , int total_block)
{
    // calculate the end block
    int end_block = start_block +total_block - 1;
    // set the flags for the first block
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN |HEAP_BLOCK_IS_FIRST;
    if(total_block >1) entry |=HEAP_BLOCK_HAS_NEXT;
    
    //
    for(int i = start_block ; i <= end_block ;  i++)
    {
        heap->table->entries[i] = entry;
        // set the flags for the next block
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        // if the enxt block isnt the last block
        if(i != end_block-1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}
/** Allocate memory based on number of blocks needed */
void* heap_malloc_blocks(struct heap* heap , uint32_t total_blocks)
{
    void* address = 0;

    int start_block = heap_get_start_block(heap , total_blocks);
    if(start_block < 0)
    {
        goto out;
    }

    address = heap_block_to_address(heap,start_block);
    

    // Mark The Blocks as taken
    heap_mark_blocks_taken(heap,start_block,total_blocks);
out:
    return address;


}
void heap_mark_blocks_free(struct heap* heap , uint32_t block)
{
    struct heap_table* table = heap->table;
    for(int i = block ; i< (int)table->total ; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i]= HEAP_BLOCK_TABLE_ENTRY_FREE;
        // if this entry is the last entry in this allocation
        if(!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }

}
int heap_address_to_block(struct heap* heap , void* ptr)
{
    uint32_t offset = (uint32_t)ptr - (uint32_t)heap->saddr;
    uint32_t block = offset / BINGOS_HEAP_BLOCK_SIZE;
    return block;
}
void* heap_malloc(struct heap* heap , size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks_needed = aligned_size / BINGOS_HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap , total_blocks_needed);
}
void heap_free(struct heap* heap,void* ptr)
{
    heap_mark_blocks_free(heap , heap_address_to_block(heap ,ptr));

}