section .asm

extern int21h_handler   ; import int21_handler
global int21h   ; export int21h

extern no_interrupt_handler ; import no_interrupt_handller
global no_interrupt     ; export no_interrupt
global idt_load ; export this idt_load
global enable_interrupts ; export enable_interrupts
global disable_interrupts ; export disable_interrupys

enable_interrupts:
    sti     ; enable interrupts
    ret     
    
disable_interrupts:
    cli     
    ret
    
idt_load:
    push ebp ; push the pointer to the current stack
    mov ebp , esp ; get the stack of the function

    mov ebx , [ebp+8] ; move the first parameter value to ebx
    lidt [ebx]        ; interrupt descriptor table in ebx

    pop ebp           ; pop the prev stack
    ret               ; return

int21h:
    cli     ; Disables Interrupts
    pushad ; Push all the GPRS
    call int21h_handler  
    popad   ; Pop all the GPRS
    sti     ; enables the interrupts
    iret    ; ret interrupt

no_interrupt:
    cli     ; Disables Interrupts
    pushad ; Push all the GPRS
    call no_interrupt_handler  
    popad   ; Pop all the GPRS
    sti     ; enables the interrupts
    iret    ; ret interrupt
