#include "CW2.h"

// https://docs.microsoft.com/en-gb/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
// https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes
// Spawn new process from executable.
bool start_server(STARTUPINFOA& si, PROCESS_INFORMATION& pi)
{
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    LPCSTR server_name = "ICS0025PipeServer.exe";

    if (!CreateProcessA(server_name,
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    ))
    {
        cout << "Failed to launch PipeServer. Error: " << GetLastError() << endl;
        return false;
    }

    return true;
}

void close_server(PROCESS_INFORMATION& pi)
{
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// https://docs.microsoft.com/en-gb/windows/win32/psapi/enumerating-all-processes
bool is_server_running(HANDLE& hPipeServer)
{
    // Get the list of process identifiers.
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return false;


    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] == 0) continue;
        DWORD processID = aProcesses[i];

        TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

        // Get a handle to the process.
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
            PROCESS_VM_READ,
            FALSE, processID);

        if (hProcess == NULL) continue;

        // Get the process name.
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
            &cbNeeded))
        {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                sizeof(szProcessName) / sizeof(TCHAR));
        }

        if (wcscmp(szProcessName, TEXT("ICS0025PipeServer.exe")) == 0)
        {
            // _tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);
            hPipeServer = hProcess;
            return true;
        }


        // Release the handle to the process.
        CloseHandle(hProcess);
    }

    return false;
}