; Bios loads the bootloader to 0x7c00
ORG 0x7c00
; using 16 bits code
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


; BIOS Parameter Block
_start:
    jmp short start
    nop

times 33 db 0
; start label
start:
    ; changing code segment to 0x7c0
    jmp 0 : step2

step2:
    cli ; clear Interrupts
    ; setiing the segments manually to make sure its in 0x7c00
    mov ax , 0x00
    mov ds , ax
    mov es , ax
    mov ss , ax
    mov sp , 0x7c00
    sti              ; Enable Interrupts
    
.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax , cr0
    or eax , 0x1
    mov cr0 , eax
    jmp CODE_SEG:load_32
    jmp $
; Creating GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:        ; Defining The code Segment
    dw 0xffff    ; segment limit first 0-15 bits
    dw 0         ; Base First 0-15 bits
    db 0         ; Base 16 - 32 bits
    db 0x9a      ; access byte
    db 11001111b ; High 4 bit Flags and low 4 bit flags 
    db 0         ; Base 24 - 31 bits

; offset 0x10 
gdt_data:        ; Defining The Data Segment
    dw 0xffff    ; Segment limit first 0-15 bits
    dw 0         ; Base First 0-15 bits
    db 0         ; Base 16 - 32 bits
    db 0x92      ; access byte
    db 11001111b ; High 4 bit Flags and low 4 bit flags 
    db 0         ; Base 24 - 31 bits

gdt_end:



gdt_descriptor:
    dw gdt_end - gdt_start -1
    dd gdt_start

[BITS 32]
load_32:
    ; load the kernel into memory
    mov eax , 1         ; The starting sector beacuse 0 is for the bootloader
    mov ecx , 100       ; total number of sectors
    mov edi , 0x0100000 ; the address in memory we want to load the kernel to
    call ata_lba_read   ; load the kernel
    jmp CODE_SEG:0x0100000 ; jump to kernel code

; now the driver for talking to ATA controller
ata_lba_read:
    mov ebx, eax       ; Back up the full 32-bit LBA value into EBX for later use.
    shr eax, 24        ; Shift the LBA value 24 bits to the right, leaving only the highest 8 bits in AL.
    or eax , 0xE0      ; selects the master drive
    mov dx, 0x1F6      ; Set DX to the I/O port address (0x1F6) that expects the highest 8 bits of the LBA.
    out dx, al         ; Write the value in AL (the highest 8 bits of the original LBA) to the port.

    mov eax , ecx      ; load ax with the total sectors to read
    mov dx , 0x1F2     ; Set DX to the I/O port address (0x1F2) that expects the total number of sectors to read. 
    out dx , al        ; write the value of al (the total sectors to read) into the port

    ; Sending more bits of the LBA
    mov eax , ebx       
    mov dx , 0x1F3      
    out dx , al
    ; Sending more bits of the LBA
    mov eax , ebx
    mov dx , 0x1F4
    shr eax , 8
    out dx , al
    ; Sending more bits of the LBA
    mov eax , ebx
    mov dx , 0x1F5
    shr eax , 16
    out dx , al

    ; Sending the Command
    mov dx , 0x1F7
    mov al , 0x20
    out dx , al


   ; Read all sectors into memory  
.next_sector:
    push ecx


; Checking if we need to read
.try_again:
    mov dx , 0x1F7 ; read data from this port
    in al , dx     ; save the data into al
    test al , 8    ; check for this mask
    jz .try_again  ; if fails try again

; We need to read 256 words at a time
    mov ecx , 256
    mov dx , 0x1F0
    rep insw     ; reads a word from the port specified in dx and into the memory location ES:DI
    ; rep repeats this ecx times
    ; so basically the latest 3 lines says to read from 0x1F0 a word 256 times which is 512 bytes
    ; which is 1 sector into the place specified in es:di which is 0x0100000 which is the place 
    ; we want to load the kernel into
    pop ecx ; restore ecx , now has the total number of sectors wanted
    loop .next_sector ; this loops from the .next_sector 100 times to read 100 sectors
    ; loop will loop from .next_sector ecx times, every time we start by pushing ecx 
    ; to keep its loop count and uses this count when looping by popping it again

    ; end of reading sectors
    ret



times 510-($ - $$) db 0
dw 0xAA55