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
        case WAIT_OBJECT_0: // Input received
            hasData = true;
            break;
        case WAIT_OBJECT_0 + 1: // Exit event triggered
            cout << "Command aborted." << endl;
            exit = true;
            break;
        case WAIT_TIMEOUT:
            printf("Command failed due to server timeout: error %d\n",
                GetLastError());
            pc.SetServerError(true);
            exit = true;
            break;
        default:
            printf("Unhandled error: %d\n", GetLastError());
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
                    cout << endl;
                    cout << "Could not connect to pipe. ";
                    switch (GetLastError())
                    {
                    case ERROR_PIPE_BUSY:
                        printf("Did you reset the pipe? (error %d)\n",
                            ERROR_PIPE_BUSY);
                        break;
                    case ERROR_FILE_NOT_FOUND:
                        printf("Is the server running? (error %d)\n",
                            ERROR_FILE_NOT_FOUND);
                        break;
                    default:
                        printf("Unhandled error: %d\n", GetLastError());
                        return;
                    }
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