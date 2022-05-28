#pragma once

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../../common/common.h"
#include "../../Coursework 1/Coursework 1/Data.h"

#include "PipeClient.h"

enum inputs { c, r, s, e };

class ThreadBase
{
protected:
    queue<inputs>& q;
    PipeClient& pc;
    mutex& mx;
    condition_variable& cv;
public:
    ThreadBase(queue<inputs>& q, PipeClient& pc, mutex& m, condition_variable& c) : q(q), pc(pc), mx(m), cv(c) {};
};

class Writer : public ThreadBase
{
    using ThreadBase::ThreadBase;
public:
    void operator() ();
};

class InputListener : public ThreadBase
{
    using ThreadBase::ThreadBase;
public:
    void operator() ();
};

class Reader : public ThreadBase
{
    using ThreadBase::ThreadBase;
private:
    Data* ds = new Data();
    char* reply = new char[BUFSIZE];
public:
    void operator() ();
    // ~Reader();
};

bool start_server(STARTUPINFOA& si, PROCESS_INFORMATION& pi);
void close_server(PROCESS_INFORMATION& pi);
bool is_server_running(HANDLE& hPipeServer);