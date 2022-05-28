#include "CW2.h"

void stop_server(PipeClient& pc)
{
    pc.SetCommand("stop");
    if (pc.GetConnectionStatus())
        pc.SendToServer();
    pc.DisconnectPipe();
}

void Writer::operator() ()
{
    HANDLE hEvents[] = { pc.HaveInput, pc.ExitEvent };
    bool exit = false;

    while (true)
    {
        switch (WaitForMultipleObjects(2, hEvents, FALSE, TIMEOUT))
        {
        case WAIT_OBJECT_0: // Input received
            break;
        case WAIT_OBJECT_0 + 1: // Exit event triggered
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
            stop_server(pc);
            CloseHandle(pc.PipeHandle);
            return;
        }

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
                    cout << "Did you reset the pipe?" << endl;
                    break;
                case ERROR_FILE_NOT_FOUND:
                    cout << "Is the server running?" << endl;
                    break;
                default:
                    printf("Unhandled error: %d\n", GetLastError());
                    return;
                }
                cout << ">> ";
            }
            break;
        case s:
            stop_server(pc);
            break;
        }

        if (pc.GetConnectionStatus())
            pc.SendToServer();

        q.pop();
    }
};