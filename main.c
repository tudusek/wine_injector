#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static char *concat_args(char *exe_name, char *args[], size_t args_len) {
    // Also have to prepend executable name or Windows will pass wrong argv[0].
    // The MSDN documentation mentions this, but in a really unclear way.
    const size_t exe_len = strlen(exe_name);
    char *out = malloc(exe_len + 1);
    memcpy(out, exe_name, exe_len);
    size_t offset = exe_len + 1;
    out[offset - 1] = ' ';

    // Concatenate the actual arguments
    for (size_t i = 0; i < args_len; i++) {
        char *arg = args[i];
        const size_t arg_len = strlen(arg);

        out = realloc(out, offset + arg_len + 1);  // + 1 for space
        memcpy(out + offset, arg, arg_len);
        offset += arg_len + 1;  // + 1 for space
        out[offset - 1] = ' ';
    }
    // Replace final space w/ terminator
    out[offset - 1] = '\0';
    return out;
}

int main(int argc, char *argv[]) {
    if (argc < 3)
        return (printf("Usage: wine_injector.exe <executable> <dll> <args for "
                       "executable (optional)>\n") &
                0) |
               1;

    STARTUPINFO si = {.cb = sizeof si};
    PROCESS_INFORMATION pi = {0};
    char *cmdline = NULL;
    if (argc > 3) {
        cmdline = concat_args(argv[1], argv + 3, argc - 3);
        printf("cmdline: '%s'\n", cmdline);
    }
    if (CreateProcessA(argv[1], cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0) {
        fprintf(stderr, "wine_injector: CreateProcessA failed!\n");
        return EXIT_FAILURE;
    }

    free(cmdline);

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
