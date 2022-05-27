#include "CW2.h"

void Writer::operator() ()
{
    HANDLE hEvents[] = { pc.HaveInput, pc.ExitEvent };
    bool hasData = false;
    bool exit = false;
    const char* input = nullptr;

    while (true)
    {
        switch (WaitForMultipleObjects(2, hEvents, FALSE, TIMEOUT))
        {
        case WAIT_OBJECT_0:
            hasData = true;
            break;
        case WAIT_OBJECT_0 + 1:
            cout << endl << "Writing aborted" << endl;
            exit = true;
            break;
        case WAIT_TIMEOUT:
            cout << "Writing failed. Error: " << GetLastError() << endl;
            pc.SetServerError(true);
            exit = true;
            break;
        }

        if (exit) {
            if (pc.GetConnectionStatus())
                CloseHandle(pc.PipeHandle);
            break;
        }

        if (hasData)
        {
            unique_lock<mutex> lock(mx);
            inputs i = q.front();

            switch (i)
            {
            case c:
                if (!pc.GetConnectionStatus() && !pc.ConnectPipe())
                {
                    cout << "Error: " << GetLastError() << endl;
                    return;
                }
                pc.SetCommand("ready");
                break;
            case s:
                pc.SetCommand("stop");
                pc.DisconnectPipe();
                break;
            }

            if (!pc.SendToServer()) return;

            q.pop();
            lock.unlock();
        }
    }
};