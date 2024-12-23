#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

struct idt_desc
{
    uint16_t offset_1; // offset bits 0-15
    uint16_t selector ; // selector thats in our gdt
    uint8_t  zero ; // unused filled with 0
    uint8_t type_attr; // Descriptor Type and attributes
    uint16_t offset_2 ; // offset bits 16-31
    

} __attribute__((packed));


struct idtr_desc
{
    uint16_t limit ; // size of the descriptor
    uint32_t base ; // Base address of the interrupt descriptor table
} __attribute__((packed));


void idt_init();
void enable_interrupts();
void disable_interrupts();








#endif