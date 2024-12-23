FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
# -g Enable Debugging -O0 Disables Optimization
# -ffreestanding Specifies that the program is being compiled in a freestanding environment (e.g., an OS kernel or bootloader).
# -nostdling prevents linking with stdlib
# -nostartfiles Prevents linking woth startupfiles like crt0.o
# nodefaultlibs disables linking against default libraries
# -falign-x alignes x to 16bytes code for performance
# -fstrength-reduce Enbales some sort of optimization
# -finline-functions replaces function calls with actual code to reduce overhead
# -Werror treats all warnings as errors
# -Wno Suppresseds warnings
# -fno-builtin prevents the compiler from asuming std lib functions
# -Wall Enables Most Warning Messages 
# 

# 	Solves dependencies from ./bin/boot.bin ./bin/kernel.bin first from below
all: ./bin/boot.bin ./bin/kernel.bin 
	rm -rf ./bin/os.bin
#	add the boatloader to os
	dd if=./bin/boot.bin >> ./bin/os.bin
#   add the kernel to os
	dd if=./bin/kernel.bin >> ./bin/os.bin
# 	fill in 10 sectors in zeros so we can be able to read 10 sectors
	dd if=/dev/zero bs=512 count=100 >>./bin/os.bin

# 	Assemble and compile the kernel to  bin from dependency $FILES
./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o
#	Assemble the bootloader into boot.bin 
./bin/boot.bin:	./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

# 	Assemble Kernel assembly into kernel object file
./build/kernel.asm.o:	./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o	./build/kernel.asm.o

# 	Compile Kernel C code 
./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o 

#   Assemble interrupt descriptor table into object file
./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o	./build/idt/idt.asm.o

# 	Compile Interrupt descriptor table C code
./build/idt/idt.o: ./src/idt/idt.c 
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

# 	Compile Memory C code
./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o


#   Assemble io into object file
./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o	./build/io/io.asm.o


# 	Compile Heap implmentation C code
./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

# 	Compile Kernel heap implmentation C code
./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

# 	Compile Paging C code
./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/paging $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

#   Assemble Paging assembly into object file
./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o	./build/memory/paging/paging.asm.o

# 	Compile Disk Driver C code
./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

#	Delete bin files
clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ./build/kernelfull.o
	rm -rf ${FILES}
