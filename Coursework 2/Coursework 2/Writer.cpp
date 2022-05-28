#include "CW2.h"

void Writer::operator() ()
{
    HANDLE hEvents[] = { pc.HaveInput, pc.ExitEvent };
    bool hasData = false;
    bool exit = false;

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
            if (pc.GetConnectionStatus())
            {
                printf("Server has been silent for %ds. Enter 'stop' to close the connection.\n",
                    TIMEOUT / 1000);
            }
            // This is important to avoid processing an empty queue.
            continue;
        default:
            printf("Unhandled error: %d\n", GetLastError());
            exit = true;
            break;
        }

        if (exit) {
            if (pc.GetConnectionStatus())
            {
                pc.DisconnectPipe();
                CloseHandle(pc.PipeHandle);
            }
            break;
        }

        if (hasData)
        {
            scoped_lock lock(mx);
            inputs i = q.front();

            // Messages are "ready" by default, as specified when opening a connection.
            // Therefore there is no need to check for them.
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
                break;
            case s:
                pc.SetCommand("stop");
                if (pc.GetConnectionStatus())
                    pc.DisconnectPipe();
                break;
            }

            if (pc.GetConnectionStatus())
                pc.SendToServer();
            q.pop();
        }
    }
};