#pragma once

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "PipeClient.h"

#define BUFSIZE 512
#define TIMEOUT 60*1000

using namespace std;

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
    unsigned long nRead = 0;
    char* reply = new char[BUFSIZE];
public:
    void operator() ();
};