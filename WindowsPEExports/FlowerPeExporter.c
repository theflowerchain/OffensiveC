#include <windows.h>
#include <stdio.h>
#include <string.h>

void* RvaToVa(DWORD_PTR base, DWORD offset) {
    return (void*)(base + offset);
}

void* FlowerFetchImgBase(const char* targetMod) {
    return (void*)GetModuleHandleA(targetMod);
}

void LpcstrToStr(LPCSTR lpString, char* outputBuffer, size_t bufferSize) {
    strncpy(outputBuffer, lpString, bufferSize - 1);
    outputBuffer[bufferSize - 1] = '\0';
}

void FlowerParseImgExp(const char* targetMod) {
    void* imageBase = FlowerFetchImgBase(targetMod);
    if (!imageBase) {
        printf("[FAILED TO GET MODULE BASE ADDRESS FOR] {%s}.\n", targetMod);
        return;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)RvaToVa((DWORD_PTR)imageBase, dosHeader->e_lfanew);

    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)RvaToVa(
        (DWORD_PTR)imageBase,
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
    );

    DWORD* names = (DWORD*)RvaToVa((DWORD_PTR)imageBase, exportDirectory->AddressOfNames);
    DWORD* functions = (DWORD*)RvaToVa((DWORD_PTR)imageBase, exportDirectory->AddressOfFunctions);
    WORD* ordinals = (WORD*)RvaToVa((DWORD_PTR)imageBase, exportDirectory->AddressOfNameOrdinals);

    printf("[EXPORTED FUNCTIONS FROM] {%s}:\n", targetMod);
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; i++) {
        LPCSTR functionName = (LPCSTR)RvaToVa((DWORD_PTR)imageBase, names[i]);
        WORD ordinal = ordinals[i];
        void* functionAddress = RvaToVa((DWORD_PTR)imageBase, functions[ordinal]);

        char printableName[256];
        LpcstrToStr(functionName, printableName, sizeof(printableName));
        printf("%s - [ORDINAL]: %u | [ADDRESS]: 0x%p\n", printableName, ordinal, functionAddress);
    }
}

int main() {
    FlowerParseImgExp("ntdll.dll");
    return 0;
}
