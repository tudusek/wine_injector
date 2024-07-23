#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

int injectDll(PROCESS_INFORMATION pi, string dll) {
    cout << "Loading " << dll << endl;
    LPVOID alloc = VirtualAllocEx(pi.hProcess, NULL, dll.length(), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        printf("VirtualAllocEx failed.\n");
        return EXIT_FAILURE;
    }

    if (WriteProcessMemory(pi.hProcess, alloc, dll.c_str(), dll.length(), NULL) == 0) {
        printf("WriteProcessMemory failed.\n");
        return EXIT_FAILURE;
    }

    HANDLE handle = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, alloc, 0, NULL);
    if (!handle) {
        printf("CreateRemoteThread failed.\n");
        return EXIT_FAILURE;
    }

    WaitForSingleObject(handle, INFINITE);
    VirtualFreeEx(pi.hProcess, alloc, 0, MEM_RELEASE);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "Usage: wine_injector.exe [args] -- (executable) (optional args to exe)\n"
             << "--edll dllpath\tearly dll load\n"
             << "--ldll dllpath\tlate dll load\n";
        return EXIT_FAILURE;
    }

    STARTUPINFOA si = {.cb = sizeof si};
    PROCESS_INFORMATION pi = {0};
    vector<string> edlls;
    vector<string> ldlls;

    int separator = 0;
    int parseState = 0;
    for (int i = 1; i < argc && !separator; i++) {
        switch (parseState) {
            case 1:
                edlls.push_back(argv[i]);
                parseState = 0;
                break;
            case 2:
                ldlls.push_back(argv[i]);
                parseState = 0;
                break;
            default:
                if (!strcmp("--", argv[i])) {
                    separator = i;
                } else if (!separator && !strcmp("--edll", argv[i])) {
                    parseState = 1;
                } else if (!separator && !strcmp("--ldll", argv[i])) {
                    parseState = 2;
                }
                break;
        }
    }

    int argcount = argc - separator - 2;
    cout << "separator: " << separator << endl;
    cout << "edllcount: " << edlls.size() << endl;
    cout << "ldllcount: " << ldlls.size() << endl;
    cout << "argcount: " << argcount << endl;
    if (!edlls.size() && !ldlls.size()) {
        cout << "no dlls specified\n";
        return EXIT_FAILURE;
    }
    if (argcount < 0) {
        cout << "no executale specified\n";
        return EXIT_FAILURE;
    }

    string cmdargs;

    for (int i = separator + 1; i < argc; i++) {
        cmdargs += argv[i];
        cmdargs += i == (argc - 1) ? "" : " ";
    }

    if (CreateProcessA(argv[separator + 1], (LPSTR)cmdargs.c_str(), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi) == 0) {
        cout << "CreateProcessA failed.\n";
        return EXIT_FAILURE;
    }
    cout << "PID: " << pi.dwProcessId << endl;

    // early dll load
    for (string dll : edlls) {
        injectDll(pi, dll);
    }

    ResumeThread(pi.hThread);

    // late dll load
    if (ldlls.size()) {
        cout << "Press key to inject late dlls" << endl;
        cin.get();
        for (string dll : ldlls) {
            injectDll(pi, dll);
        }
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
