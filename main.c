#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[]) {
    if (argc < 3)
        return printf("Usage: wine_injector.exe <executable> <dll>\n");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    CreateProcessA(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    printf("Press ENTER to inject.");
    getchar();

    LPVOID alloc = VirtualAllocEx(pi.hProcess, NULL, strlen(argv[2]), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(pi.hProcess, alloc, argv[2], strlen(argv[2]), NULL);
    CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, alloc, 0, NULL);

    WaitForSingleObject(pi.hProcess, INFINITE);
    VirtualFreeEx(pi.hProcess, alloc, 0, MEM_RELEASE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
