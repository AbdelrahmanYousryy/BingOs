section .asm


global insb  ; export insb
global insw  ; export insw
global outb  ; export outb
global outw  ; export outw

insb:
    push ebp        ; push the base pointer
    mov ebp , esp   ; get the current stack pointer
    xor eax , eax   ; zero the ax register
    mov edx , [ebp+8] ; move the path (first argument to edx)
    in al , dx       ; input 1 byte from port saved in edx
    pop ebp           ; pop the  base pointer
    ret                 ; returning ax as default


insw:
    push ebp        ; push the  base pointer
    mov ebp , esp   ; get the current stack pointer
    xor eax , eax   ; zero the ax register
    mov edx , [ebp+8] ; move the path (first argument to edx)
    in ax , dx       ; input 2 bytes from port saved in edx
    pop ebp           ; pop the  base pointer
    ret                 ; returning ax as default

    
outb:
    push ebp        ; push the  base pointer
    mov ebp , esp   ; get the stack pointer

    mov eax , [ebp+12] ; mov the second argument (the value) to ax
    mov edx , [ebp+8]  ; mov the first argument (the port) to dx
    out dx , al        ; output the value to port in dx
    pop ebp            ; pop the old base pointer
    ret

outw:
    push ebp        ; push the  base pointer
    mov ebp , esp   ; get the stack pointer

    mov eax , [ebp+12] ; mov the second argument (the value) to ax
    mov edx , [ebp+8]  ; mov the first argument (the port) to dx
    out dx , ax        ; output the value to port in dx
    pop ebp            ; pop the old base pointer
    ret