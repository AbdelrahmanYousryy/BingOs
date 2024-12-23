#ifndef HEAP_H
#define HEAP_H
#include "config.h"
#include <stdint.h>
#include <stddef.h>

// Definitions for each entry
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00
#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000



// Define the entry type
typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;


struct heap_table
{
    HEAP_BLOCK_TABLE_ENTRY* entries; // Pointer to the entry table
    size_t total;                    // total numbher of entries 

};

struct heap
{
    struct heap_table* table; // Ponter to heap table 
    void* saddr;              // start address of the heap data pool
};

/** Function to create Heap
 * 
 * heap : Uninitialized heap structure
 * ptr  : Address to be the start of the heap
 * end  : Address to be the end of the heap
 * table : Pointer to a valid entry table  
 */
int heap_create(struct heap* heap , void* ptr , void* end , struct heap_table* table);

/** Malloc returns a pointer to a free place in heap with suitable size  */
void* heap_malloc(struct heap* heap , size_t size);

/** Heap free frees the allocation starting in pointer ptr */
void heap_free(struct heap* heap,void* ptr);

#endif