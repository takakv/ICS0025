#include "CW2.h"

void Reader::operator() ()
{
    OVERLAPPED Overlapped;
    memset(&Overlapped, 0, sizeof Overlapped);
    Overlapped.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    HANDLE hEvents[] = { Overlapped.hEvent, pc.ExitEvent };

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

        if (pc.ReadFromServer(reply, Overlapped, hEvents))
        {
            string str = reply;

            cout << str << endl;

            unique_lock lock(mx);
            q.push(r);
            lock.unlock();
            cv.notify_one();
            SetEvent(pc.HaveInput);
        }
    }
    CloseHandle(Overlapped.hEvent);
    delete reply;
}