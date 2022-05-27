#pragma once

#include <windows.h>
#include <iostream>
#include <string>

class PipeClient
{
private:
    LPCSTR lpszPipename = "\\\\.\\pipe\\ICS0025";
    bool connected = false;
    bool serverError = false;
    const char* command = nullptr;
public:
    PipeClient();
    ~PipeClient() = default;
    HANDLE PipeHandle;
    HANDLE HaveInput;
    HANDLE ExitEvent;
    bool ConnectPipe();
    bool DisconnectPipe();
    bool SendToServer();
    bool GetConnectionStatus();
    bool GetServerError();
    void SetServerError(bool state);
    void SetCommand(const char* command);
};