[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    ; Function prologue
    push ebp
    mov ebp , esp 
    
    ; Accessing Variables  
    mov eax , [ebp+8]
    mov cr3 , eax ; mov the passed page directory address to the cr3 register
    
    ; Function Epilogue 
    pop ebp
    ret


enable_paging:
    push ebp
    mov  ebp , esp
    mov  eax , cr0
    or   eax , 0x80000000 ; modify in eax because we cannnot modify directly cr0
    mov  cr0 , eax 
    pop ebp
    ret

