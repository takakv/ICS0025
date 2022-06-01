#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <mutex>

#define BUFSIZE 512
#define TIMEOUT 11*1000

class PipeClient
{
private:
    LPCSTR lpszPipename = "\\\\.\\pipe\\ICS0025";
    bool connected = false;
    const char* command = nullptr;
    std::mutex* mx; // Avoid race conditions when error logging.
public:
    PipeClient(std::mutex* mx);
    ~PipeClient() = default;
    HANDLE PipeHandle;
    HANDLE HaveInput;
    HANDLE ExitEvent;
    bool ConnectPipe();
    void DisconnectPipe();
    bool SendToServer();
    bool ReadFromServer(char* reply, OVERLAPPED Overlapped, HANDLE hEvents[]);
    bool GetConnectionStatus();
    void SetCommand(const char* command);
};