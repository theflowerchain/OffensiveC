#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

THREADENTRY32* getProcessThreads(DWORD procId, size_t* count) {
    *count = 0;
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    size_t threadsCount = 0;
    if (Thread32First(snapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == procId) {
                threadsCount++;
            }
        } while (Thread32Next(snapshot, &te32));
    }

    THREADENTRY32* threads = (THREADENTRY32*)malloc(threadsCount * sizeof(THREADENTRY32));
    if (threads == NULL) {
        CloseHandle(snapshot);
        return NULL;
    }

    te32.dwSize = sizeof(THREADENTRY32);
    size_t index = 0;
    if (Thread32First(snapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == procId) {
                threads[index++] = te32;
            }
        } while (Thread32Next(snapshot, &te32));
    }

    *count = threadsCount;
    CloseHandle(snapshot);
    return threads;
}

int main() {
    DWORD procId = GetCurrentProcessId();
    size_t count;
    THREADENTRY32* threads = getProcessThreads(procId, &count);

    if (threads == NULL) {
        printf("[FAILED TO RETRIEVE THREADS]\n");
        return 1;
    }

    printf("[FOUND] %zu threads in process %lu:\n", count, procId);
    for (size_t i = 0; i < count; i++) {
        printf("Thread ID: %lu\n", threads[i].th32ThreadID);
    }

    free(threads);
    return 0;
}
