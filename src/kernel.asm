[BITS 32]
; export the symbol start for the  llinker
global _start

extern kernel_main
CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    ; set the segments
    mov ax , DATA_SEG
    mov ds , ax
    mov es , ax
    mov fs , ax
    mov gs , ax
    mov ss , ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; enable the A20 line
    in al , 0x92
    or al , 2
    out 0x92 , al


    ; remap the master PIC
    mov al , 00010001b  ; Initialization configuration for the PIC
    out 0x20 , al       ; Send the Configuration to PIC1 command port
    mov al , 0x20       ; Remaping value Interrupt 0x20 is where master ISR should  start
    out 0x021 , al      ; Send the offset to data port of PIC1
    mov al , 00000001b  ; 
    out 0x21 , al       ; Use the 8086 mode c
    ; end of remapping the master pic

    ; jump to kernel_main function in c code
    call kernel_main
    jmp $

; 
; Solves Allignment Issues
times 512-($ - $$) db 0