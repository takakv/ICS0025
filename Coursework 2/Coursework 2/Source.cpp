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

int main()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    // start_server(si, pi);

    PipeClient pc;
    queue<inputs> q;
    mutex mx;
    condition_variable cv;

    thread listener_thread{ InputListener(ref(q), ref(pc), mx, cv) };
    thread sender_thread{ Writer(ref(q), ref(pc), mx, cv) };
    thread receiver_thread{ Reader(ref(q), ref(pc), mx, cv) };

    listener_thread.join();
    sender_thread.join();
    receiver_thread.join();

    return 0;
}