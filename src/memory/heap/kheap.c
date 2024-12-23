#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
struct heap kenel_heap;
struct heap_table kernel_heap_table;

void kheap_init()
{
    // calculate the number of entries needed
    int total_table_entries = BINGOS_HEAP_SIZE_BYTES / BINGOS_HEAP_BLOCK_SIZE;
    // initialize the heap table address
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)BINGOS_HEAP_TABLE_ADDRESS;
    // initialize the heap table entries count
    kernel_heap_table.total = total_table_entries;

    // calculate end of heap
    void *end = (void*)(BINGOS_HEAP_ADDRESS+BINGOS_HEAP_SIZE_BYTES);

    int res = heap_create(&kenel_heap , (void*)BINGOS_HEAP_ADDRESS, end , &kernel_heap_table);
    if ( res < 0 )
    {
        print("Failed to create heap\n");
    }
}

void* kmalloc(size_t size)
{
    return heap_malloc(&kenel_heap , size);
}

void* kzalloc(size_t size)
{
    void* ptr = kmalloc(size);
    if(!ptr)return 0;
    memset(ptr,0x00,size);
    return ptr;

}

void kfree(void* ptr)
{
    heap_free(&kenel_heap,ptr);
}