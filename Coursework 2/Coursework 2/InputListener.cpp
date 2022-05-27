#include "CW2.h"

void InputListener::operator() ()
{
    inputs i = c;
    string input = "";

    while (true)
    {
        if (pc.GetServerError())
        {
            cout << "Server error. Enter 'exit' to quit" << endl;
        }

        cout << ">> ";
        cin >> input;

        if (input == "connect")
        {
            if (pc.GetConnectionStatus())
                cout << "Server connection already established" << endl;
            i = c;
        }
        else if (input == "stop")
        {
            if (!pc.GetConnectionStatus())
                cout << "There is no active server connection" << endl;
            i = s;
        }
        else if (input == "exit")
        {
            i = e;
            SetEvent(pc.ExitEvent);
            return;
        }
        else
        {
            cout << "Unknown input" << endl;
            continue;
        }
        unique_lock<mutex> lock(mx);
        q.push(i);
        lock.unlock();
        cv.notify_one();
        SetEvent(pc.HaveInput);
    }
}