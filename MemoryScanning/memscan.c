#include <windows.h>
#include <stdio.h>

typedef struct _MEMORY_BASIC_INFORMATION MEMORY_BASIC_INFORMATION;

MEMORY_BASIC_INFORMATION* getProcessMemoryRegions(HANDLE hProcess, size_t* count) {
    *count = 0;
    MEMORY_BASIC_INFORMATION mbi;
    PVOID baseAddr = 0;
    size_t regionsCount = 0;

    while (VirtualQueryEx(hProcess, baseAddr, &mbi, sizeof(mbi)) != 0) {
        regionsCount++;
        baseAddr = (PVOID)((SIZE_T)mbi.BaseAddress + mbi.RegionSize);
    }

    MEMORY_BASIC_INFORMATION* memRegions = (MEMORY_BASIC_INFORMATION*)malloc(regionsCount * sizeof(MEMORY_BASIC_INFORMATION));
    if (memRegions == NULL) {
        return NULL;
    }

    baseAddr = 0;
    size_t index = 0;
    while (VirtualQueryEx(hProcess, baseAddr, &mbi, sizeof(mbi)) != 0) {
        memRegions[index++] = mbi;
        baseAddr = (PVOID)((SIZE_T)mbi.BaseAddress + mbi.RegionSize);
    }

    *count = regionsCount;
    return memRegions;
}

int main() {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (hProcess == NULL) {
        printf("[Failed to open process. \n]");
        return 1;
    }

    size_t count;
    MEMORY_BASIC_INFORMATION* regions = getProcessMemoryRegions(hProcess, &count);
    if (regions == NULL) {
        printf("[Failed to get memory region \n]");
        CloseHandle(hProcess);
        return 1;
    }

    for (size_t i = 0; i < count; i++) {
        printf("[BASE ADDRESS]: %p, [REGION SIZE]: %zu\n", regions[i].BaseAddress, regions[i].RegionSize);
    }

    free(regions);
    CloseHandle(hProcess);
    return 0;
}
