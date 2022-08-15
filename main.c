#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: wine_injector.exe (executable) (dll) (optional arguments to executable)\n");
        return EXIT_FAILURE;
    }

    STARTUPINFOA si = {.cb = sizeof si};
    PROCESS_INFORMATION pi = {0};

    char *args = NULL;
    if (argc > 3) {
        size_t size_of_args = 0;

        for (int i = 3; i < argc; i++)
            size_of_args += strlen(argv[i]) + 1; // argv + space/null

        if (!(args = (char *)malloc(size_of_args))) {
            printf("Failed to allocate args.");
            return EXIT_FAILURE;
        }
        ZeroMemory(args, size_of_args);

        for (int i = 3; i < argc; i++) {
            memcpy(args + strlen(args), argv[i], strlen(argv[i]));        // append argv
            memcpy(args + strlen(args), i == (argc - 1) ? "\0" : " ", 1); // if its the last arg copy null, otherwise copy space
        }
    }

    if (CreateProcessA(argv[1], args, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0) {
        printf("CreateProcessA failed.\n");
        return EXIT_FAILURE;
    }

    if (args)
        free(args);

    printf("Press ENTER to inject.\n");
    (void)getchar();

    LPVOID alloc = VirtualAllocEx(pi.hProcess, NULL, strlen(argv[2]), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        printf("VirtualAllocEx failed.\n");
        return EXIT_FAILURE;
    }

    if (WriteProcessMemory(pi.hProcess, alloc, argv[2], strlen(argv[2]), NULL) == 0) {
        printf("WriteProcessMemory failed.\n");
        return EXIT_FAILURE;
    }

    HANDLE handle = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, alloc, 0, NULL);
    if (!handle) {
        printf("CreateRemoteThread failed.\n");
        return EXIT_FAILURE;
    }

    WaitForSingleObject(handle, 5000);
    VirtualFreeEx(pi.hProcess, alloc, 0, MEM_RELEASE);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
