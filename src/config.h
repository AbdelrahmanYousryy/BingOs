#ifndef CONFIG_H
#define CONFIG_H

#define BINGOS_TOTAL_INTERRUPTS 512
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10


// Heap Definitions
#define BINGOS_HEAP_SIZE_BYTES    104857600 // heap size
#define BINGOS_HEAP_BLOCK_SIZE     4096      // block size
#define BINGOS_HEAP_ADDRESS        0x01000000 // Heap starting address
#define BINGOS_HEAP_TABLE_ADDRESS  0x00007E00  // Heap entry table address



#define BINGOS_SECTOR_SIZE 512








#endif