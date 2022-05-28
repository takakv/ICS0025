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
                // Use this instead of a destructor.
                cout << endl << "Printing Data Structure:" << endl << endl;
                ds->PrintAll();
                delete ds;
                delete reply;
                return;
            case WAIT_TIMEOUT:
                break;
            }
            continue;
        }

        if (pc.ReadFromServer(reply, Overlapped, hEvents))
        {
            // Server messages are of the type: P 88 <Sudan Golden-Sparrow> 25 Jun 2018
            string s = reply;

            // https://cplusplus.com/reference/sstream/stringstream/stringstream/
            stringstream ss(s);
            char group;
            int subgroup;
            ss >> group >> subgroup;

            size_t start = s.find('<') + 1;
            size_t end = s.find('>');
            string name = s.substr(start, end - start);

            stringstream ss_timedate(s.substr(end + 2));
            int d, y;
            string m;
            ss_timedate >> d >> m >> y;

            // From Date.cpp
            vector<string> months = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
            size_t month = distance(months.begin(), find(months.begin(), months.end(), m)) + 1;

            Date* date = new Date(d, month, y);

            ds->InsertItem(group, subgroup, name, *date);

            unique_lock lock(mx);
            q.push(r);
            lock.unlock();
            cv.notify_one();
            SetEvent(pc.HaveInput);
        }
    }
    CloseHandle(Overlapped.hEvent);
    // delete reply;
}

// The destructor approach is an exception nightmare.
/*
Reader::~Reader()
{
    ds->PrintAll();
    // delete ds;
    // delete reply;
}
*/