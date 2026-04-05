#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

int main() {
    printf("⚡ INSTANT WSL KILLER - 2 SECOND CRASH\n");
    
    // Allocate + commit 2GB in ONE SHOT
    size_t size = 2ULL << 30;  // 2GB
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_LOCKED,
                    -1, 0);
    
    if (mem == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    printf("[+] 2GB mapped & locked - committing NOW\n");
    
    // Touch EVERY page simultaneously (max pressure)
    for (size_t i = 0; i < size; i += 4096) {
        *(volatile char*)(mem + i) = 0x41;  // Single byte touch
    }
    
    printf("[+] 2GB committed - WSL DYING\n");
    
    // MADNESS: Allocate more while holding first
    void *mem2 = mmap(NULL, size*2, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    if (mem2 != MAP_FAILED) {
        for (size_t i = 0; i < size*2; i += 4096) {
            *(volatile char*)(mem2 + i) = 0x42;
        }
    }
    
    while(1);  // Hold the corpse
}
