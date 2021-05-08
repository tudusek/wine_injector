#include <iostream>
#include <windows.h>

int main(int argc, char* argv[]) {
    if (argc < 3)
        return printf("Usage: wine_injector.exe <executable> <dll>\n");

    std::string exe = argv[1];
    std::string dll = argv[2];

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    CreateProcessA(nullptr, argv[1], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

    Sleep(2500);

    LPVOID alloc = VirtualAllocEx(pi.hProcess, nullptr, dll.size(), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(pi.hProcess, alloc, dll.data(), dll.size(), nullptr);
    CreateRemoteThread(pi.hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE) LoadLibrary, alloc, 0, nullptr);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
