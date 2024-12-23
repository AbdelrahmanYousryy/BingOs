#ifndef KHEAP_H
#define KHEAP_H
#include <stdint.h>
#include <stddef.h>
void kfree(void* ptr);
void kheap_init();
void* kzalloc(size_t size);
void* kmalloc(size_t size);


#endif