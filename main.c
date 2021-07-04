#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[]) {
    if (argc < 3)
        return printf("Usage: wine_injector.exe <executable> <dll>\n");

    STARTUPINFO si = {.cb = sizeof si};
    PROCESS_INFORMATION pi = {0};
    CreateProcessA(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    printf("Press ENTER to inject.");
    getchar();

    LPVOID alloc = VirtualAllocEx(pi.hProcess, NULL, strlen(argv[2]), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(pi.hProcess, alloc, argv[2], strlen(argv[2]), NULL);
    HANDLE handle = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, alloc, 0, NULL);
    WaitForSingleObject(handle, 5000);
    VirtualFreeEx(pi.hProcess, alloc, 0, MEM_RELEASE);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
