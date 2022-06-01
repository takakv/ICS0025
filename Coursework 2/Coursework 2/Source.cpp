#include "CW2.h"

/*
* The downside of this implementation is that the threads remain
* active even when the pipe is not connected, and wait for a subsequent
* connection. A better alternative would be to kill the sender and receiver
* threads when the stop message is sent to the server, and recreate
* the threads upon next connection.
*/

int main()
{
    HANDLE hProcess = nullptr;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    string input = "";
    bool auto_start = false;

    if (!is_server_running(hProcess))
    {
        cout << "Start the server automatically? (y/n)" << endl << ">> ";
        cin >> input;

        if (input == "y" || input == "Y")
        {
            auto_start = true;
            if (!start_server(si, pi))
            {
                cout << endl;
                cout << "Please start the server manually." << endl;
            }
            else hProcess = pi.hProcess;
        }
    }

    queue<inputs> q;
    mutex mx;
    condition_variable cv;

    PipeClient pc(&mx);

    thread listener_thread{ InputListener(ref(q), ref(pc), mx, cv) };
    thread sender_thread{ Writer(ref(q), ref(pc), mx, cv) };
    thread receiver_thread{ Reader(ref(q), ref(pc), mx, cv) };

    listener_thread.join();
    sender_thread.join();
    receiver_thread.join();

    if (!auto_start && !is_server_running(hProcess)) return 0;
    else
    {
        DWORD lpExitcode = 0;
        GetExitCodeProcess(hProcess, &lpExitcode);
        if (lpExitcode != STILL_ACTIVE)
        {
            if (auto_start) close_server(pi);
            else CloseHandle(hProcess);
            return 0;
        }
    }

    cout << "Terminate the server? (y/n)" << endl << ">> ";
    cin >> input;

    if (input == "y" || input == "Y")
    {
        if (!auto_start)
            hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, GetProcessId(hProcess));

        TerminateProcess(hProcess, 0);
    }
    else
    {
        cout << "Waiting for the server to be closed manually..." << endl;
        WaitForSingleObject(hProcess, INFINITE);
    }

    if (auto_start) close_server(pi);
    else CloseHandle(hProcess);

    return 0;
}