#include "CW2.h"

void Reader::operator() ()
{
    OVERLAPPED Overlapped;
    memset(&Overlapped, 0, sizeof Overlapped);
    Overlapped.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    HANDLE hEvents[] = { Overlapped.hEvent, pc.ExitEvent };
    bool hasData = false;
    bool exit = false;

    while (true)
    {
        if (!pc.GetConnectionStatus())
        {
            switch (WaitForSingleObject(pc.ExitEvent, 1000))
            {
            case WAIT_OBJECT_0:
                return;
            case WAIT_TIMEOUT:
                break;
            }
            continue;
        }

        if (!ReadFile(pc.PipeHandle, reply, BUFSIZE, &nRead, &Overlapped))
        {
            int error = GetLastError();
            switch (error)
            {
            case ERROR_IO_PENDING:
                switch (WaitForMultipleObjects(2, hEvents, FALSE, TIMEOUT))
                {
                case WAIT_OBJECT_0:
                    GetOverlappedResult(pc.PipeHandle, &Overlapped, &nRead, FALSE);
                    hasData = true;
                    break;
                case WAIT_OBJECT_0 + 1:
                    cout << "Reading broken off" << endl;
                    exit = true;
                    break;
                case WAIT_TIMEOUT:
                    cout << "Timeout period " << TIMEOUT << "ms elapsed, nothing was received." << endl;
                    exit = true;
                    break;
                default: cout << "Rading failed, error " << GetLastError() << endl;
                    exit = true;
                    pc.SetServerError(true);
                    break;
                }
                break;
            default:
                cout << "Reading failed, error " << GetLastError() << endl;
                exit = true;
                pc.SetServerError(true);
                break;
            }
        }
        else hasData = true;

        if (exit) break;

        if (hasData)
        {
            string str = reply;

            cout << str << endl;

            unique_lock<mutex> lock(mx);
            q.push(r);
            lock.unlock();
            cv.notify_one();
            SetEvent(pc.HaveInput);
        }
    }
    CloseHandle(Overlapped.hEvent);
    delete reply;
}