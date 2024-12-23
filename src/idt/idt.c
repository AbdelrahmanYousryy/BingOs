#include "idt.h"
#include "config.h"
#include "../memory/memory.h"
#include "kernel.h"
#include "../io/io.h"
struct idt_desc  idt_descriptors[BINGOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

// import idt_load from idt.asm
extern void idt_load(struct idtr_desc* ptr);
// import int21 from idt.asm
extern void int21h();
// import no_interrupt from idt.asm
extern void no_interrupt();


/** Default handler for initial IRQs used by no_interrupt implemented in idt.asm */
void no_interrupt_handler()
{

    outb(0x20,0x20);

}

/** Handler for interrupt 21h used by wrapper int21h implemented in idt.asm  */
void int21h_handler()
{
    print("Keyboard Pressed\n");
    outb(0x20,0x20); // send the acknowledge
}

void idt_zero()
{
    print("Divide by zerro error\n");
    
}

/** Set Interrupt Number interrupt_no to call address */
void idt_set(int interrupt_no , void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE ; // Set Type , S , DPL , P
    desc->offset_2 = (uint32_t)address >> 16;
}
void idt_init()
{
    // nullify the descriptor table
    memset(idt_descriptors,0,sizeof(idt_descriptors));
    // set the idtr to point to the descriptor table
    idtr_descriptor.limit =sizeof(idt_descriptors)-1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    //initialize all interrupts with default handler
    for (int i = 0; i < BINGOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i,no_interrupt);
    }
    

    // set the first interrupt
    idt_set(0,idt_zero);
    // set the second irq handler (Keyboard Keys Interrupt)
    idt_set(0x21,int21h);

    // load the idtr structure
    idt_load(&idtr_descriptor);

    print("IDT Initialized Succefully!\n");


}

