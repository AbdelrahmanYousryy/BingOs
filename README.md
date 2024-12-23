
# Developing a Multithreaded Kernel from Scratch  

## Overview  
This repository documents my journey in building a multithreaded kernel from scratch

The goal of this project is to create a basic operating system kernel with multithreading capabilities, gaining hands-on experience in:  
- Bootloader design and kernel initialization.  
- Memory management and paging.  
- Interrupt handling and system calls.  
- Process scheduling and threading.  

This repository will serve as both a learning journal and a reference for others interested in operating system development.  

---

## Features  

### Implemented  
- Basic bootloader for kernel loading.  
- Paging system for virtual memory management.  
- Initial interrupt handling for hardware and software interrupts.  

### Planned  
- Process scheduling with a round-robin algorithm.  
- Support for threading and concurrency.  
- Debugging tools for kernel internals.  
- Enhanced system call handling for user-mode processes.  

---

## Getting Started  

### Prerequisites  
- **Tools**: You’ll need a cross-compiler for the target architecture (e.g., `gcc` for x86).  
- **Environment**: QEMU or Bochs for kernel testing and debugging.  

### Cloning the Repository  
```bash
git clone https://github.com/yourusername/multithreaded-kernel.git
cd multithreaded-kernel
```

### Building the Kernel  
1. Set up your cross-compiler (instructions in `docs/cross-compiler-setup.md`).  
2. Build the project using the provided `Makefile`:  
   ```bash
   make
   ```

### Running the Kernel  
1. Use QEMU to test the kernel:  
   ```bash
   make run
   ```  

---

## Roadmap  

- [x] Bootloader implementation  
- [x] Basic paging system  
- [ ] Implement process scheduling  
- [ ] Add threading model  
- [ ] Debug system calls  

---

## Contributing  
Contributions are welcome! If you’d like to collaborate:  
1. Fork this repository.  
2. Create a feature branch:  
   ```bash
   git checkout -b feature-name
   ```  
3. Commit your changes:  
   ```bash
   git commit -m "Description of changes"
   ```  
4. Push to your fork and create a pull request.  

---

## License  
This project is licensed under the [MIT License](LICENSE).  
